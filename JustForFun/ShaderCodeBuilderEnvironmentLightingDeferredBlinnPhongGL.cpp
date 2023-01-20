/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL()
{
    JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL")
}

JFF::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL::~ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL()
{
    JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL")
}

void JFF::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL::generateCode(
    const Params& params,
    std::string& outVertexShaderCode, 
    std::string& outGeometryShaderCode, 
    std::string& outFragmentShaderCode) const
{
    outVertexShaderCode = getVertexShaderCode(params);
    outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL::getVertexShaderCode(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec2 uv;
			} jff_input;

			layout (location = 0) out vec4 FragColor; // Color attachment 0
			
			// G-buffer textures
			uniform sampler2D ppFragWorldPos;
			uniform sampler2D ppNormalWorldDir;
			uniform sampler2D ppAlbedoSpecular;
			uniform sampler2D ppAmbientShininess;
			uniform sampler2D ppReflection;

			// G-buffer parameters
			vec4 fragPosWorldSpace;
			vec3 normalWorldSpace;

			vec4 diffuse;
			vec4 specular;
			vec4 ambient;
			vec4 shininess;
			vec4 reflection;
			// TODO: More material parameters here

			// UV used for texture sampling calculations
			vec2 uv;

			// Use uniform block for uniforms that doesn't change between programs
			// This uniform block will use binding point 0
			layout (std140) uniform CameraParams
			{
				mat4 viewMatrix;
				mat4 projectionMatrix;
				vec3 cameraPosWorldSpace;
			};

			// Environment maps
			uniform samplerCube envMap;

			// ---------------------------------- G-BUFFER EXTRACTION FUNCTION ---------------------------------- //

			void extractFromGBuffer()
			{
				fragPosWorldSpace = texture(ppFragWorldPos, uv);
				normalWorldSpace = texture(ppNormalWorldDir, uv).rgb;
				
				vec4 albedoSpecular = texture(ppAlbedoSpecular, uv);
				diffuse = vec4(albedoSpecular.rgb, 1.0);
				specular = albedoSpecular.aaaa; // Channel alpha 4 times
				
				vec4 ambientShininess = texture(ppAmbientShininess, uv);
				ambient = vec4(ambientShininess.rgb, 0.0);
				shininess = ambientShininess.aaaa;

				reflection = texture(ppReflection, uv);

				// TODO: More material parameters here
			}

			// ---------------------------------- ENVIRONMENT CONTRIBUTION FUNCTION ---------------------------------- //

			vec3 environmentFunction()
			{
				vec3 camToFragDirWorldSpace = normalize(vec3(fragPosWorldSpace) - cameraPosWorldSpace);
				vec3 reflectionDirWorldSpace = reflect(camToFragDirWorldSpace, normalWorldSpace);
				return texture(envMap, reflectionDirWorldSpace).rgb * reflection.rgb;
			}

			// ---------------------------------- MAIN FUNCTION ---------------------------------- //

			void main()
			{
				// Setup some variables
				uv = jff_input.uv;
				extractFromGBuffer();

				FragColor = vec4(environmentFunction(), 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
