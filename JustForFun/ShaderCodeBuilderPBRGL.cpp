/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderPBRGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderPBRGL::ShaderCodeBuilderPBRGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderPBRGL")
}

JFF::ShaderCodeBuilderPBRGL::~ShaderCodeBuilderPBRGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderPBRGL")
}

void JFF::ShaderCodeBuilderPBRGL::generateCode(const Params& params,
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode,
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderPBRGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderPBRGL::getVertexShaderCode(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderPBRGL::getFragmentShaderCode(const Params& params) const
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

			// ------------------------- LIGHT ATTRIBUTES ------------------------- //

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

			// ------------------------- MATERIAL OUTPUT ATTRIBUTES ------------------------- //

			// Parallax parameters

			vec4 height;
			vec4 displacement;
			float parallaxIntensity;

			// Metallic/Specular workflow
			
			vec4 baseColor;
			vec4 metalness;
			vec4 roughness;

			vec4 albedo;
			vec4 specular;
			vec4 glossiness;

			// Common PBR parameters

			vec4 normalCamera;
			vec4 emissionColor;
			vec4 ambientOcclusion;
			vec4 opacity;

			// Special PBR parameters

/*TODO*/	vec4 sheen;
/*TODO*/	vec4 clearCoat;
/*TODO*/	vec4 transmission;

			// ------------------------- OTHER GLOBAL VARIABLES ------------------------- //

			// Normal used to do all calculations. It can come from normal mapping or from the primitive
			vec3 normalWorldSpace;

			// UV used for texture sampling calculations
			vec2 uv;

			// ------------------------- ENVIRONMENT CUBEMAPS ------------------------- //

			uniform samplerCube irradianceMap;
			uniform samplerCube prefilteredEnvMap;
			uniform sampler2D BRDFIntegrationMap;

			const float MAX_PREFILTERED_LOD_LEVELS = 5; // Check ReflectionProbeComponent to ensure numPreFilteredMipmaps matches this constant

			// ------------------------- FUNCTION DECLARATIONS ------------------------- //

			// declare next functions because it's used in material() function
			void parallaxMapping();
			void parallaxMappingDisplacement(in sampler2D displacementMap);
			void parallaxMappingHeight(in sampler2D heightMap);
		)glsl";

	static std::string pbrWorkflowAdaptation =
		R"glsl(
			baseColor = albedo;
			roughness = vec4(1.0) - glossiness;
		)glsl";

	static std::string pbrDiffuseMetalness =
		R"glsl(
			// Metal materials doesn't have diffuse contribution. This is remove from diffuse part on metallic workflow 
			// because diffuse texture (baseColor) does contain metallic information
			kd *= 1.0 - metalness.r; 
		)glsl";

	static std::string pbrMetallicF0 =
		R"glsl(
			vec3 F0 = vec3(0.04); // Selected an averaged base reflectivity of many types of dielectric materials
			F0 = mix(F0, baseColor.rgb, metalness.r); // Metal materials have higher base reflectivity and they are tinted. baseColor is used to approximate this behavior
		)glsl";

	static std::string pbrSpecularF0 =
		R"glsl(
			vec3 F0 = specular.rgb;
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
				vec3 normalTangentSpace = normalCamera.xyz * 2.0 - 1.0; // Convert normals from color space [0,1] to vector space [-1,1]
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

	static std::string parallaxMappingCode =
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
		)glsl";

	static std::string shadowCastCode =
		R"glsl(
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
		)glsl";

	/* 
	All contents in PBR section turns around a radiometry formula called 'reflectance equation':
			
		Lo(p,wo) = hemisphere_integral( fr(p,wi,wo) * Li(p,wi) * dot(n,wi) * dwi )
			
	Where:

	* Lo(p,wo) is the sum of reflected radiance of a point (fragment) p in direction wo, which is the direction of the viewer
	* Li(p,wi) is the 'radiance', which is the strength of light coming from a single direction wi to a point (fragment) p. 
		The sum of all incoming lights is called 'irradiance'.
	* fr(p,wi,wo) is the BRDF, which is the 'Bidirectional Reflective Distribution Function' that scales the radiance based on 
		the surface's material properties.
	* dot(n,wi) is the 'incidence', which is a ratio in range [0,1] of angle between each incident light with the surface normal.

	As integrals are not practical for real time rendering, we use an aproximation called 'Riemann sum'.

	The BRDF function used is Cook-Torrance BRDF and is defined as:

		fr(p,wi,wo) = kd * f_lambert + ks * f_cook_torrance

	Where 'kd' and 'ks' are the contribution of light that is refracted and reflected respectively. As PBR is energy-conservative,
	the incoming light energy must be equal to the light refracted plus the energy reflected, so:

		kd = 1.0 - ks

	f_lambert defines the behavior of light that is scattered throught the material surface (refraction) and is defined as:

		f_lambert = albedo / pi

	f_cook_torrance defines the behavior of light that is reflected on the surface and is defined as:

		f_cook_torrance = DFG / (4 * dot(wo,n) * dot(wi,n))
			
	Where:

	* 'D' is 'normal Distribution function' and tell us how many microfacets (micro-mirrors in surface) are aligned to the 
		halfway vector.
	* 'F' is 'Fresnel function' and tell us the amount of light reflected at different surface angles.
	* 'G' is 'Geometry function' and describes the self-shadowing property of microfacets.

	The 'Normal Distribution Function' used here is Trowbridge-Reitz GGX and is an statistical aproximation of microfacet alignment
	given a roughness (alpha):
			
		NDF = pow(alpha,2) / (pi * pow(pow(dot(n,h),2) * (pow(alpha,2) - 1) + 1, 2))

	The 'Fresnel function' used here is Fresnel-Schlick:

		F = F0 + (1 - F0) * pow(1 - dot(h,v), 5)

	Where F0 is the 'Base Reflectivity' which is the amount of reflection of a surface when looking to it perpendiculary. Note that
	there is a distintion here between dielectric and metal surfaces. In dielectrics, F0 has a low value and in metals the value is
	much higher and the refection is 'tinted' (See Fresnel function below and metalness property).
	As Fresnel gives us the amount of reflection, we can use:

		ks = F 

	The 'Geometry Function' as described before is a self-shadowing statistical aproximation of microfacets overshadowing each other
	and is splitted in two: 
	* Geometry shadowing: The microfacets occlude between them independently of the viewpoint.
	* Geometry obstruction: Some microfacets that occlude others from the current point of view.
	The Geometry Function used here is Schlick-Beckmann-GGX and is:

		G(n,v,k) = dot(n,v) / (dot(n,v) * (1 - k) + k)

	Where 'k' is:

		k = pow(alpha + 1,2) / 8	for direct lighting
		k = pow(alpha,2) / 2		for indirect lighting

	And the final version of Geometry (called Geometry-Smith) Function is:

		G_shadowing = G(n,l,k)		where 'l' is the light direction
		G_obstruction = G(n,v,k)	where 'v' is the view direction
				
		G(n,v,l,k) = G_shadowing * G_obstruction

	This equation has two implementations: One for direct lighting and another for indirect lighting.
	*/
	static std::string mainFunctionCode =
		R"glsl(
			// ----------------------------------------------------------------- //			
			// ------------------------------ PBR ------------------------------ //
			// ----------------------------------------------------------------- //
				
			#define PI 3.14159265359

			// ------------------- PBR FOR DIRECT LIGHTING --------------------- //

			float normalDistribution_directLighting(float NdotH)
			{
				float alpha2 = pow(roughness.r, 2);
				float NdotH2 = pow(NdotH, 2); 
				
				return alpha2 / (PI * pow(NdotH2 * (alpha2 - 1.0) + 1.0, 2));
			}		

			vec3 fresnel_directLighting(float HdotV)
			{
				@F0
				return F0 + (vec3(1.0) - F0) * pow(max(1.0 - HdotV, 0.0), 5.0); // Used max() function to avoid micro-negative number in pow() function, which is undefined and returns NaN
			}

			float geometrySimple_directLighting(float dotProduct)
			{
				float k = pow(roughness.r + 1.0, 2.0) / 8.0; // Exclusive function for direct lighting
				return dotProduct / (dotProduct * (1.0 - k) + k);
			}

			float geometry_directLighting(float NdotV, float NdotL)
			{
				float G_shadowing = geometrySimple_directLighting(NdotL);
				float G_obstruction = geometrySimple_directLighting(NdotV);
				return G_obstruction * G_shadowing;
			}

			vec3 BRDF_directLighting(float NdotV, float NdotL, float NdotH, float HdotV)
			{
				float D = normalDistribution_directLighting(NdotH);
				vec3 F = fresnel_directLighting(HdotV);
				float G = geometry_directLighting(NdotV, NdotL);
				float NdotWo = NdotV;
				float NdotWi = NdotL;
					
				vec3 f_lambert = baseColor.rgb / PI;
				float f_cook_torrance = (D * G) / max(4.0 * NdotWo * NdotWi, 0.001); // 'F' is not included here because it's included in 'ks'. The 'max' function avoids 0 denom

				vec3 ks = F;
				vec3 kd = vec3(1.0) - ks;	
				@pbrDiffuseMetalness

				return kd * f_lambert + ks * f_cook_torrance;
			}

			vec3 directionalLightsContrib()
			{
				vec3 reflectance = vec3(0.0);
				for(int i = 0; i < @1; ++i)
				{
					// Gather light params
					vec3 lightColor = directionalLights[i].color * directionalLights[i].intensity;
					vec3 lightDirWorldSpace = normalize(directionalLights[i].direction);

					float inShadows = directionalLightShadowCast(i);

					// Reflectance params
					vec3 camDirWorldSpace = normalize(cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz);
					vec3 halfwayDirWorldSpace = normalize(-lightDirWorldSpace + camDirWorldSpace);

					float NdotV = max(dot(normalWorldSpace, camDirWorldSpace), 0.0);
					float NdotL = max(dot(normalWorldSpace, -lightDirWorldSpace), 0.0);
					float NdotH = max(dot(normalWorldSpace, halfwayDirWorldSpace), 0.0);
					float HdotV = max(dot(halfwayDirWorldSpace, camDirWorldSpace), 0.0);

					vec3 fr = BRDF_directLighting(NdotV, NdotL, NdotH, HdotV);	// BRDF
					vec3 Li = lightColor;										// Radiance
					float NdotWi = NdotL;										// Incident angle
				
					// Reflectance equation
					reflectance += fr * Li * NdotWi * (1.0 - inShadows);
				}
				return reflectance;
			}

			vec3 pointLightsContrib()
			{
				vec3 reflectance = vec3(0.0);
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

					// Reflectance params
					vec3 camDirWorldSpace = normalize(cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz);
					vec3 halfwayDirWorldSpace = normalize(-lightDirWorldSpace + camDirWorldSpace);

					float NdotV = max(dot(normalWorldSpace, camDirWorldSpace), 0.0);
					float NdotL = max(dot(normalWorldSpace, -lightDirWorldSpace), 0.0);
					float NdotH = max(dot(normalWorldSpace, halfwayDirWorldSpace), 0.0);
					float HdotV = max(dot(halfwayDirWorldSpace, camDirWorldSpace), 0.0);

					vec3 fr = BRDF_directLighting(NdotV, NdotL, NdotH, HdotV);	// BRDF
					vec3 Li = lightColor;										// Radiance
					float NdotWi = NdotL;										// Incident angle

					// Reflectance equation
					reflectance += fr * Li * NdotWi * (1.0 - inShadows);
				}
				return reflectance;
			}

			vec3 spotLightsContrib()
			{
				vec3 reflectance = vec3(0.0);
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

					vec3 lightColor = spotLights[i].color * spotLights[i].intensity * attenuation * coneCutoff;

					float inShadows = spotLightShadowCast(i);

					// Reflectance params
					vec3 camDirWorldSpace = normalize(cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz);
					vec3 halfwayDirWorldSpace = normalize(-lightDirWorldSpace + camDirWorldSpace);

					float NdotV = max(dot(normalWorldSpace, camDirWorldSpace), 0.0);
					float NdotL = max(dot(normalWorldSpace, -lightDirWorldSpace), 0.0);
					float NdotH = max(dot(normalWorldSpace, halfwayDirWorldSpace), 0.0);
					float HdotV = max(dot(halfwayDirWorldSpace, camDirWorldSpace), 0.0);

					vec3 fr = BRDF_directLighting(NdotV, NdotL, NdotH, HdotV);	// BRDF
					vec3 Li = lightColor;										// Radiance
					float NdotWi = NdotL;										// Incident angle

					// Reflectance equation
					reflectance += fr * Li * NdotWi * (1.0 - inShadows);
				}
				return reflectance;
			}

			vec3 PBR_directLightContrib()
			{
				return directionalLightsContrib() + pointLightsContrib() + spotLightsContrib();
			}

			// ------------------- PBR FOR INDIRECT LIGHTING (DIFFUSE PART) --------------------- //

			vec3 fresnel_indirectLighting(float NdotV)
			{
				@F0
				
				// On indirect lighting, NdotV is used instead HdotV because light comes from anywhere and we don't have H vector.
				// Because of that, a 'halo' is created around objects on rough surfaces, so we add roughness to alleviate this effect
				vec3 haloFactor = max(vec3(1.0 - roughness.r), F0);

				return F0 + (haloFactor - F0) * pow(max(1.0 - NdotV, 0.0), 5.0); // Used max() function to avoid micro-negative number in pow() function, which is undefined and returns NaN
			}

			vec3 BRDF_indirectLighting_diffuse(float NdotV)
			{
				vec3 F = fresnel_indirectLighting(NdotV);

				vec3 ks = F;
				vec3 kd = vec3(1.0) - ks;
				vec3 f_lambert = baseColor.rgb; // Don't divide by PI because we calculated this previously (see ShaderCodeBuilderIrradianceGeneratorGL)
	
				return kd * f_lambert; // Diffuse part of BRDF
			}

			vec3 PBR_indirectLightContrib_diffuse()
			{
				// Reflectance params
				vec3 camDirWorldSpace = normalize(cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz);
				float NdotV = max(dot(normalWorldSpace, camDirWorldSpace), 0.0);

				vec3 fr = BRDF_indirectLighting_diffuse(NdotV);
				vec3 irradiance = texture(irradianceMap, normalWorldSpace).rgb; // Check ShaderCodeBuilderIrradianceGeneratorGL for more info

				// Reflectance equation
				vec3 reflectanceDiffuse = fr * irradiance; // Irradiance is the sum of all Li * NdotWi

				return reflectanceDiffuse;
			}

			// ------------------- PBR FOR INDIRECT LIGHTING (SPECULAR PART) --------------------- //

			vec3 PBR_indirectLightContrib_specular()
			{
				/* 
				For the indirect specular reflectance part, the 'split sum' approximation is used: 
					
					Lo(p,wo) = integral(Li(p,Wi) * dWi) * integral(fr(p,Wi,Wo) * NdotWi * dWi)
				
				The first integral is called 'pre-filtered environment map' which info is stored in a mipmapped cubemap.
				
				The second part is called 'BRDF integration map' and it's a 2 color 2D LUT texture where input X axis is NdotWi (range [0,1])
				and input Y axis is roughness (also with range [0,1]). The result is X: BRDF multiplication factor and Y: BRDF sum factor
				
				The approximation assumes that the view direction 'V' (and obviously the reflected direction 'R') matches the normal 'N' on each 
				fragment, which at the same time is used as output sample direction 'Wo'. That is:

					Wo = V = R = N
				*/		

				// Reflectance params
				vec3 camDirWorldSpace = normalize(cameraPosWorldSpace - jff_input.fragPosWorldSpace.xyz);
				vec3 reflectedCamDirWorldSpace = reflect(-camDirWorldSpace, normalWorldSpace);
				float NdotV = max(dot(normalWorldSpace, camDirWorldSpace), 0.0);

				vec3 F = fresnel_indirectLighting(NdotV);
		
				// Pre-filtered environment map
				float lod = MAX_PREFILTERED_LOD_LEVELS * roughness.r;
				vec3 prefilteredColor = textureLod(prefilteredEnvMap, reflectedCamDirWorldSpace, lod).rgb;

				// BRDF integration map
				vec2 envBRDF = texture2D(BRDFIntegrationMap, vec2(NdotV, roughness.r)).xy; // TODO: why NdotV ?? It should be NdotWi

				// Reflectance equation
				vec3 reflectanceSpecular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
				return reflectanceSpecular;
			}

			// ------------------- PBR FOR INDIRECT LIGHTING --------------------- //
			
			vec3 PBR_indirectLightContrib()
			{
				return (PBR_indirectLightContrib_diffuse() + PBR_indirectLightContrib_specular()) * ambientOcclusion.r;
			}

			// ------------------------- MAIN FUNCTION ------------------------- //

			void main()
			{
				// Setup some variables
				uv = jff_input.uv;

				material();
				materialOverrides();

				@pbrWorkflowAdaptation
				roughness.r = max(roughness.r, 0.01); // Limit the roughness level to 0.01 to avoid divide by zero errors 

				if(opacity.r <= 0.0) // Discard fragments that are fully transparent
					discard;

				normalMapping();
				FragColor = vec4(PBR_directLightContrib() + PBR_indirectLightContrib() + emissionColor.rgb, opacity.r);
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

	// Replace PBR parameters depending on the workflow
	std::regex pbrDiffuseMetalnessRegex("@pbrDiffuseMetalness");
	std::regex workflowF0Regex("@F0");
	std::regex workflowAdaptationRegex("@pbrWorkflowAdaptation");
	if (params.pbrWorkflow == ShaderCodeBuilder::PBRWorkflow::METALLIC)
	{
		mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, pbrDiffuseMetalnessRegex, pbrDiffuseMetalness);
		mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, workflowF0Regex, pbrMetallicF0);
		mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, workflowAdaptationRegex, "");
	}
	else // Specular workflow
	{
		mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, pbrDiffuseMetalnessRegex, "");
		mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, workflowF0Regex, pbrSpecularF0);
		mainFunctionCodeReplaced = std::regex_replace(mainFunctionCodeReplaced, workflowAdaptationRegex, pbrWorkflowAdaptation);
	}

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
	oss << params.customCode << parallaxMappingCode << shadowCastCode << mainFunctionCodeReplaced;

	return oss.str();
}
