/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderPostProcessGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderPostProcessGL::ShaderCodeBuilderPostProcessGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderPostProcessGL")
}

JFF::ShaderCodeBuilderPostProcessGL::~ShaderCodeBuilderPostProcessGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderPostProcessGL")
}

void JFF::ShaderCodeBuilderPostProcessGL::generateCode(const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderPostProcessGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderPostProcessGL::getVertexShaderCode(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderPostProcessGL::getFragmentShaderCode(const Params& params) const
{
	static std::string codePostProcessFunctions =
		R"glsl(
			in VertexShaderOutput
			{
				vec2 uv;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			uniform sampler2D ppOutputColor;

			// -------------------------------- HDR -------------------------------- //
			// Tone mapping functions take an unbound color (HDR) and transforms it to a bound version [0,1] (LDR).
			// This is done because a GL_RGB16F color buffer is used, but non-HDR monitors still expect LDR values

			// Tone mapping Reinhard formula
			void toneMappingReinhard()
			{
				FragColor = vec4(FragColor.rgb / (FragColor.rgb + vec3(1.0)), 1.0);
			}

			// Tone mapping with exposure control
			void toneMappingExposure(float exposure)
			{
				FragColor = vec4(vec3(1.0) - exp(-FragColor.rgb * exposure), 1.0);
			}
		)glsl";

	static std::string codeMainFunction =
		R"glsl(
			void main()
			{
				FragColor = texture(ppOutputColor, jff_input.uv);

				material();
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << codePostProcessFunctions << params.customCode << codeMainFunction;

	return oss.str();
}
