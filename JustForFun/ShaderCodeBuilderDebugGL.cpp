/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderDebugGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderDebugGL::ShaderCodeBuilderDebugGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderDebugGL")
}

JFF::ShaderCodeBuilderDebugGL::~ShaderCodeBuilderDebugGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderDebugGL")
}

void JFF::ShaderCodeBuilderDebugGL::generateCode(const Params& params, 
	std::string& outVertexShaderCode,
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outGeometryShaderCode = getGeometryShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderDebugGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderDebugGL::getVertexShaderCode(const Params& params) const
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
				vec3 tangentWorldSpace;
				vec3 bitangentWorldSpace;
				vec2 uv;
			} jff_output;

			uniform mat4 modelMatrix;
			uniform mat3 normalMatrix;

			void main()
			{
				jff_output.fragPosModelSpace = vertexPosModelSpace;
				jff_output.fragPosWorldSpace = modelMatrix * vec4(vertexPosModelSpace, 1.0);

				jff_output.normalWorldSpace = normalize(normalMatrix * normalize(normalModelSpace));
				jff_output.tangentWorldSpace = normalize(normalMatrix * normalize(tangentModelSpace));
				jff_output.bitangentWorldSpace =  normalize(normalMatrix * normalize(bitangentModelSpace));

				jff_output.uv = uvModelSpace.xy;

				gl_Position = modelMatrix * vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderDebugGL::getGeometryShaderCode(const Params& params) const
{
	// This code does nothing with primitives; it only forwards them
	static std::string forwardCode =
		R"glsl(
			layout (triangles) in;
			layout (triangle_strip, max_vertices = 3) out;

			in VertexShaderOutput
			{
				vec3 fragPosModelSpace;
				vec4 fragPosWorldSpace;
				vec3 normalWorldSpace;
				vec3 tangentWorldSpace;
				vec3 bitangentWorldSpace;
				vec2 uv;
			} jff_input[];

			out vec4 lineColor;

			const vec4 PINK = vec4(1.0, 0.2, 0.5, 1.0);

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
				gl_Position = projectionMatrix * viewMatrix * gl_in[0].gl_Position;
				lineColor = PINK;
				EmitVertex();

				gl_Position = projectionMatrix * viewMatrix * gl_in[1].gl_Position;
				lineColor = PINK;
				EmitVertex();

				gl_Position = projectionMatrix * viewMatrix * gl_in[2].gl_Position;
				lineColor = PINK;
				EmitVertex();

				EndPrimitive();
			}
		)glsl";

	// This code transform triangle primitives into their normals (one normal per vertex)
	static std::string showNormalsCode =
		R"glsl(
			layout (triangles) in;
			layout (line_strip, max_vertices = 18) out;

			in VertexShaderOutput
			{
				vec3 fragPosModelSpace;
				vec4 fragPosWorldSpace;
				vec3 normalWorldSpace;
				vec3 tangentWorldSpace;
				vec3 bitangentWorldSpace;
				vec2 uv;
			} jff_input[];

			out vec4 lineColor;

			// Use uniform block for uniforms that doesn't change between programs
			// This uniform block will use binding point 0
			layout (std140) uniform CameraParams
			{
				mat4 viewMatrix;
				mat4 projectionMatrix;
				vec3 cameraPosWorldSpace;
			};
			
			const float LINE_LENGHT = 0.02;
			const vec4 RED = vec4(1.0, 0.0, 0.0, 1.0);
			const vec4 GREEN = vec4(0.0, 1.0, 0.0, 1.0);
			const vec4 BLUE = vec4(0.0, 0.0, 1.0, 1.0);

			void main()
			{
				// -------------------------- Draw a normal line per polygon vertex -------------------------- //
				
				gl_Position = projectionMatrix * viewMatrix * gl_in[0].gl_Position;
				lineColor = BLUE;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[0].gl_Position + vec4(jff_input[0].normalWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = BLUE;				
				EmitVertex();

				EndPrimitive();

				gl_Position = projectionMatrix * viewMatrix * gl_in[1].gl_Position;
				lineColor = BLUE;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[1].gl_Position + vec4(jff_input[1].normalWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = BLUE;
				EmitVertex();

				EndPrimitive();

				gl_Position = projectionMatrix * viewMatrix * gl_in[2].gl_Position;
				lineColor = BLUE;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[2].gl_Position + vec4(jff_input[2].normalWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = BLUE;
				EmitVertex();

				EndPrimitive();

				// -------------------------- Draw a tangent line per polygon vertex -------------------------- //
				
				gl_Position = projectionMatrix * viewMatrix * gl_in[0].gl_Position;
				lineColor = RED;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[0].gl_Position + vec4(jff_input[0].tangentWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = RED;
				EmitVertex();

				EndPrimitive();

				gl_Position = projectionMatrix * viewMatrix * gl_in[1].gl_Position;
				lineColor = RED;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[1].gl_Position + vec4(jff_input[1].tangentWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = RED;
				EmitVertex();

				EndPrimitive();

				gl_Position = projectionMatrix * viewMatrix * gl_in[2].gl_Position;
				lineColor = RED;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[2].gl_Position + vec4(jff_input[2].tangentWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = RED;
				EmitVertex();

				EndPrimitive();

				// -------------------------- Draw a bitangent line per polygon vertex -------------------------- //
				
				gl_Position = projectionMatrix * viewMatrix * gl_in[0].gl_Position;
				lineColor = GREEN;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[0].gl_Position + vec4(jff_input[0].bitangentWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = GREEN;
				EmitVertex();

				EndPrimitive();

				gl_Position = projectionMatrix * viewMatrix * gl_in[1].gl_Position;
				lineColor = GREEN;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[1].gl_Position + vec4(jff_input[1].bitangentWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = GREEN;
				EmitVertex();

				EndPrimitive();

				gl_Position = projectionMatrix * viewMatrix * gl_in[2].gl_Position;
				lineColor = GREEN;
				EmitVertex();
				gl_Position = projectionMatrix * viewMatrix * (gl_in[2].gl_Position + vec4(jff_input[2].bitangentWorldSpace, 0.0) * LINE_LENGHT);
				lineColor = GREEN;
				EmitVertex();

				EndPrimitive();
			}
		)glsl";
	
	std::ostringstream oss;
	oss << getShaderVersionLine(params); 
	
	switch (params.debugDisplay)
	{
	case DebugDisplay::NO_DISPLAY:
	case DebugDisplay::POLYGONS:
		oss << forwardCode;
		break;
	case DebugDisplay::NORMALS:
		oss << showNormalsCode;
		break;
	default:
		break;
	}

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderDebugGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in vec4 lineColor;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			void main()
			{
				FragColor = lineColor;
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
