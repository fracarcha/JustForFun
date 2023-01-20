/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderBackgroundGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderBackgroundGL::ShaderCodeBuilderBackgroundGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderBackgroundGL")
}

JFF::ShaderCodeBuilderBackgroundGL::~ShaderCodeBuilderBackgroundGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderBackgroundGL")
}

void JFF::ShaderCodeBuilderBackgroundGL::generateCode(const Params& params,
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	// TODO: geometry
	outFragmentShaderCode = getFragmentShaderCode(params);
}

// ---------------------------- HELPER FUNCTIONS ---------------------------- //

inline std::string JFF::ShaderCodeBuilderBackgroundGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderBackgroundGL::getVertexShaderCode(const Params& params) const
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
				vec3 fragPosModelSpace;
				vec4 fragPosWorldSpace;
				vec3 normalWorldSpace;
				vec2 uv;
			} jff_output;

			uniform mat4 modelMatrix;
			uniform mat3 normalMatrix;

			// Use uniform block for uniforms that doesn't change between programs
			// This uniform block will use binding point 0
			layout (std140) uniform CameraParams
			{
				mat4 viewMatrix;
				mat4 projectionMatrix;
				vec3 cameraPosWorldSpace;
			};

			void main()
			{
				jff_output.normalWorldSpace = normalize(normalMatrix * normalize(normalModelSpace));
				jff_output.uv = uvModelSpace.xy;
				jff_output.fragPosModelSpace = vertexPosModelSpace;
				jff_output.fragPosWorldSpace = modelMatrix * vec4(vertexPosModelSpace, 1.0);
				
				vec4 pos = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosModelSpace, 1.0);

				// Used W as Z component to 'send' vertex position to the far plane. This is done on perspective division phase,
				// just after vertex shader, where (x/w, y/w, z/w) == (x/w, y/w, w/w) == (x/w, y/w, 1.0)
				gl_Position = pos.xyww; // Send vertex to background
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderBackgroundGL::getFragmentShaderCode(const Params& params) const
{
	static std::string attributesCode =
		R"glsl(
			in VertexShaderOutput
			{
				vec3 fragPosModelSpace;
				vec4 fragPosWorldSpace;
				vec3 normalWorldSpace;
				vec2 uv;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			// Use uniform block for uniforms that doesn't change between programs
			// This uniform block will use binding point 0
			layout (std140) uniform CameraParams
			{
				mat4 viewMatrix;
				mat4 projectionMatrix;
				vec3 cameraPosWorldSpace;
			};

			// Material output attributes
			vec4 bgColor;

			// UV used for texture sampling calculations
			vec2 uv;
		)glsl";

	static std::string textureSampler = 
		R"glsl(
			uniform sampler2D @1;
		)glsl";

	static std::string cubemapSampler =
		R"glsl(
			uniform samplerCube @1;
		)glsl";

	static std::string mainFunctionCode =
		R"glsl(			
			void main()
			{
				// Setup some variables
				uv = jff_input.uv;

				material();
				materialOverrides();
				FragColor = bgColor;
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << attributesCode;
	
	// Add all texture uniforms
	for (const std::string& texName : params.textures)
	{
		std::regex samplerName("@1");
		oss << std::regex_replace(textureSampler, samplerName, texName);
	}

	// Add all cubemap uniforms
	for (const std::string& cubeName : params.cubemaps)
	{
		std::regex samplerName("@1");
		oss << std::regex_replace(cubemapSampler, samplerName, cubeName);
	}

	// Add custom code and main function
	oss << params.customCode << mainFunctionCode;

	return oss.str();
}
