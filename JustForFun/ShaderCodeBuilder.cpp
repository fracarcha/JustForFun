/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilder.h"

const std::string JFF::ShaderCodeBuilder::MODEL_MATRIX("modelMatrix");
const std::string JFF::ShaderCodeBuilder::VIEW_MATRIX("viewMatrix");
const std::string JFF::ShaderCodeBuilder::PROJECTION_MATRIX("projectionMatrix");
const std::string JFF::ShaderCodeBuilder::NORMAL_MATRIX("normalMatrix");
const std::string JFF::ShaderCodeBuilder::CUBEMAP_VIEW_MATRICES("cubemapViewMatrices");

const std::string JFF::ShaderCodeBuilder::INPUT_UV_0("uv");

const std::string JFF::ShaderCodeBuilder::BASE_COLOR("baseColor");
const std::string JFF::ShaderCodeBuilder::NORMAL_CAMERA("normalCamera");
const std::string JFF::ShaderCodeBuilder::EMISSION_COLOR("emissionColor");
const std::string JFF::ShaderCodeBuilder::METALNESS("metalness");
const std::string JFF::ShaderCodeBuilder::DIFFUSE_ROUGHNESS("roughness");
const std::string JFF::ShaderCodeBuilder::AMBIENT_OCCLUSION("ambientOcclusion");
const std::string JFF::ShaderCodeBuilder::SHEEN("sheen");
const std::string JFF::ShaderCodeBuilder::CLEAR_COAT("clearCoat");
const std::string JFF::ShaderCodeBuilder::TRANSMISSION("transmission");
				  
const std::string JFF::ShaderCodeBuilder::HEIGHT("height");
const std::string JFF::ShaderCodeBuilder::DISPLACEMENT("displacement");
const std::string JFF::ShaderCodeBuilder::PARALLAX_INTENSITY("parallaxIntensity");
const std::string JFF::ShaderCodeBuilder::DIFFUSE("diffuse");
const std::string JFF::ShaderCodeBuilder::SPECULAR("specular");
const std::string JFF::ShaderCodeBuilder::AMBIENT("ambient");
const std::string JFF::ShaderCodeBuilder::EMISSIVE("emissive");
const std::string JFF::ShaderCodeBuilder::NORMAL("normal");
const std::string JFF::ShaderCodeBuilder::SHININESS("shininess");
const std::string JFF::ShaderCodeBuilder::OPACITY("opacity");
const std::string JFF::ShaderCodeBuilder::LIGHTMAP("lightmap");
const std::string JFF::ShaderCodeBuilder::REFLECTION("reflection");

const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_OUTPUT_COLOR("ppOutputColor");
const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_OUTPUT_COLOR_2("ppOutputColor2");
const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_FRAGMENT_WORLD_POS("ppFragWorldPos");
const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_NORMAL_WORLD_DIR("ppNormalWorldDir");
const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_ALBEDO_SPECULAR("ppAlbedoSpecular");
const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_AMBIENT_SHININESS("ppAmbientShininess");
const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_REFLECTION("ppReflection");
const std::string JFF::ShaderCodeBuilder::POST_PROCESSING_EMISSIVE("ppEmissive");
const std::string JFF::ShaderCodeBuilder::MIPMAP_LEVEL("mipLevel");
const std::string JFF::ShaderCodeBuilder::MIPMAP_LEVELS("mipLevels");
const std::string JFF::ShaderCodeBuilder::THRESHOLD("threshold");
const std::string JFF::ShaderCodeBuilder::INTENSITY("intensity");
const std::string JFF::ShaderCodeBuilder::RANDOM_ROTATED_TANGENTS("randomRotatedTangents");
const std::string JFF::ShaderCodeBuilder::HEMISPHERE_RADIUS("hemisphereRadius");
const std::string JFF::ShaderCodeBuilder::NUM_HEMISPHERE_SAMPLES("numSamples");
const std::string JFF::ShaderCodeBuilder::HEMISPHERE_SAMPLES("hemisphereSamplesTangentSpace");
const std::string JFF::ShaderCodeBuilder::EQUIRECTANGULAR_TEX("equirectangularTex");
const std::string JFF::ShaderCodeBuilder::ROUGHNESS("roughness");
const std::string JFF::ShaderCodeBuilder::ENVIRONMENT_MAP_FACE_WIDTH("envMapFaceWidth");

const std::string JFF::ShaderCodeBuilder::ENVIRONMENT_MAP("envMap");
const std::string JFF::ShaderCodeBuilder::IRRADIANCE_MAP("irradianceMap");
const std::string JFF::ShaderCodeBuilder::PRE_FILTERED_MAP("prefilteredEnvMap");
const std::string JFF::ShaderCodeBuilder::BRDF_INTEGRATION_MAP("BRDFIntegrationMap");

const std::string JFF::ShaderCodeBuilder::DIRECTIONAL_LIGHT_MATRIX("dirLightMatrix");
const std::string JFF::ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT("directionalLight");
const std::string JFF::ShaderCodeBuilder::DIRECTIONAL_LIGHT_MATRICES("dirLightMatrices");
const std::string JFF::ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT_ARRAY("directionalLights");
	const std::string JFF::ShaderCodeBuilder::DIR_LIGHT_DIRECTION("direction");
	const std::string JFF::ShaderCodeBuilder::DIR_LIGHT_COLOR("color");
	const std::string JFF::ShaderCodeBuilder::DIR_LIGHT_INTENSITY("intensity");
	const std::string JFF::ShaderCodeBuilder::DIR_LIGHT_CAST_SHADOWS("castShadows");
	const std::string JFF::ShaderCodeBuilder::DIR_LIGHT_SHADOW_MAP("shadowMap");

const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_STRUCT("pointLight");
const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY("pointLights");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_POSITION("position");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_COLOR("color");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_INTENSITY("intensity");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_LINEAR_ATTENUATION_FACTOR("linearAttenuationFactor");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_QUADRATIC_ATTENUATION_FACTOR("quadraticAttenuationFactor");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_CAST_SHADOWS("castShadows");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_SHADOW_MAP("shadowMap");
	const std::string JFF::ShaderCodeBuilder::POINT_LIGHT_FAR_PLANE("farPlane");

const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_MATRIX("spotLightMatrix");
const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_STRUCT("spotLight");
const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_MATRICES("spotLightMatrices");
const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY("spotLights");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_POSITION("position");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_DIRECTION("direction");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_COLOR("color");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_INTENSITY("intensity");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_LINEAR_ATTENUATION_FACTOR("linearAttenuationFactor");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_QUADRATIC_ATTENUATION_FACTOR("quadraticAttenuationFactor");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_INNER_HALF_ANGLE_CUTOFF("innerHalfAngleCutoff");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_OUTER_HALF_ANGLE_CUTOFF("outerHalfAngleCutoff");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_CAST_SHADOWS("castShadows");
	const std::string JFF::ShaderCodeBuilder::SPOT_LIGHT_SHADOW_MAP("shadowMap");

const std::string JFF::ShaderCodeBuilder::LIGHT_POSITION("lightPos");
const std::string JFF::ShaderCodeBuilder::LIGHT_FAR_PLANE("farPlane");

const std::string JFF::ShaderCodeBuilder::MATERIAL_FUNCTION_HEADER("\n\t\t\tvoid material() \n\t\t\t{\n");
const std::string JFF::ShaderCodeBuilder::MATERIAL_FUNCTION_CLOSE_BRACKET("\t\t\t}\n");
const std::string JFF::ShaderCodeBuilder::MATERIAL_OVERRIDES_EMPTY_FUNCTION("\t\t\tvoid materialOverrides() {}\n");