/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "PreprocessPreFilteredEnvironmentMapGenerator.h"

#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include <regex>

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);
extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(const JFF::Framebuffer::Params& params);
extern std::shared_ptr<JFF::MeshObject> createMeshObject(JFF::Engine* const engine, const JFF::MeshObject::BasicMesh& predefinedShape);

JFF::PreprocessPreFilteredEnvironmentMapGenerator::PreprocessPreFilteredEnvironmentMapGenerator(
	Engine* const engine, 
	const std::weak_ptr<Cubemap>& environmentMap, 
	unsigned int cubemapWidth,
	unsigned int numMipmaps) :
	engine(engine),

	envMap(environmentMap),
	cubemapWidth(cubemapWidth),
	numMipmaps(numMipmaps),

	projectionMatrix(),
	viewMatrixRight(),
	viewMatrixLeft(),
	viewMatrixTop(),
	viewMatrixBottom(),
	viewMatrixFront(),
	viewMatrixBack(),

	preFilteredEnvironmentMapGeneratorMaterial(),
	fbo(),
	mesh()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor PreprocessPreFilteredEnvironmentMapGenerator")

	// ------------------------------ BUILD MATERIALS ------------------------------ //

	preFilteredEnvironmentMapGeneratorMaterial = createMaterial(engine, "Pre-filtered environment map generator material");
	preFilteredEnvironmentMapGeneratorMaterial->setDomain(Material::MaterialDomain::PRE_FILTERED_ENVIRONMENT_MAP_GENERATOR);
	preFilteredEnvironmentMapGeneratorMaterial->cook();

	// ------------------------------ BUILD FRAMEBUFFER PARAMS ------------------------------ //

	Framebuffer::AttachmentData textureData;
	textureData.width				= cubemapWidth;
	textureData.height				= cubemapWidth;
	textureData.renderBuffer		= false;
	textureData.texType				= Framebuffer::TextureType::TEXTURE_2D;
	textureData.wrapMode			= { Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE };
	textureData.filterMode			= { Framebuffer::MinificationFilter::NEAREST, Framebuffer::MagnificationFilter::NEAREST };
	textureData.HDR					= true;
	textureData.numColorChannels	= 4;
	textureData.mipmapLevel			= 0;

	Framebuffer::Params params;
	params.samplesPerPixel = 0u;
	params.attachments[Framebuffer::AttachmentPoint::COLOR_0] = textureData; // Final color channel

	// ------------------------------ BUILD FBOs ------------------------------ //

	fbo = createFramebuffer(params);

	// ------------------------------ CREATE CUBE MESH ------------------------------ //

	mesh = createMeshObject(engine, MeshObject::BasicMesh::CUBE);
	mesh->cook();

	// ------------------------------ CREATE MATRICES ------------------------------ //

	/*
	* NOTE: The UP vector (third parameter of lookAt()) may look strange since it doesn't look UP (0.0, 1.0, 0.0). This is due to the fact that
	* positive Z face is what you whould look if you were inside the cubemap, that is, the back face of the cube.
	* For more info, check Cubemap class
	*/

	auto math = engine->math.lock();
	Vec3 worldCenter; // Center of the world to lookAt inside a cubemap

	viewMatrixRight		= math->lookAt(worldCenter, Vec3::RIGHT,	Vec3::DOWN);
	viewMatrixLeft		= math->lookAt(worldCenter, Vec3::LEFT,		Vec3::DOWN);
	viewMatrixTop		= math->lookAt(worldCenter, Vec3::UP,		Vec3::BACKWARD);
	viewMatrixBottom	= math->lookAt(worldCenter, Vec3::DOWN,		Vec3::FORWARD);
	viewMatrixFront		= math->lookAt(worldCenter, Vec3::FORWARD,	Vec3::DOWN);
	viewMatrixBack		= math->lookAt(worldCenter, Vec3::BACKWARD, Vec3::DOWN);

	float fovyRad	= math->radians(90.0f); // 90 degrees takes exactly one face of the cubemap
	float aspect	= 1.0f; // Aspect ratio: cubemapWidth / cubemapWidth
	float zNear		= 0.1f;
	float zFar		= 1.0f;

	projectionMatrix = math->perspective(fovyRad, aspect, zNear, zFar);
}

JFF::PreprocessPreFilteredEnvironmentMapGenerator::~PreprocessPreFilteredEnvironmentMapGenerator()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor PreprocessPreFilteredEnvironmentMapGenerator")

	preFilteredEnvironmentMapGeneratorMaterial->destroy();
	fbo->destroy();
}

void JFF::PreprocessPreFilteredEnvironmentMapGenerator::execute()
{
	auto renderer = engine->renderer.lock();
	auto math = engine->math.lock();

	// Adjust the viewport to the size of one face of the cubemap
	renderer->setViewport(0, 0, cubemapWidth, cubemapWidth);

	// The order of layer:cubemap-face is: 0:right 1:left 2:top 3:bottom 4:back 5:front, so each layer must match its corresponding cubemap face
	Mat4 viewMatrices[] = { viewMatrixRight, viewMatrixLeft, viewMatrixTop, viewMatrixBottom, viewMatrixBack, viewMatrixFront };

	Cubemap::ImageInfo envMapInfo = envMap->getImageInfo();
	std::string cubemapFacePaths[] =
	{
		envMapInfo.imageRightFilename,
		envMapInfo.imageLeftFilename,
		envMapInfo.imageTopFilename,
		envMapInfo.imageBottomFilename,
		envMapInfo.imageBackFilename,
		envMapInfo.imageFrontFilename
	};

	std::string preFilteredAppendix = "_preFilteredEnvMap";
	std::string mipmapAppendix = "_mip";

	for (unsigned int mipmap = 0; mipmap < numMipmaps + 1; ++mipmap)
	{
		// Update the draw viewport and framebuffer size for mipmaps
		if (mipmap > 0)
		{
			unsigned int width = cubemapWidth / (unsigned int)math->pow(2.0f, (float) mipmap);
			renderer->setViewport(0, 0, width, width);
			fbo->setSize(width, width);
		}

		float roughness = (float)mipmap / (float)(std::max(numMipmaps, 1u));

		for (int i = 0; i < 6; ++i)
		{
			// Enable the FBO as render target (also clear buffers)
			fbo->enable();

			// Use material
			preFilteredEnvironmentMapGeneratorMaterial->use();

			// View projection
			preFilteredEnvironmentMapGeneratorMaterial->sendMat4(ShaderCodeBuilder::VIEW_MATRIX.c_str(), viewMatrices[i]);
			preFilteredEnvironmentMapGeneratorMaterial->sendMat4(ShaderCodeBuilder::PROJECTION_MATRIX.c_str(), projectionMatrix);

			// Send the environment map and other needed params
			preFilteredEnvironmentMapGeneratorMaterial->sendEnvironmentMap(envMap);
			preFilteredEnvironmentMapGeneratorMaterial->sendFloat(ShaderCodeBuilder::ENVIRONMENT_MAP_FACE_WIDTH.c_str(), (float)envMapInfo.width);
			preFilteredEnvironmentMapGeneratorMaterial->sendFloat(ShaderCodeBuilder::ROUGHNESS.c_str(), roughness);

			// Because we are rendering from inside the cube, ensure the correct face culling
			renderer->faceCulling(Renderer::FaceCullOp::CULL_FRONT_FACES);

			// Draw
			mesh->draw();

			// Restore face culling
			renderer->restoreFaceCulling();

			// Write to file
			std::string pathAppendix = preFilteredAppendix;
			if (mipmap > 0)
				pathAppendix += mipmapAppendix + std::to_string(mipmap);

			std::string path = std::regex_replace(cubemapFacePaths[i], std::regex(R"raw(\..*)raw"), pathAppendix);
			fbo->writeToFile(path.c_str());
		}
	}

	// Restore viewport
	renderer->restoreViewport();
}
