/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL")
}

JFF::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL::~ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL")
}

void JFF::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL::getVertexShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			layout (location = 0) in vec3 vertexPosModelSpace;
			layout (location = 1) in vec3 normalModelSpace;
			layout (location = 2) in vec3 tangentModelSpace;
			layout (location = 3) in vec3 bitangentModelSpace;
			layout (location = 4) in vec3 uvModelSpace;

			// NOTE: Model matrix is not included because the cube is not displaced, rotated or scaled around the world
			uniform mat4 viewMatrix;
			uniform mat4 projectionMatrix;

			out VertexShaderOutput
			{
				vec3 vertexPosModelSpace; // Used as vector to point inside a cubemap
			} jff_output;

			void main()
			{
				// The goal here is to draw all faces of a cube (from inside the cube, given all 6 viewMatrices)
				// and map irradiance texture to 6 separate textures
				jff_output.vertexPosModelSpace = vertexPosModelSpace;

				// Position inside the cube and looking to each view direction in a projection of 90 degrees
				gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec3 vertexPosModelSpace;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			uniform samplerCube envMap;
			uniform float envMapFaceWidth;
			uniform float roughness;

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
			vec3 importanceSampleNDF(vec2 randomPos, vec3 normalWorldSpace)
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
				vec3 B = abs(normalWorldSpace.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
				vec3 T = normalize(cross(B, normalWorldSpace));
				B = cross(normalWorldSpace, T);
				mat3 TBN = mat3(T, B, normalWorldSpace);

				// From tangent to world space
				vec3 halfwayDirWorldSpace = normalize(TBN * halfwayDirTangentSpace);
				
				return halfwayDirWorldSpace;
			}

			float normalDistribution(float NdotH)
			{
				float alpha = roughness * roughness; // Use squared roughtness for better visual results
				float alpha2 = alpha * alpha;
				float NdotH2 = pow(NdotH, 2); 
				
				return alpha2 / (PI * pow(NdotH2 * (alpha2 - 1.0) + 1.0, 2));
			}

			// ------------------------- PRE-FILTERED ENVIRONMENT MAP GENERATOR	 ------------------------- //

			vec3 prefilteredEnvironmentMapGenerator()
			{
				/*
				The approximation assumes that the view direction 'V' (and obviously the reflected direction 'R') matches the 
				normal 'N' on each fragment, which at the same time is used as output sample direction 'Wo'.
				*/
				vec3 normalWorldSpace = normalize(jff_input.vertexPosModelSpace);
				vec3 reflectedWorldSpace = normalWorldSpace;
				vec3 viewDirWorldSpace = reflectedWorldSpace;

				const uint SAMPLE_COUNT = 1024u;
				float totalWeight = 0.0;
				vec3 prefilteredColor = vec3(0.0);
				
				/* 
				Apply Monte Carlo integration:
					
					O = integral(f(x) * dx) = (1 / SAMPLE_COUNT) * sum(f(x) / pdf(x))

				Where:
				* f(x) is a single sample calculation
				* pdf(x) is the probability of a sample to occur over the total sample set
				*/
				for(uint i = 0u; i < SAMPLE_COUNT; ++i)
				{
					vec2 randomPos = hammersley(i, SAMPLE_COUNT);
					vec3 halfwayDirWorldSpace = importanceSampleNDF(randomPos, normalWorldSpace);
					// TODO: Why is Gram-Schmith used here? Can't understand next line
					vec3 lightDirWorldSpace = normalize(2.0 * dot(viewDirWorldSpace, halfwayDirWorldSpace) * halfwayDirWorldSpace - viewDirWorldSpace);

					float NdotL = max(dot(normalWorldSpace, lightDirWorldSpace), 0.0);
					if(NdotL > 0.0)
					{
						// Next portion of code removes bright dot artifacts when the convolution is made with high roughness
						float NdotH = max(dot(normalWorldSpace, halfwayDirWorldSpace), 0.0);
						float HdotV = max(dot(halfwayDirWorldSpace, viewDirWorldSpace), 0.0);

						float D = normalDistribution(NdotH);
						float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;

						float saTexel = 4.0 * PI / (6.0 * envMapFaceWidth * envMapFaceWidth);
						float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

						float lod = roughness == 0.0 ? 0.0 : (0.5 * log2(saSample / saTexel));

						// Perform mipmapped environment map lookup
						prefilteredColor += textureLod(envMap, lightDirWorldSpace, lod).rgb * NdotL;
						totalWeight += NdotL;
					}
				}
				prefilteredColor /= totalWeight;

				return prefilteredColor;
			}

			// ------------------------- MAIN FUNCTION ------------------------- //

			void main()
			{
				FragColor = vec4(prefilteredEnvironmentMapGenerator(), 1.0);
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
