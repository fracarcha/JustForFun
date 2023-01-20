/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderOmnidirectionalShadowCastGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL::ShaderCodeBuilderOmnidirectionalShadowCastGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderOmnidirectionalShadowCastGL")
}

JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL::~ShaderCodeBuilderOmnidirectionalShadowCastGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderOmnidirectionalShadowCastGL")
}

void JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL::generateCode(const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outGeometryShaderCode = getGeometryShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL::getVertexShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			layout (location = 0) in vec3 vertexPosModelSpace;
			layout (location = 1) in vec3 normalModelSpace;
			layout (location = 2) in vec3 tangentModelSpace;
			layout (location = 3) in vec3 bitangentModelSpace;
			layout (location = 4) in vec3 uvModelSpace;

			uniform mat4 modelMatrix;

			void main()
			{
				// Set position to world space only because geometry shader will transform to clip space per cube face
				gl_Position = modelMatrix * vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL::getGeometryShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			layout (triangles) in;
			layout (triangle_strip, max_vertices = 18) out;

			uniform mat4 cubemapViewMatrices[6]; // One view matrix per cubemap face
			uniform mat4 projectionMatrix;

			out vec4 fragPosWorldSpace;

			void main()
			{
				for(int face = 0; face < 6; ++face)
				{
					for(int i = 0; i < 3; ++i)
					{
						// Selects the rendering target face of the cubemap. 
						// This works for framebuffers which attachment points are considered "layered"
						// The order of layer:cubemap-face is: 0:right 1:left 2:top 3:bottom 4:near 5:far
						gl_Layer = face;

						fragPosWorldSpace = gl_in[i].gl_Position;
						gl_Position = projectionMatrix * cubemapViewMatrices[face] * fragPosWorldSpace;
						EmitVertex();
					}
				
					EndPrimitive();
				}
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in vec4 fragPosWorldSpace;

			uniform vec3 lightPos;
			uniform float farPlane;

			void main()
			{
				// Calculate distance between lightPos and fragment position (both in world space)
				float distance = length(fragPosWorldSpace.xyz - lightPos);
				
				// Divide by far plane to normalize [0,1] the distance
				float depthLinear = distance / farPlane;

				// Store LINEAR depth to simplify calculations on next render passes
				gl_FragDepth = depthLinear;
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
