/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderGaussianBlurVerticalGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderGaussianBlurVerticalGL::ShaderCodeBuilderGaussianBlurVerticalGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderGaussianBlurVerticalGL")
}

JFF::ShaderCodeBuilderGaussianBlurVerticalGL::~ShaderCodeBuilderGaussianBlurVerticalGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderGaussianBlurVerticalGL")
}

void JFF::ShaderCodeBuilderGaussianBlurVerticalGL::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderGaussianBlurVerticalGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderGaussianBlurVerticalGL::getVertexShaderCode(const Params& params) const
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
				vec2 uv;
			} jff_output;

			void main()
			{
				jff_output.uv = uvModelSpace.xy;
				gl_Position = vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderGaussianBlurVerticalGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec2 uv;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			uniform sampler2D ppOutputColor;
			uniform float mipLevel;

			const int NUM_WEIGHTS = 5;
			float weights[NUM_WEIGHTS] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); // Gaussian bell weights

			void main()
			{
				// Calculate horizontal gaussian blur and store the jff_output in brightness channel
				vec2 texelSize = 1.0 / textureSize(ppOutputColor, int(mipLevel)); // Texel size (in normalized space) in LOD mipLevel
				vec3 result = textureLod(ppOutputColor, jff_input.uv, mipLevel).rgb * weights[0]; // Sample the current fragment
				
				for (int i = 1; i < NUM_WEIGHTS; ++i) // Sample horizontal neighbours and combine them using gaussian weights
				{
					result += textureLod(ppOutputColor, jff_input.uv + vec2(0.0, texelSize.y * i), mipLevel).rgb * weights[i];
					result += textureLod(ppOutputColor, jff_input.uv - vec2(0.0, texelSize.y * i), mipLevel).rgb * weights[i];
				}

				FragColor = vec4(result, 1.0);
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
