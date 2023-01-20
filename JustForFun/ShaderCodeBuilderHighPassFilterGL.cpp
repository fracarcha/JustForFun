/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderHighPassFilterGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderHighPassFilterGL::ShaderCodeBuilderHighPassFilterGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderHighPassFilterGL")
}

JFF::ShaderCodeBuilderHighPassFilterGL::~ShaderCodeBuilderHighPassFilterGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderHighPassFilterGL")
}

void JFF::ShaderCodeBuilderHighPassFilterGL::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode,
	std::string& outGeometryShaderCode,
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderHighPassFilterGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderHighPassFilterGL::getVertexShaderCode(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderHighPassFilterGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec2 uv;
			} jff_input;

			layout (location = 0) out vec4 FragColor; // Color attachment 0

			uniform sampler2D ppOutputColor;
			uniform float mipLevel;
			uniform float threshold;

			void main()
			{
				FragColor = textureLod(ppOutputColor, jff_input.uv, mipLevel);

				// Calculate brightness
				float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722)); // Dot product = r * 0.2126 + g * 0.7152 + b * 0.0722;

				// Color below threshold (in HDR buffers) is considered dark and will be filtered
				if(brightness < threshold) 
				{
					FragColor = vec4(0.0, 0.0, 0.0, 1.0);
				}
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
