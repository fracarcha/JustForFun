/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderGeometryDeferredBlinnPhongGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong::ShaderCodeBuilderGeometryDeferredBlinnPhong()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderGeometryDeferredBlinnPhong")
}

JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong::~ShaderCodeBuilderGeometryDeferredBlinnPhong()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderGeometryDeferredBlinnPhong")
}

void JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong::generateCode(
	const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode,
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong::getVertexShaderCode(const Params& params) const
{
	// TODO: re-orthogonalize tangentWorldSpace and bitangentWorldSpace using Gram-Schmidt process
	static std::string code =
		R"glsl(
			layout (location = 0) in vec3 vertexPosModelSpace;
			layout (location = 1) in vec3 normalModelSpace;
			layout (location = 2) in vec3 tangentModelSpace;
			layout (location = 3) in vec3 bitangentModelSpace;
			layout (location = 4) in vec3 uvModelSpace;

			out VertexShaderOutput
			{
				vec4 fragPosWorldSpace;

				vec3 normalWorldSpace;
				mat3 TBN;

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
				jff_output.fragPosWorldSpace = modelMatrix * vec4(vertexPosModelSpace, 1.0);

				jff_output.normalWorldSpace = normalize(normalMatrix * normalize(normalModelSpace));
				vec3 tangentWorldSpace = normalize(normalMatrix * normalize(tangentModelSpace));
				vec3 bitangentWorldSpace = normalize(normalMatrix * normalize(bitangentModelSpace));
				jff_output.TBN = mat3(tangentWorldSpace, bitangentWorldSpace, jff_output.normalWorldSpace);

				jff_output.uv = uvModelSpace.xy;

				gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong::getFragmentShaderCode(const Params& params) const
{
	static std::string attributesCode =
		R"glsl(
			in VertexShaderOutput
			{
				vec4 fragPosWorldSpace;

				vec3 normalWorldSpace;
				mat3 TBN;

				vec2 uv;
			} jff_input;

			layout (location = 0) out vec4 ppFragWorldPos;		// Color attachment 0
			layout (location = 1) out vec4 ppNormalWorldDir;	// Color attachment 1
			layout (location = 2) out vec4 ppAlbedoSpecular;	// Color attachment 2
			layout (location = 3) out vec4 ppAmbientShininess;	// Color attachment 3
			layout (location = 4) out vec4 ppReflection;		// Color attachment 4
			layout (location = 5) out vec4 ppEmissive;			// Color attachment 5

			// Use uniform block for uniforms that doesn't change between programs
			// This uniform block will use binding point 0
			layout (std140) uniform CameraParams
			{
				mat4 viewMatrix;
				mat4 projectionMatrix;
				vec3 cameraPosWorldSpace;
			};

			// Material output attributes

			vec4 height;
			vec4 displacement;
			float parallaxIntensity;

			vec4 diffuse;
			vec4 specular;
			vec4 ambient;
			vec4 emissive;
			vec4 normal;
			vec4 shininess;
			vec4 opacity;
			vec4 lightmap;		// TODO
			vec4 reflection;

			// Normal used to do all calculations. It can come from normal mapping or from the primitive
			vec3 normalWorldSpace;

			// UV used for texture sampling calculations
			vec2 uv;

			// declare next functions because it's used in material() function
			void parallaxMapping();
			void parallaxMappingDisplacement(in sampler2D displacementMap);
			void parallaxMappingHeight(in sampler2D heightMap);
		)glsl";

	static std::string textureSampler =
		R"glsl(
			uniform sampler2D @1;
		)glsl";

	static std::string cubemapSampler =
		R"glsl(
			uniform samplerCube @1;
		)glsl";

	static std::string normalMapping =
		R"glsl(
			void normalMapping()
			{
				vec3 normalTangentSpace = normal.xyz * 2.0 - 1.0; // Convert normals from color space [0,1] to vector space [-1,1]
				normalWorldSpace = normalize(jff_input.TBN * normalTangentSpace);
			}
		)glsl";

	static std::string primitiveNormal =
		R"glsl(
			void normalMapping()
			{
				normalWorldSpace = normalize(jff_input.normalWorldSpace);
			}
		)glsl";

	static std::string mainFunctionCode =
		R"glsl(
			// ---------------------------------- PARALLAX MAPPING FUNCTION ---------------------------------- //

			void parallaxMapping()
			{
				// Common parameters setup
				mat3 TBN_inv = transpose(jff_input.TBN); // Inverse of orthogonal matrix is equal to its transpose, which is way cheaper in computation
				vec3 fragToCamDirWorldSpace = cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz;
				vec3 fragToCamDirTangentSpace = normalize(TBN_inv * fragToCamDirWorldSpace);
	
				// Height workflow
				vec2 uvHeightOffset = (1.0 - height.r) * parallaxIntensity * (fragToCamDirTangentSpace.xy / fragToCamDirTangentSpace.z);
				uv -= uvHeightOffset;
	
				// displacement workflow
				vec2 uvDisplacementOffset = displacement.r * parallaxIntensity * (fragToCamDirTangentSpace.xy / fragToCamDirTangentSpace.z);
				uv -= uvDisplacementOffset;

				// Discard uv beyond limits to make a cool bump effect on edges
				if(uv.x > 1.0 || uv.x < 0.0 || uv.y > 1.0 || uv.y < 0.0)
					discard;	
			}

			void parallaxMappingDisplacement(in sampler2D displacementMap)
			{
				// Common parameters setup
				mat3 TBN_inv = transpose(jff_input.TBN); // Inverse of orthogonal matrix is equal to its transpose, which is way cheaper in computation
				vec3 fragToCamDirWorldSpace = cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz;
				vec3 fragToCamDirTangentSpace = normalize(TBN_inv * fragToCamDirWorldSpace);

				// Split the height into the number of steep steps
				// Decide the number of samples depending on the eye-surface angle (the less angle, the more layers needed)
				float numLayers = mix(32.0, 8.0, max(dot(vec3(0.0, 0.0, 1.0), fragToCamDirTangentSpace), 0.0));
	
				// Get the height of each chunk
				float heightChunk = 1.0 / numLayers;
				float currentLayerHeight = 0.0;

				// Amount to shift texture coords per layer (in fragToCamDirTangentSpace direction)
				vec2 deltaUV = (fragToCamDirTangentSpace.xy * parallaxIntensity) / numLayers; // TODO: precision error here. xy components should increment tha same amount as z to preserve fragToCamDirTangentSpace direction
				vec2 currentUV = uv;

				// Get height map initial values
				float currentMapHeight = texture(displacementMap, currentUV).r;

				while(currentLayerHeight < currentMapHeight) // Iterate until -fragToCamDir hits any "mountain"
				{
					// Move current UV coords to compare the next two values
					currentUV -= deltaUV;

					// Get height on next UV coords
					currentMapHeight = texture(displacementMap, currentUV).r;
		
					// Get height on next layer
					currentLayerHeight += heightChunk;
				}

				// The result of this function is the new UV
				uv = currentUV;

				// Get the uv, map height and layer height previous of the hit
				vec2 prevUV = currentUV + deltaUV;
				float prevMapHeight = texture(displacementMap, prevUV).r;
				float prevLayerHeight = currentLayerHeight - heightChunk;

				// Get height after and before hit for linear interpolation
				float afterHeight = currentMapHeight - currentLayerHeight;
				float beforeHeight = prevMapHeight - prevLayerHeight;

				// Interpolate texture coordiantes for a smooth steep transition
				float weight = afterHeight / (afterHeight - beforeHeight);
				uv = prevUV * weight + currentUV * (1.0 - weight);

				// Discard uv beyond limits to make a cool bump effect on edges
				if(uv.x > 1.0 || uv.x < 0.0 || uv.y > 1.0 || uv.y < 0.0)
					discard;
			}

			void parallaxMappingHeight(in sampler2D heightMap)
			{
				// Common parameters setup
				mat3 TBN_inv = transpose(jff_input.TBN); // Inverse of orthogonal matrix is equal to its transpose, which is way cheaper in computation
				vec3 fragToCamDirWorldSpace = cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz;
				vec3 fragToCamDirTangentSpace = normalize(TBN_inv * fragToCamDirWorldSpace);

				// Split the height into the number of steep steps
				// Decide the number of samples depending on the eye-surface angle (the less angle, the more layers needed)
				float numLayers = mix(32.0, 8.0, max(dot(vec3(0.0, 0.0, 1.0), fragToCamDirTangentSpace), 0.0));
	
				// Get the height of each chunk
				float heightChunk = 1.0 / numLayers;
				float currentLayerHeight = 0.0;

				// Amount to shift texture coords per layer (in fragToCamDirTangentSpace direction)
				vec2 deltaUV = (fragToCamDirTangentSpace.xy * parallaxIntensity) / numLayers; // TODO: precision error here. xy components should increment tha same amount as z to preserve fragToCamDirTangentSpace direction
				vec2 currentUV = uv;

				// Get height map initial values
				float currentMapHeight = texture(heightMap, currentUV).r;
				currentMapHeight = 1.0 - currentMapHeight; // Height is the inverse of displacement

				while(currentLayerHeight < currentMapHeight) // Iterate until -fragToCamDir hits any "mountain"
				{
					// Move current UV coords to compare the next two values
					currentUV -= deltaUV;

					// Get height on next UV coords
					currentMapHeight = texture(heightMap, currentUV).r;
					currentMapHeight = 1.0 - currentMapHeight; // Height is the inverse of displacement
		
					// Get height on next layer
					currentLayerHeight += heightChunk;
				}

				// The result of this function is the new UV
				uv = currentUV;

				// Get the uv, map height and layer height previous of the hit
				vec2 prevUV = currentUV + deltaUV;
				float prevMapHeight = texture(heightMap, prevUV).r;
				prevMapHeight = 1.0 - prevMapHeight; // Height is the inverse of displacement
				float prevLayerHeight = currentLayerHeight - heightChunk;

				// Get height after and before hit for linear interpolation
				float afterHeight = currentMapHeight - currentLayerHeight;
				float beforeHeight = prevMapHeight - prevLayerHeight;

				// Interpolate texture coordiantes for a smooth steep transition
				float weight = afterHeight / (afterHeight - beforeHeight);
				uv = prevUV * weight + currentUV * (1.0 - weight);

				// Discard uv beyond limits to make a cool bump effect on edges
				if(uv.x > 1.0 || uv.x < 0.0 || uv.y > 1.0 || uv.y < 0.0)
					discard;
			}

			void main()
			{
				// Setup some variables
				uv = jff_input.uv;

				height				= vec4(0.0);
				displacement		= vec4(0.0);
				parallaxIntensity	= 0.0;
	
				diffuse		= vec4(0.0);
				specular	= vec4(0.0);
				ambient		= vec4(0.0);
				emissive	= vec4(0.0);
				normal		= vec4(0.0);
				shininess	= vec4(1.0);
				opacity		= vec4(1.0);
				lightmap	= vec4(0.0);
				reflection	= vec4(0.0);

				material();
				materialOverrides();

				if(opacity.r <= 0.0) // Discard fragments that are fully transparent
					discard;

				normalMapping();

				// Outputs
				ppFragWorldPos		= jff_input.fragPosWorldSpace;
				ppNormalWorldDir	= vec4(normalWorldSpace, 0.0);
				ppAlbedoSpecular	= vec4(diffuse.rgb, specular.r);
				ppAmbientShininess	= vec4(ambient.rgb, shininess.r);
				ppReflection		= vec4(reflection.rgb, 0.0);
				ppEmissive			= vec4(emissive.rgb, 0.0);

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

	// Add normal mapping function code
	oss << (params.useNormalMap ? normalMapping : primitiveNormal);

	// Add custom code and main function
	oss << params.customCode << mainFunctionCode;

	return oss.str();
}
