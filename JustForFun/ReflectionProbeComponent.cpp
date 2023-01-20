/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ReflectionProbeComponent.h"

#include "Log.h"
#include "Engine.h"
#include "FileSystemSetup.h"

#include "ShaderCodeBuilder.h"
#include "PreprocessIrradianceGenerator.h"
#include "PreprocessPreFilteredEnvironmentMapGenerator.h"
#include "PreprocessBRDFIntegrationMapGenerator.h"

#include <regex>

extern std::shared_ptr<JFF::Cubemap> createCubemap(JFF::Engine* const engine, const char* name, const char* assetFilePath);
extern std::shared_ptr<JFF::Cubemap> createCubemap(JFF::Engine* const engine, const JFF::Cubemap::Params& params);
extern std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const JFF::Texture::Params& params);

JFF::ReflectionProbeComponent::ReflectionProbeComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled,
	const char* cubemapAssetFilepath) :
	EnvironmentMapComponent(gameObject, name, initiallyEnabled),
	assetFilepath(cubemapAssetFilepath),
	envMap(),

	irradianceMapFaceWidth(32u),
	preFilteredMapFaceWidth(128u),
	numPreFilteredMipmaps(5u), // Check PBR shaders to ensure this variable matches with MAX_PREFILTERED_LOD_LEVELS
	BRDFIntegrationMapWidth(512u),

	irradianceMap(),
	preFilteredMap(),
	BRDFIntegrationMap()
{
	JFF_LOG_INFO("Ctor ReflectionProbeComponent")
}

JFF::ReflectionProbeComponent::~ReflectionProbeComponent()
{
	JFF_LOG_INFO("Dtor ReflectionProbeComponent")
}

void JFF::ReflectionProbeComponent::onStart()
{
	// Load cubemap from asset 
	std::string assetFullPath = std::regex_replace(assetFilepath, std::regex(R"raw(/)raw"), JFF_SLASH_STRING);
	envMap = createCubemap(gameObject->engine, "Reflection probe cubemap", assetFullPath.c_str());

	// TODO: Don't generate next maps if they are already generated

	// Irradiance map generation (Used for diffuse part of indirect lighting)
	generateIrradianceMap();

	// Pre-filtered map and BRDF integration map generation (Used for specular part of indirect lighting)
	generatePreFilteredEnvironmentMap();
	generateBRDFIntegrationMap();
}

void JFF::ReflectionProbeComponent::onEnable()
{
	// Register the environment map in renderer
	gameObject->engine->renderer.lock()->addEnvironmentMap(this);
}

void JFF::ReflectionProbeComponent::onDisable() noexcept
{
	// Unregister the environment map in Renderer
	gameObject->engine->renderer.lock()->removeEnvironmentMap(this);
}

void JFF::ReflectionProbeComponent::onDestroy() noexcept
{
	// Destroy cubemaps
	envMap->destroy();
	irradianceMap->destroy();
	preFilteredMap->destroy();
	BRDFIntegrationMap->destroy();
}

void JFF::ReflectionProbeComponent::sendEnvironmentMap(RenderComponent* const renderComponent)
{
	// Send the cubemap to the shader for its use
	renderComponent->sendEnvironmentMap(envMap, irradianceMap, preFilteredMap, BRDFIntegrationMap);
}

inline void JFF::ReflectionProbeComponent::generateIrradianceMap()
{
	auto io = gameObject->engine->io.lock();

	// Create an irradiance map from envMap
	std::shared_ptr<PreprocessIrradianceGenerator> irradianceGen =
		std::make_shared<PreprocessIrradianceGenerator>(gameObject->engine, envMap, irradianceMapFaceWidth);
	irradianceGen->execute();

	// Extract cubemap info
	std::string irradianceAppendix = "_irradiance.hdr";

	Cubemap::ImageInfo envMapInfo = envMap->getImageInfo();
	std::string imgRight	= std::regex_replace(envMapInfo.imageRightFilename,  std::regex(R"raw(\..*)raw"), irradianceAppendix);
	std::string imgLeft		= std::regex_replace(envMapInfo.imageLeftFilename,	 std::regex(R"raw(\..*)raw"), irradianceAppendix);
	std::string imgTop		= std::regex_replace(envMapInfo.imageTopFilename,	 std::regex(R"raw(\..*)raw"), irradianceAppendix);
	std::string imgBottom	= std::regex_replace(envMapInfo.imageBottomFilename, std::regex(R"raw(\..*)raw"), irradianceAppendix);
	std::string imgBack		= std::regex_replace(envMapInfo.imageBackFilename,	 std::regex(R"raw(\..*)raw"), irradianceAppendix);
	std::string imgFront	= std::regex_replace(envMapInfo.imageFrontFilename,  std::regex(R"raw(\..*)raw"), irradianceAppendix);

	std::string generatedFolder = "Generated";

	imgRight	= generatedFolder + JFF_SLASH_STRING + imgRight;
	imgLeft		= generatedFolder + JFF_SLASH_STRING + imgLeft;
	imgTop		= generatedFolder + JFF_SLASH_STRING + imgTop;
	imgBottom	= generatedFolder + JFF_SLASH_STRING + imgBottom;
	imgBack		= generatedFolder + JFF_SLASH_STRING + imgBack;
	imgFront	= generatedFolder + JFF_SLASH_STRING + imgFront;

	// After generating the irradiance map, load all faces and create a cubemap
	Cubemap::Params irradianceMapParams;

	irradianceMapParams.shaderVariableName = ShaderCodeBuilder::IRRADIANCE_MAP;

	irradianceMapParams.folder		= generatedFolder;

	irradianceMapParams.imgRight	= io->loadImage(imgRight.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	irradianceMapParams.imgLeft		= io->loadImage(imgLeft.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	irradianceMapParams.imgTop		= io->loadImage(imgTop.c_str(),		/* flipVertically */ false, /* HDRImage */ true);
	irradianceMapParams.imgBottom	= io->loadImage(imgBottom.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	irradianceMapParams.imgBack		= io->loadImage(imgBack.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	irradianceMapParams.imgFront	= io->loadImage(imgFront.c_str(),	/* flipVertically */ false, /* HDRImage */ true);

	irradianceMapParams.coordsWrapMode		= { Cubemap::Wrap::CLAMP_TO_EDGE, Cubemap::Wrap::CLAMP_TO_EDGE, Cubemap::Wrap::CLAMP_TO_EDGE };
	irradianceMapParams.filterMode			= { Cubemap::MinificationFilter::LINEAR,Cubemap::MagnificationFilter::LINEAR };
	irradianceMapParams.numColorChannels	= 4;
	irradianceMapParams.specialFormat		= Cubemap::SpecialFormat::HDR;
	irradianceMapParams.numMipmapsGenerated = 0;

	irradianceMap = createCubemap(gameObject->engine, irradianceMapParams);
}

inline void JFF::ReflectionProbeComponent::generatePreFilteredEnvironmentMap()
{
	auto io = gameObject->engine->io.lock();

	// Create a pre-filtered environment map from envMap
	std::shared_ptr<PreprocessPreFilteredEnvironmentMapGenerator> preFilteredGen =
		std::make_shared<PreprocessPreFilteredEnvironmentMapGenerator>(gameObject->engine, envMap, preFilteredMapFaceWidth, numPreFilteredMipmaps);
	preFilteredGen->execute();

	// Extract cubemap info
	std::string preFilteredAppendix = "_preFilteredEnvMap.hdr";

	Cubemap::ImageInfo envMapInfo = envMap->getImageInfo();
	std::string imgRight	= std::regex_replace(envMapInfo.imageRightFilename,  std::regex(R"raw(\..*)raw"), preFilteredAppendix);
	std::string imgLeft		= std::regex_replace(envMapInfo.imageLeftFilename,	 std::regex(R"raw(\..*)raw"), preFilteredAppendix);
	std::string imgTop		= std::regex_replace(envMapInfo.imageTopFilename,	 std::regex(R"raw(\..*)raw"), preFilteredAppendix);
	std::string imgBottom	= std::regex_replace(envMapInfo.imageBottomFilename, std::regex(R"raw(\..*)raw"), preFilteredAppendix);
	std::string imgBack		= std::regex_replace(envMapInfo.imageBackFilename,	 std::regex(R"raw(\..*)raw"), preFilteredAppendix);
	std::string imgFront	= std::regex_replace(envMapInfo.imageFrontFilename,  std::regex(R"raw(\..*)raw"), preFilteredAppendix);

	std::string generatedFolder = "Generated";

	imgRight	= generatedFolder + JFF_SLASH_STRING + imgRight;
	imgLeft		= generatedFolder + JFF_SLASH_STRING + imgLeft;
	imgTop		= generatedFolder + JFF_SLASH_STRING + imgTop;
	imgBottom	= generatedFolder + JFF_SLASH_STRING + imgBottom;
	imgBack		= generatedFolder + JFF_SLASH_STRING + imgBack;
	imgFront	= generatedFolder + JFF_SLASH_STRING + imgFront;

	// After generating the pre-filtered map, load all faces and create a cubemap
	Cubemap::Params preFilteredMapParams;

	preFilteredMapParams.shaderVariableName = ShaderCodeBuilder::PRE_FILTERED_MAP;

	preFilteredMapParams.folder		= generatedFolder;

	preFilteredMapParams.imgRight	= io->loadImage(imgRight.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	preFilteredMapParams.imgLeft	= io->loadImage(imgLeft.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	preFilteredMapParams.imgTop		= io->loadImage(imgTop.c_str(),		/* flipVertically */ false, /* HDRImage */ true);
	preFilteredMapParams.imgBottom	= io->loadImage(imgBottom.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	preFilteredMapParams.imgBack	= io->loadImage(imgBack.c_str(),	/* flipVertically */ false, /* HDRImage */ true);
	preFilteredMapParams.imgFront	= io->loadImage(imgFront.c_str(),	/* flipVertically */ false, /* HDRImage */ true);

	preFilteredMapParams.coordsWrapMode		 = { Cubemap::Wrap::CLAMP_TO_EDGE, Cubemap::Wrap::CLAMP_TO_EDGE, Cubemap::Wrap::CLAMP_TO_EDGE };
	preFilteredMapParams.filterMode			 = { Cubemap::MinificationFilter::LINEAR_LINEAR_MIP, Cubemap::MagnificationFilter::LINEAR }; // Important mipmap filtering here
	preFilteredMapParams.numColorChannels	 = 4;
	preFilteredMapParams.specialFormat		 = Cubemap::SpecialFormat::HDR;
	preFilteredMapParams.numMipmapsGenerated = numPreFilteredMipmaps;

	// Change filtering mode of there aren't mipmaps
	if (numPreFilteredMipmaps <= 0)
		preFilteredMapParams.filterMode.minFilter = Cubemap::MinificationFilter::LINEAR;

	preFilteredMap = createCubemap(gameObject->engine, preFilteredMapParams);
}

inline void JFF::ReflectionProbeComponent::generateBRDFIntegrationMap()
{
	auto io = gameObject->engine->io.lock();

	// Create a BRDF integration map
	std::shared_ptr<PreprocessBRDFIntegrationMapGenerator> BRDFIntegrationGen =
		std::make_shared<PreprocessBRDFIntegrationMapGenerator>(gameObject->engine, BRDFIntegrationMapWidth);
	BRDFIntegrationGen->execute();

	std::string imgPath = std::string("Generated") + JFF_SLASH_STRING + "BRDFIntegrationMap.hdr";

	// After generating the BRDF integration map, load it into a texture
	Texture::Params BRDFIntegrationMapParams;

	BRDFIntegrationMapParams.folder				= "";
	BRDFIntegrationMapParams.shaderVariableName = ShaderCodeBuilder::BRDF_INTEGRATION_MAP;
	BRDFIntegrationMapParams.img				= io->loadImage(imgPath.c_str(), /* flipVertically */ true, /* HDRImage */ true); // Important here to flip vertically
	BRDFIntegrationMapParams.coordsWrapMode		= { Texture::Wrap::CLAMP_TO_EDGE, Texture::Wrap::CLAMP_TO_EDGE, Texture::Wrap::CLAMP_TO_EDGE };
	BRDFIntegrationMapParams.filterMode			= { Texture::MinificationFilter::LINEAR, Texture::MagnificationFilter::LINEAR };
	BRDFIntegrationMapParams.numColorChannels	= 4;
	BRDFIntegrationMapParams.specialFormat		= Texture::SpecialFormat::HDR;

	BRDFIntegrationMap = createTexture(gameObject->engine, BRDFIntegrationMapParams);
}
