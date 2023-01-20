/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL()
{
    JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL")
}

JFF::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL::~ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL()
{
    JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL")
}

void JFF::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL::generateCode(
    const Params& params,
    std::string& outVertexShaderCode,
    std::string& outGeometryShaderCode,
    std::string& outFragmentShaderCode) const
{
    outVertexShaderCode = getVertexShaderCode(params);
    outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL::getVertexShaderCode(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL::getFragmentShaderCode(const Params& params) const
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

			// Light params

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

			uniform SpotLight spotLight;
			uniform mat4 spotLightMatrix; // Light matrices (Each matrix is light's projectionMatrix * viewMatrix)

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
				vec3 fragPosViewSpace = (viewMatrix * fragPosWorldSpace).xyz;
				vec3 fragToCamDirViewSpace = normalize(cameraPosViewSpace - fragPosViewSpace);

				vec3 halfwayVectorViewSpace = normalize(fragToLightDirViewSpace + fragToCamDirViewSpace);
				vec3 normalViewSpace = (viewMatrix * vec4(normalWorldSpace, 0.0)).xyz; // Can mult by view matrix because it doesn't have scale transformations
				
				float specularIncidence = pow(max(dot(halfwayVectorViewSpace, normalViewSpace), 0.0), shininess.r);

				return specularIncidence * lightColor * specular.xyz;
			}
			
			// ---------------------------------- SHADOW FUNCTIONS ---------------------------------- //

			float spotLightShadowCast() // 1.0: fragment in shadows; 0.0: fragment not in shadows
			{
				vec4 fragPosLightSpace = spotLightMatrix * fragPosWorldSpace;
				vec3 fragPosLightSpaceNDC = fragPosLightSpace.xyz / fragPosLightSpace.w; // From clip space [-w,w] to Normalice Device Coordinates [-1,1]
				fragPosLightSpaceNDC = fragPosLightSpaceNDC * 0.5 + 0.5; // From [-1,1] to [0,1]

				// Soften shadows by taking an average of neighbours of a texel in shadowmap. PFC (Percentage-Closer Filtering)
				float inShadows = 0.0;
				float currentDepth = fragPosLightSpaceNDC.z;
				vec2 texelSize = 1.0 / textureSize(spotLight.shadowMap, 0); // Texel size (in normalized space) in LOD 0
				for(int x = -1; x <= 1; ++x)
				{
					for(int y = -1; y <= 1; ++y)
					{
						float closestDepth = texture(spotLight.shadowMap, fragPosLightSpaceNDC.xy + vec2(x,y) * texelSize).r;
						inShadows += currentDepth > closestDepth ? 1.0 : 0.0;		
					}
				}
	
				inShadows /= 9.0; // Average shadow value
				inShadows = currentDepth > 1.0 ? 0.0 : inShadows; // If current depth is far beyond shadow frustum's far plane, it doesn't cast shadows

				return inShadows * spotLight.castShadows;
			}

			// ---------------------------------- LIGHT CONTRIBUTION FUNCTIONS ---------------------------------- //

			vec3 spotLightsContrib()
			{
				float dist = length(fragPosWorldSpace.xyz - spotLight.position);

				float Kc = 1.0; // Avoid divide by zero when attenuation is calculated
				float Kl = spotLight.linearAttenuationFactor;
				float Kq = spotLight.quadraticAttenuationFactor;
				float attenuation = 1.0 / (Kc + Kl * dist + Kq * dist * dist);

				vec3 spotDirWorldSpace = normalize(spotLight.direction);
				vec3 lightDirWorldSpace = normalize(fragPosWorldSpace.xyz - spotLight.position);

				// Next lines compares cosines, not angles
				float lightIncidenceValue = dot(spotDirWorldSpace, lightDirWorldSpace);
				float innerCutoff = spotLight.innerHalfAngleCutoff;
				float outerCutoff = spotLight.outerHalfAngleCutoff;
				float coneCutoff = clamp((lightIncidenceValue - outerCutoff) / (innerCutoff - outerCutoff), 0.0, 1.0);

				vec3 ambientLightColor = spotLight.color * spotLight.intensity * attenuation;
				vec3 lightColor = ambientLightColor * coneCutoff;

				float inShadows = spotLightShadowCast();

				vec3 ambientContrib = ambientFunction(ambientLightColor); // Ambient is not affected by shadow casting
				vec3 diffuseContrib = diffuseFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);
				vec3 specularContrib = specularFunction(lightColor, lightDirWorldSpace) * (1.0 - inShadows);

				return ambientContrib + diffuseContrib + specularContrib;
			}

			// ---------------------------------- MAIN FUNCTION ---------------------------------- //

			void main()
			{
				// Setup some variables
				uv = jff_input.uv;
				extractFromGBuffer();

				FragColor = vec4(spotLightsContrib(), 1.0);
			}
		)glsl";

	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
