/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderSSAOGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderSSAOGL::ShaderCodeBuilderSSAOGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderSSAOGL")
}

JFF::ShaderCodeBuilderSSAOGL::~ShaderCodeBuilderSSAOGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderSSAOGL")
}

void JFF::ShaderCodeBuilderSSAOGL::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode,
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderSSAOGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderSSAOGL::getVertexShaderCode(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderSSAOGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec2 uv;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			// Post processing textures
			uniform sampler2D ppFragWorldPos;
			uniform sampler2D ppNormalWorldDir;

			// Small texture storing random tangents in tangent space used to create noise in the shadows
			uniform sampler2D randomRotatedTangents; // Used to increase the noise when using the hemisphere of samples

			// Intensity parameter
			uniform float intensity;

			// Hemisphere samples parameters
			uniform float hemisphereRadius;
			uniform int numSamples;

			#define MAX_NUM_SAMPLES 64
			uniform vec3 hemisphereSamplesTangentSpace[MAX_NUM_SAMPLES];

			// Use uniform block for uniforms that doesn't change between programs
			// This uniform block will use binding point 0
			layout (std140) uniform CameraParams
			{
				mat4 viewMatrix;
				mat4 projectionMatrix;
				vec3 cameraPosWorldSpace;
			};

			// UV used for texture sampling calculations
			vec2 uv;

			// -------------------------- SSAO FUNCTION -------------------------- //

			float ssao()
			{
				// We need to scale the small noise texture to make it change many times on each uv change
				vec2 noiseScale = textureSize(ppFragWorldPos, 0) / textureSize(randomRotatedTangents, 0);

				// Extract data from input textures
				vec3 fragPosWorldSpace = texture(ppFragWorldPos, uv).xyz;
				vec3 normalWorldSpace = texture(ppNormalWorldDir, uv).xyz;
				vec3 randomTangentTangentSpace = texture(randomRotatedTangents, uv * noiseScale).xyz;

				// Orthogonalize tangent (this converts it to tangentWorldSpace) using Gramm-Schmidt process and build TBN matrix
				vec3 randomTangentWorldSpace = normalize(randomTangentTangentSpace - normalWorldSpace * dot(randomTangentTangentSpace, normalWorldSpace)); 
				vec3 bitangentWorldSpace = cross(normalWorldSpace, randomTangentWorldSpace);
				mat3 randomTBN = mat3(randomTangentWorldSpace, bitangentWorldSpace, normalWorldSpace);

				// Start occlusion algorithm: 0 -> Not occluded | 1 -> Occluded
				float occlusion = 0.0;
				for(int i = 0; i < numSamples; ++i)
				{
					vec3 sampleWorldSpace = randomTBN * hemisphereSamplesTangentSpace[i]; // This is an offset
					sampleWorldSpace = fragPosWorldSpace + sampleWorldSpace * hemisphereRadius;
					
					// Use the sampleWorldSpace to check ppFragWorldPos and sample the fragment in that position
					vec4 sampleUV = vec4(sampleWorldSpace, 1.0);
					sampleUV = projectionMatrix * viewMatrix * sampleUV; // from sample in world space to clip space [-w,w]
					sampleUV.xyz /= sampleUV.w; // Perspective divide (from [-w,w] to [-1, 1])
					sampleUV.xyz = sampleUV.xyz * 0.5 + 0.5; // From [-1,1] to [0,1]

					// Get the depth of an existing fragment in sample position (we get the depth in view space)
					vec4 fragPosOffsetViewSpace = viewMatrix * texture(ppFragWorldPos, sampleUV.xy);
					float fragOffsetDepth = fragPosOffsetViewSpace.z;

					// Get the depth of the current sample in view space
					vec4 sampleViewSpace = viewMatrix * vec4(sampleWorldSpace, 1.0);
					float sampleDepth = sampleViewSpace.z;

					// Do a range check to avoid creating shadow between a figure and the background (shadow outlining)
					vec4 fragPosViewSpace = viewMatrix * vec4(fragPosWorldSpace, 1.0);
					float fragDepth = fragPosViewSpace.z;
					float rangeCheck = smoothstep(0.0, 1.0, hemisphereRadius / abs(fragDepth - fragOffsetDepth)); 
					
					float bias = 0.025; // Make a bias to mitigate shadow acne
					occlusion += (fragOffsetDepth >= sampleDepth + bias ? 1.0 : 0.0) * rangeCheck;
				}

				// Invert occlusion: 0 -> Occluded | 1 -> Not occluded
				occlusion = 1.0 - (occlusion / numSamples);

				// Ignore background in SSAO
				occlusion = length(normalWorldSpace) <= 0.0 ? 1.0 : occlusion;

				return pow(occlusion, intensity);
			}

			// -------------------------- MAIN FUNCTION -------------------------- //

			void main()
			{	
				// Setup some variables
				uv = jff_input.uv;

				float occlusion = ssao();
				FragColor = vec4(occlusion, occlusion, occlusion, 1.0); // TODO: FragColor could be float instead of vec4
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
