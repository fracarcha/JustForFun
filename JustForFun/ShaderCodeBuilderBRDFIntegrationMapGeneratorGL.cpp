/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderBRDFIntegrationMapGeneratorGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderBRDFIntegrationMapGeneratorGL")
}

JFF::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL::~ShaderCodeBuilderBRDFIntegrationMapGeneratorGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderBRDFIntegrationMapGeneratorGL")
}

void JFF::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL::getShaderVersionLine(const Params& params) const
{
	static std::string versionCode =
		R"glsl(
			#version @1@2@3 @4
		)glsl";

	std::regex reMajor("@1");
	std::regex reMinor("@2");
	std::regex reRev("@3");
	std::regex reProfile("@4");

	std::string version = std::regex_replace(versionCode, reMajor, std::to_string(params.shaderVersionMajor));
	version = std::regex_replace(version, reMinor, std::to_string(params.shaderVersionMinor));
	version = std::regex_replace(version, reRev, std::to_string(params.shaderVersionRevision));
	version = std::regex_replace(version, reProfile, params.shaderProfile);

	return version;
}

inline std::string JFF::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL::getVertexShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			layout (location = 0) in vec3 vertexPosModelSpace;
			layout (location = 1) in vec3 normalModelSpace;
			layout (location = 2) in vec3 tangentModelSpace;
			layout (location = 3) in vec3 bitangentModelSpace;
			layout (location = 4) in vec3 uvModelSpace;

			out VertexShaderOutput
			{
				vec3 uv; // Used X as NdotWo and Y as roughness
			} jff_output;

			void main()
			{
				// A plane mesh is drawn and uv coordinates are used to interpolate NdotWo and roughness to get all possible combinations
				jff_output.uv = uvModelSpace;

				// Position inside the cube and looking to each view direction in a projection of 90 degrees
				gl_Position = vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec3 uv;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			#define PI 3.14159265359

			// -------------------------- LOW DISCREPANCY RANDOM NUMBER GENERATOR ------------------------- //

			float radicalInverseVanDerCorpus(uint bits)
			{
				bits = (bits << 16u) | (bits >> 16u);
				bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
				bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
				bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
				bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
				
				return float(bits) * 2.3283064365386963e-10; // / 0x100000000
			}

			vec2 hammersley(uint i, uint N)
			{
				// This generates points in pseudo-random locations, but more evenly distributed in a surface
				return vec2(float(i) / float(N), radicalInverseVanDerCorpus(i));
			}

			// ------------------------- NORMAL DISTRIBUTION FUNCTION ------------------------- //

			/* 
			Function used to generate the halfway vector statistically using low-discrepancy random number generator
			combined with Normal Distribution Function to use roughness to make the halfway vector point around the 
			specular direction.
			*/
			vec3 importanceSampleNDF(vec2 randomPos, vec3 normalTangentSpace, float roughness)
			{
				// Use Normal Distribution Function to make the halfway vector 'focus' on the surface reflection (be part of a 'lobe')
				float alpha = roughness * roughness; // Use squared roughtness for better visual results
				float alpha2 = alpha * alpha;

				float phi = 2.0 * PI * randomPos.x;
				float cosTheta = sqrt((1.0 - randomPos.y) / (1.0 + (alpha2 - 1.0) * randomPos.y));
				float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

				// From spherical coordinates to cartesian coordinates
				vec3 halfwayDirTangentSpace = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
				
				// Build TBN matrix
				vec3 B = abs(normalTangentSpace.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
				vec3 T = normalize(cross(B, normalTangentSpace));
				B = cross(normalTangentSpace, T);
				mat3 TBN = mat3(T, B, normalTangentSpace);

				// From tangent to world space
				vec3 halfwayDirWorldSpace = normalize(TBN * halfwayDirTangentSpace);
				
				return halfwayDirWorldSpace;
			}

			// ------------------------- GEOMETRY FUNCTION ------------------------- //

			float geometrySimple(float dotProduct, float roughness)
			{
				float k = pow(roughness, 2.0) / 2.0; // Exclusive function for indirect lighting
				return dotProduct / (dotProduct * (1.0 - k) + k);
			}

			float geometry(float NdotV, float NdotL, float roughness)
			{
				float G_shadowing = geometrySimple(NdotL, roughness);
				float G_obstruction = geometrySimple(NdotV, roughness);
				return G_obstruction * G_shadowing;
			}

			// ------------------------- BRDF INTEGRATION MAP GENERATOR	 ------------------------- //

			vec2 BRDFIntegration(float NdotV, float roughness)
			{
				float factor = 0.0;
				float bias = 0.0;

				// Extract view dir and normal in tangent space
				vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
				vec3 N = vec3(0.0, 0.0, 1.0);

				const uint SAMPLE_COUNT = 1024u;
				for(uint i = 0u; i < SAMPLE_COUNT; ++i)
				{
					vec2 randomPos = hammersley(i, SAMPLE_COUNT);
					vec3 H = importanceSampleNDF(randomPos, N, roughness);
					vec3 L = normalize(2.0 * dot(V, H) * H - V);

					float NdotL = max(dot(N, L), 0.0);
					float NdotH = max(dot(N, H), 0.0);
					float VdotH = max(dot(V, H), 0.0);

					if(NdotL > 0.0)
					{
						float G = geometry(NdotV, NdotL, roughness);
						float G_vis = (G * VdotH) / (NdotH * NdotV);
						float Fc = pow(1.0 - VdotH, 5.0);

						factor += (1.0 - Fc) * G_vis;
						bias += Fc * G_vis;
					}
				}
				factor /= float(SAMPLE_COUNT);
				bias /= float(SAMPLE_COUNT);
				
				return vec2(factor, bias);
			}

			// ------------------------- MAIN FUNCTION ------------------------- //

			void main()
			{
				vec2 BRDFIntegrated = BRDFIntegration(jff_input.uv.x, jff_input.uv.y);
				FragColor = vec4(BRDFIntegrated, 0.0, 1.0);
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
