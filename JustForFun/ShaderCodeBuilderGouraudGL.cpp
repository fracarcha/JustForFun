/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderGouraudGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderGouraudGL::ShaderCodeBuilderGouraudGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderGouraudGL")
}

JFF::ShaderCodeBuilderGouraudGL::~ShaderCodeBuilderGouraudGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderGouraudGL")
}

void JFF::ShaderCodeBuilderGouraudGL::generateCode(const Params& params, 
	std::string& outVertexShaderCode, 
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	// TODO: geometry
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderGouraudGL::getShaderVersionLine(const Params& params) const
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

inline std::string JFF::ShaderCodeBuilderGouraudGL::getVertexShaderCode(const Params& params) const
{
	static std::string attributesCode =
		R"glsl(
			layout (location = 0) in vec3 vertexPosModelSpace;
			layout (location = 1) in vec3 normalModelSpace;
			layout (location = 2) in vec3 tangentModelSpace;
			layout (location = 3) in vec3 bitangentModelSpace;
			layout (location = 4) in vec3 uvModelSpace;

			out VertexShaderOutput
			{
				vec4 vertexColor;
			} jff_output;

			vec4 fragPosWorldSpace;
			vec3 normalWorldSpace;
			vec2 uv;

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

			// Light attributes
			struct DirectionalLight
			{
				vec3 direction;
				vec3 color;
				float intensity;
			};

			struct PointLight
			{
				vec3 position;
				vec3 color;
				float intensity;
				float linearAttenuationFactor;
				float quadraticAttenuationFactor;
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
			};

			uniform DirectionalLight directionalLights[@1];
			uniform PointLight pointLights[@2];
			uniform SpotLight spotLights[@3];

			// Material output attributes
			vec4 diffuse;
			vec4 specular;
			vec4 ambient;
			vec4 emissive;
			vec4 height;
			vec4 normal;
			vec4 shininess;
			vec4 opacity;
			vec4 displacement;
			vec4 lightmap;
			vec4 reflection;
		)glsl";

	static std::string mainFunctionCode =
		R"glsl(
			// Light functions
			
			vec3 ambientFunction(vec3 lightColor)
			{
				return lightColor * ambient.xyz;
			}

			vec3 diffuseFunction(vec3 lightColor, vec3 lightDirWorldSpace)
			{
				float diffuseIncidence = max(dot(-normalize(lightDirWorldSpace), normalize(normalWorldSpace)), 0.0);
				return diffuseIncidence * lightColor * diffuse.xyz;
			}

			vec3 specularFunction(vec3 lightColor, vec3 lightDirWorldSpace)
			{
				vec3 lightReflectionWorldSpace = normalize(reflect(normalize(lightDirWorldSpace), normalize(normalWorldSpace)));
				vec3 lightReflectionViewSpace = (viewMatrix * vec4(lightReflectionWorldSpace, 0.0)).xyz; // Can mult by view matrix because it doesn't have scale transformations
				vec3 cameraPosViewSpace = vec3(0.0);
				vec3 fragPosViewSpace = (viewMatrix * fragPosWorldSpace).xyz;
				vec3 fragToCamDirViewSpace = normalize(cameraPosViewSpace - fragPosViewSpace);
				float specularIncidence = pow(max(dot(lightReflectionViewSpace, fragToCamDirViewSpace), 0.0), shininess.r);

				return specularIncidence * lightColor * specular.xyz;
			}

			// Light contribution functions

			vec3 directionalLightsContrib()
			{
				vec3 ambientContrib = vec3(0.0);
				vec3 diffuseContrib = vec3(0.0);
				vec3 specularContrib = vec3(0.0);

				for(int i = 0; i < @1; ++i)
				{
					vec3 lightColor = directionalLights[i].color * directionalLights[i].intensity;
					vec3 lightDirWorldSpace = directionalLights[i].direction;

					ambientContrib += ambientFunction(lightColor);
					diffuseContrib += diffuseFunction(lightColor, lightDirWorldSpace);
					specularContrib += specularFunction(lightColor, lightDirWorldSpace);
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
					float dist = length(fragPosWorldSpace.xyz - pointLights[i].position);
					float Kc = 1.0; // Avoid divide by zero when attenuation is calculated
					float Kl = pointLights[i].linearAttenuationFactor;
					float Kq = pointLights[i].quadraticAttenuationFactor;
					float attenuation = 1.0 / (Kc + Kl * dist + Kq * dist * dist);
					vec3 lightColor = pointLights[i].color * pointLights[i].intensity * attenuation;
					vec3 lightDirWorldSpace = normalize(fragPosWorldSpace.xyz - pointLights[i].position);

					ambientContrib += ambientFunction(lightColor);
					diffuseContrib += diffuseFunction(lightColor, lightDirWorldSpace);
					specularContrib += specularFunction(lightColor, lightDirWorldSpace);
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
					float dist = length(fragPosWorldSpace.xyz - spotLights[i].position);

					float Kc = 1.0; // Avoid divide by zero when attenuation is calculated
					float Kl = spotLights[i].linearAttenuationFactor;
					float Kq = spotLights[i].quadraticAttenuationFactor;
					float attenuation = 1.0 / (Kc + Kl * dist + Kq * dist * dist);

					vec3 spotDirWorldSpace = normalize(spotLights[i].direction);
					vec3 lightDirWorldSpace = normalize(fragPosWorldSpace.xyz - spotLights[i].position);

					// Next lines compares cosines, not angles
					float lightIncidenceValue = dot(spotDirWorldSpace, lightDirWorldSpace);
					float innerCutoff = spotLights[i].innerHalfAngleCutoff;
					float outerCutoff = spotLights[i].outerHalfAngleCutoff;
					float coneCutoff = clamp((lightIncidenceValue - outerCutoff) / (innerCutoff - outerCutoff), 0.0, 1.0);

					vec3 ambientLightColor = spotLights[i].color * spotLights[i].intensity * attenuation;
					vec3 lightColor = ambientLightColor * coneCutoff;

					ambientContrib += ambientFunction(ambientLightColor);
					diffuseContrib += diffuseFunction(lightColor, lightDirWorldSpace);
					specularContrib += specularFunction(lightColor, lightDirWorldSpace);
				}

				return ambientContrib + diffuseContrib + specularContrib;
			}

			void main()
			{
				normalWorldSpace = normalize(normalMatrix * normalize(normalModelSpace));
				uv = uvModelSpace.xy;
				fragPosWorldSpace = modelMatrix * vec4(vertexPosModelSpace, 1.0);

				material();
				materialOverrides();
				jff_output.vertexColor = vec4(directionalLightsContrib() + pointLightsContrib() + spotLightsContrib() + emissive.rgb, opacity.r);

				gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosModelSpace, 1.0);
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
	oss << getShaderVersionLine(params) << attributesCodeReplaced << params.customCode << mainFunctionCodeReplaced;

	return oss.str();
}

inline std::string JFF::ShaderCodeBuilderGouraudGL::getFragmentShaderCode(const Params& params) const
{
	static std::string code =
		R"glsl(
			in VertexShaderOutput
			{
				vec4 vertexColor;
			} jff_input;

			layout (location = 0) out vec4 FragColor;		// Color attachment 0

			void main()
			{
				FragColor = jff_input.vertexColor;
			}
		)glsl";

	// Assemble code
	std::ostringstream oss;
	oss << getShaderVersionLine(params) << code;

	return oss.str();
}
