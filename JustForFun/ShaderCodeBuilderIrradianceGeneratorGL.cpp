/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderIrradianceGeneratorGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderIrradianceGeneratorGL::ShaderCodeBuilderIrradianceGeneratorGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderIrradianceGeneratorGL")
}

JFF::ShaderCodeBuilderIrradianceGeneratorGL::~ShaderCodeBuilderIrradianceGeneratorGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderIrradianceGeneratorGL")
}

void JFF::ShaderCodeBuilderIrradianceGeneratorGL::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderIrradianceGeneratorGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderIrradianceGeneratorGL::getVertexShaderCode(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderIrradianceGeneratorGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec3 vertexPosModelSpace;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			uniform samplerCube envMap;

			#define PI 3.14159265359

			vec3 irradianceGenerator()
			{
				// Use fragment position in model space as hemisphere normal
				vec3 normal = normalize(jff_input.vertexPosModelSpace);

				// Create a TBN matrix to orient tangent space hemisphere to the normal direction
				vec3 B = vec3(0.0, 1.0, 0.0);
				vec3 T = normalize(cross(B, normal));
				B = normalize(cross(normal, T));
				mat3 TBN = mat3(T, B, normal);

				// Calculate the irradiance in an hemisphere around the current fragment pointing to current fragment normal
				vec3 irradiance = vec3(0.0);

				float sampleDelta = 0.025;
				float numSamples = 0.0;
				for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
				{
					for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
					{
						// Generate a vector pointing around all hemisphere, in tangent space
						vec3 sampleDirTangentSpace = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

						// From tangent space to world space
						vec3 sampleDirWorldSpace = TBN * sampleDirTangentSpace;

						// Gather other needed params
						float NdotWi = cos(theta); // N is normal and Wi is sampleDir, all in world space
						float hemispherePoleScale = sin(theta); // Used to scale the weight of samples coming from smaller areas close to the summit of the hemisphere

						// Calculate irradiance. Remember: Li(p,wi) * dot(n,wi)
						irradiance += texture(envMap, sampleDirWorldSpace).rgb * NdotWi * hemispherePoleScale;

						// Keep a count of samples executed
						numSamples++;
					}
				}

				// Calculate the integral of the irradiance using Riemann sum (this explains PI / numSamples factor)
				irradiance *= PI / numSamples;
				return irradiance;
			}

			void main()
			{
				FragColor = vec4(irradianceGenerator(), 1.0);
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
