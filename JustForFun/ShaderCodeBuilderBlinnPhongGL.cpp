/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderBlinnPhongGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderBlinnPhongGL::ShaderCodeBuilderBlinnPhongGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderBlinnPhongGL")
}

JFF::ShaderCodeBuilderBlinnPhongGL::~ShaderCodeBuilderBlinnPhongGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderBlinnPhongGL")
}

void JFF::ShaderCodeBuilderBlinnPhongGL::generateCode(const Params& params, 
	std::string& outVertexShaderCode,
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderBlinnPhongGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderBlinnPhongGL::getVertexShaderCode(const Params& params) const
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
				vec3 fragPosModelSpace;
				vec4 fragPosWorldSpace;

				vec3 normalWorldSpace;
				mat3 TBN;

				vec2 uv;

				vec4 fragPosDirLightSpace[@1];
				vec4 fragPosSpotLightSpace[@3];
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

			// Light matrices (Each matrix is light's projectionMatrix * viewMatrix)
			uniform mat4 dirLightMatrices[@1];
			uniform mat4 spotLightMatrices[@3];

			void main()
			{
				jff_output.fragPosModelSpace = vertexPosModelSpace;
				jff_output.fragPosWorldSpace = modelMatrix * vec4(vertexPosModelSpace, 1.0);

				jff_output.normalWorldSpace = normalize(normalMatrix * normalize(normalModelSpace));
				vec3 tangentWorldSpace = normalize(normalMatrix * normalize(tangentModelSpace));
				vec3 bitangentWorldSpace = normalize(normalMatrix * normalize(bitangentModelSpace));
				jff_output.TBN = mat3(tangentWorldSpace, bitangentWorldSpace, jff_output.normalWorldSpace);

				jff_output.uv = uvModelSpace.xy;

				for(int i = 0; i < @1; ++i)
				{
					jff_output.fragPosDirLightSpace[i] = dirLightMatrices[i] * jff_output.fragPosWorldSpace;
				}
				for(int i = 0; i < @3; ++i)
				{
					jff_output.fragPosSpotLightSpace[i] = spotLightMatrices[i] * jff_output.fragPosWorldSpace;
				}

				gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosModelSpace, 1.0);
			}
		)glsl";

	// Replace max num lights of each type by concrete numbers
	std::regex dirLights("@1");
	std::regex pointLights("@2");
	std::regex spotLights("@3");

	std::string codeReplaced = std::regex_replace(code, dirLights, std::to_string(params.maxDirLights));
	codeReplaced = std::regex_replace(codeReplaced, pointLights, std::to_string(params.maxPointLights));
	codeReplaced = std::regex_replace(codeReplaced, spotLights, std::to_string(params.maxSpotLights));

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << codeReplaced;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderBlinnPhongGL::getFragmentShaderCode(const Params& params) const
{
	static std::string attributesCode =
		R"glsl(
			in VertexShaderOutput
			{
				vec3 fragPosModelSpace;
				vec4 fragPosWorldSpace;

				vec3 normalWorldSpace;
				mat3 TBN;

				vec2 uv;

				vec4 fragPosDirLightSpace[@1];
				vec4 fragPosSpotLightSpace[@3];
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

			// Light attributes

			struct DirectionalLight
			{
				vec3 direction;
				vec3 color;
				float intensity;
				float castShadows;
				sampler2D shadowMap;
			};

			struct PointLight
			{
				vec3 position;
				vec3 color;
				float intensity;
				float linearAttenuationFactor;
				float quadraticAttenuationFactor;
				float castShadows;
				samplerCube shadowMap;
				float farPlane;
			};

			struct SpotLight
			{
				vec3 position;
				vec3 direction;
				vec3 color;
				float intensity;
				float linearAttenuationFactor;
				float quadraticAttenuationFactor;
				float innerHalfAngleCutoff;
				float outerHalfAngleCutoff;
				float castShadows;
				sampler2D shadowMap;
			};

			uniform DirectionalLight directionalLights[@1];
			uniform PointLight pointLights[@2];
			uniform SpotLight spotLights[@3];

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
			vec4 lightmap;
			vec4 reflection;

			// Normal used to do all calculations. It can come from normal mapping or from the primitive
			vec3 normalWorldSpace;

			// UV used for texture sampling calculations
			vec2 uv;

			// Environment maps
			uniform samplerCube envMap;

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

			// ---------------------------------- LIGHT FUNCTIONS ---------------------------------- //
			
			vec3 ambientFunction(vec3 lightColor)
			{
				return lightColor * ambient.xyz;
			}

			vec3 diffuseFunction(vec3 lightColor, vec3 lightDirWorldSpace)
			{
				float diffuseIncidence = max(dot(-normalize(lightDirWorldSpace), normalWorldSpace), 0.0);
				return diffuseIncidence * lightColor * diffuse.xyz;
			}

			vec3 specularFunction(vec3 lightColor, vec3 lightDirWorldSpace)
			{
				vec3 fragToLightWorldSpace = normalize(-lightDirWorldSpace);
				vec3 fragToLightDirViewSpace = (viewMatrix * vec4(fragToLightWorldSpace, 0.0)).xyz; // Can mult by view matrix because it doesn't have scale transformations

				vec3 cameraPosViewSpace = vec3(0.0);
				vec3 fragPosViewSpace = (viewMatrix * jff_input.fragPosWorldSpace).xyz;
				vec3 fragToCamDirViewSpace = normalize(cameraPosViewSpace - fragPosViewSpace);

				vec3 halfwayVectorViewSpace = normalize(fragToLightDirViewSpace + fragToCamDirViewSpace);
				vec3 normalViewSpace = (viewMatrix * vec4(normalWorldSpace, 0.0)).xyz; // Can mult by view matrix because it doesn't have scale transformations
				
				float specularIncidence = pow(max(dot(halfwayVectorViewSpace, normalViewSpace), 0.0), shininess.r);

				return specularIncidence * lightColor * specular.xyz;
			}
			
			// ---------------------------------- SHADOW FUNCTIONS ---------------------------------- //

			float directionalLightShadowCast(int index) // 1.0: fragment in shadows; 0.0: fragment not in shadows; Middle value: penumbra (soft shadows)
			{
				vec4 fragPosLightSpace = jff_input.fragPosDirLightSpace[index];
				vec3 fragPosLightSpaceNDC = fragPosLightSpace.xyz / fragPosLightSpace.w; // From clip space [-w,w] to Normalice Device Coordinates [-1,1]
				fragPosLightSpaceNDC = fragPosLightSpaceNDC * 0.5 + 0.5; // From [-1,1] to [0,1]

				// Soften shadows by taking an average of neighbours of a texel in shadowmap. PFC (Percentage-Closer Filtering)
				float inShadows = 0.0;
				float currentDepth = fragPosLightSpaceNDC.z;
				vec2 texelSize = 1.0 / textureSize(directionalLights[index].shadowMap, 0); // Texel size (in normalized space) in LOD 0
				for(int x = -1; x <= 1; ++x)
				{
					for(int y = -1; y <= 1; ++y)
					{
						float closestDepth = texture(directionalLights[index].shadowMap, fragPosLightSpaceNDC.xy + vec2(x,y) * texelSize).r;
						inShadows += currentDepth > closestDepth ? 1.0 : 0.0;			
					}
				}
	
				inShadows /= 9.0; // Average shadow value
				inShadows = currentDepth > 1.0 ? 0.0 : inShadows; // If current depth is far beyond shadow frustum's far plane, it doesn't cast shadows

				return inShadows * directionalLights[index].castShadows;
			}

			float pointLightShadowCast(int index)
			{
				vec3 lightToFragDirWorldSpace = jff_input.fragPosWorldSpace.xyz - pointLights[index].position; // No need to normalize when sampling a cubemap

				// Soften shadows by taking an average of neighbours of a texel in shadow cubemap. PFC (Percentage-Closer Filtering)
				float inShadows = 0.0;
				float currentLinearDepth = length(lightToFragDirWorldSpace);
				const int samples = 20;
				vec3 sampleDirectionOffsets[samples] = vec3[]
				(
					vec3( 1, 1, 1), vec3( 1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
					vec3( 1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
					vec3( 1, 1, 0), vec3( 1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
					vec3( 1, 0, 1), vec3(-1, 0, 1), vec3( 1, 0, -1), vec3(-1, 0, -1),
					vec3( 0, 1, 1), vec3( 0, -1, 1), vec3( 0, -1, -1), vec3( 0, 1, -1)
				);
				float diskRadius = 0.02;

				for(int i = 0; i < samples; ++i)
				{
					float closestLinearDepth = texture(pointLights[index].shadowMap, lightToFragDirWorldSpace + sampleDirectionOffsets[i] * diskRadius).r; // Linear depth in range [0,1]
					closestLinearDepth *= pointLights[index].farPlane; // Linear depth from [0,1] to [0,zFar]
					inShadows += currentLinearDepth > closestLinearDepth ? 1.0 : 0.0;
				}
				inShadows /= float(samples);
				
				return inShadows * pointLights[index].castShadows;
			}

			float spotLightShadowCast(int index) // 1.0: fragment in shadows; 0.0: fragment not in shadows
			{
				vec4 fragPosLightSpace = jff_input.fragPosSpotLightSpace[index];
				vec3 fragPosLightSpaceNDC = fragPosLightSpace.xyz / fragPosLightSpace.w; // From clip space [-w,w] to Normalice Device Coordinates [-1,1]
				fragPosLightSpaceNDC = fragPosLightSpaceNDC * 0.5 + 0.5; // From [-1,1] to [0,1]

				// Soften shadows by taking an average of neighbours of a texel in shadowmap. PFC (Percentage-Closer Filtering)
				float inShadows = 0.0;
				float currentDepth = fragPosLightSpaceNDC.z;
				vec2 texelSize = 1.0 / textureSize(spotLights[index].shadowMap, 0); // Texel size (in normalized space) in LOD 0
				for(int x = -1; x <= 1; ++x)
				{
					for(int y = -1; y <= 1; ++y)
					{
						float closestDepth = texture(spotLights[index].shadowMap, fragPosLightSpaceNDC.xy + vec2(x,y) * texelSize).r;
						inShadows += currentDepth > closestDepth ? 1.0 : 0.0;		
					}
				}
	
				inShadows /= 9.0; // Average shadow value
				inShadows = currentDepth > 1.0 ? 0.0 : inShadows; // If current depth is far beyond shadow frustum's far plane, it doesn't cast shadows

				return inShadows * spotLights[index].castShadows;
			}

			// ---------------------------------- LIGHT CONTRIBUTION FUNCTIONS ---------------------------------- //
			
			vec3 directionalLightsContrib()
			{
				vec3 ambientContrib = vec3(0.0);
				vec3 diffuseContrib = vec3(0.0);
				vec3 specularContrib = vec3(0.0);

				for(int i = 0; i < @1; ++i)
				{
					vec3 lightColor = directionalLights[i].color * directionalLights[i].intensity;
					vec3 lightDirWorldSpace = directionalLights[i].direction;

					float inShadows = directionalLightShadowCast(i);

					ambientContrib += ambientFunction(lightColor); // Ambient is not affected by shadow casting
					diffuseContrib += diffuseFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);
					specularContrib += specularFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);
				}

				return ambientContrib + diffuseContrib + specularContrib;
			}

			vec3 pointLightsContrib()
			{
				vec3 ambientContrib = vec3(0.0);
				vec3 diffuseContrib = vec3(0.0);
				vec3 specularContrib = vec3(0.0);

				for(int i = 0; i < @2; ++i)
				{
					float dist = length(jff_input.fragPosWorldSpace.xyz - pointLights[i].position);
					float Kc = 1.0; // Avoid divide by zero when attenuation is calculated
					float Kl = pointLights[i].linearAttenuationFactor;
					float Kq = pointLights[i].quadraticAttenuationFactor;
					float attenuation = 1.0 / (Kc + Kl * dist + Kq * dist * dist);
					vec3 lightColor = pointLights[i].color * pointLights[i].intensity * attenuation;
					vec3 lightDirWorldSpace = normalize(jff_input.fragPosWorldSpace.xyz - pointLights[i].position);

					float inShadows = pointLightShadowCast(i);

					ambientContrib += ambientFunction(lightColor); // Ambient is not affected by shadow casting
					diffuseContrib += diffuseFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);
					specularContrib += specularFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);
				}

				return ambientContrib + diffuseContrib + specularContrib;
			}

			vec3 spotLightsContrib()
			{
				vec3 ambientContrib = vec3(0.0);
				vec3 diffuseContrib = vec3(0.0);
				vec3 specularContrib = vec3(0.0);

				for(int i = 0; i < @3; ++i)
				{
					float dist = length(jff_input.fragPosWorldSpace.xyz - spotLights[i].position);

					float Kc = 1.0; // Avoid divide by zero when attenuation is calculated
					float Kl = spotLights[i].linearAttenuationFactor;
					float Kq = spotLights[i].quadraticAttenuationFactor;
					float attenuation = 1.0 / (Kc + Kl * dist + Kq * dist * dist);

					vec3 spotDirWorldSpace = normalize(spotLights[i].direction);
					vec3 lightDirWorldSpace = normalize(jff_input.fragPosWorldSpace.xyz - spotLights[i].position);

					// Next lines compares cosines, not angles
					float lightIncidenceValue = dot(spotDirWorldSpace, lightDirWorldSpace);
					float innerCutoff = spotLights[i].innerHalfAngleCutoff;
					float outerCutoff = spotLights[i].outerHalfAngleCutoff;
					float coneCutoff = clamp((lightIncidenceValue - outerCutoff) / (innerCutoff - outerCutoff), 0.0, 1.0);

					vec3 ambientLightColor = spotLights[i].color * spotLights[i].intensity * attenuation;
					vec3 lightColor = ambientLightColor * coneCutoff;

					float inShadows = spotLightShadowCast(i);

					ambientContrib += ambientFunction(ambientLightColor); // Ambient is not affected by shadow casting
					diffuseContrib += diffuseFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);
					specularContrib += specularFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);
				}

				return ambientContrib + diffuseContrib + specularContrib;
			}

			// ---------------------------------- ENVIRONMENT CONTRIBUTION FUNCTION ---------------------------------- //

			vec3 environmentFunction()
			{
				vec3 camToFragDirWorldSpace = normalize(vec3(jff_input.fragPosWorldSpace) - cameraPosWorldSpace);
				vec3 reflectionDirWorldSpace = reflect(camToFragDirWorldSpace, normalWorldSpace);
				return texture(envMap, reflectionDirWorldSpace).rgb * reflection.rgb;
			}

			void main()
			{
				// Setup some variables
				uv = jff_input.uv;

				material();
				materialOverrides();

				if(opacity.r <= 0.0) // Discard fragments that are fully transparent
					discard;

				normalMapping();
				FragColor = vec4(directionalLightsContrib() + pointLightsContrib() + spotLightsContrib() + environmentFunction() + emissive.rgb, opacity.r);
			}
		)glsl";

	// Replace max num lights of each type by concrete numbers
	std::regex dirLights("@1");
	std::regex pointLights("@2");
	std::regex spotLights("@3");

	std::string attributesCodeReplaced = std::regex_replace(attributesCode, dirLights, std::to_string(params.maxDirLights));
	attributesCodeReplaced = std::regex_replace(attributesCodeReplaced, pointLights, std::to_string(params.maxPointLights));
	attributesCodeReplaced = std::regex_replace(attributesCodeReplaced, spotLights, std::to_string(params.maxSpotLights));

	std::string mainFunctionCodeReplaced = std::regex_replace(mainFunctionCode, dirLights, std::to_string(params.maxDirLights));
	mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, pointLights, std::to_string(params.maxPointLights));
	mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, spotLights, std::to_string(params.maxSpotLights));

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << attributesCodeReplaced;

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
	oss << params.customCode << mainFunctionCodeReplaced;

	return oss.str();
}
