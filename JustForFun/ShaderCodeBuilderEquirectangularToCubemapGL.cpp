/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderEquirectangularToCubemapGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderEquirectangularToCubemapGL::ShaderCodeBuilderEquirectangularToCubemapGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderEquirectangularToCubemapGL")
}

JFF::ShaderCodeBuilderEquirectangularToCubemapGL::~ShaderCodeBuilderEquirectangularToCubemapGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderEquirectangularToCubemapGL")
}

void JFF::ShaderCodeBuilderEquirectangularToCubemapGL::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode,
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderEquirectangularToCubemapGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderEquirectangularToCubemapGL::getVertexShaderCode(const Params& params) const
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
				// and map equirectangular texture to 6 separate textures
				jff_output.vertexPosModelSpace = vertexPosModelSpace;

				// Position inside the cube and looking to each view direction in a projection of 90 degrees
				gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderEquirectangularToCubemapGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec3 vertexPosModelSpace;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			uniform sampler2D equirectangularTex;

			vec2 cubemapToUVCoords(vec3 cubemapUVW)
			{
				// Important: Normalize the vector to get a correct UV in next line
				cubemapUVW = normalize(cubemapUVW);

				// Atan2 returns the angle (in radians) between (x,y) vector and x axis. Result range: [-pi, pi]
				// NOTE: Atan2 receives the parameters in a special format: atan(y,x). Note the order of x and y
				// Asin returns the angle (in radians) of a vector and x axis given its sine. Result range: [-pi/2, pi/2]
				vec2 uv = vec2(atan(cubemapUVW.z, cubemapUVW.x), asin(cubemapUVW.y));

				const vec2 invAtan = vec2(0.1591, 0.3183);	// x: 1/(2*pi) | y: 1/pi
				uv *= invAtan;								// Range: [-0.5, 0.5]
				uv += 0.5;									// Range: [0,1] This range is needed to sample a texture ;)

				return uv;
			}		

			void main()
			{
				vec2 uv = cubemapToUVCoords(jff_input.vertexPosModelSpace);
				FragColor = vec4(texture(equirectangularTex, uv).rgb, 1.0);
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
