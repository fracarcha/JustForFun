/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "PreprocessEquirectangularToCubemap.h"

#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include <regex>

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);
extern std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const JFF::Texture::Params& params);
extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(const JFF::Framebuffer::Params& params);
extern std::shared_ptr<JFF::MeshObject> createMeshObject(JFF::Engine* const engine, const JFF::MeshObject::BasicMesh& predefinedShape);

JFF::PreprocessEquirectangularToCubemap::PreprocessEquirectangularToCubemap(
	Engine* const engine, 
	const std::weak_ptr<Image>& img, 
	unsigned int cubemapWidth) :
	engine(engine),

	imageFilePath(),
	cubemapWidth(cubemapWidth),

	projectionMatrix(),
	viewMatrixRight(),
	viewMatrixLeft(),
	viewMatrixTop(),
	viewMatrixBottom(),
	viewMatrixFront(),
	viewMatrixBack(),

	equirectangularToCubemapMaterial(),
	fbo(),
	mesh()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor PreprocessEquirectangularToCubemap")

	// ------------------------------ BUILD A TEXTURE FROM IMAGE ------------------------------ //

	auto image = img.lock();
	const Image::Data& imageData = image->data();

	imageFilePath = imageData.filename;

	Texture::Params texParams;

	texParams.folder			 = "";
	texParams.shaderVariableName = ShaderCodeBuilder::EQUIRECTANGULAR_TEX;
	texParams.img				 = image;
	texParams.coordsWrapMode	 = { Texture::Wrap::CLAMP_TO_EDGE, Texture::Wrap::CLAMP_TO_EDGE , Texture::Wrap::CLAMP_TO_EDGE };
	texParams.filterMode		 = { Texture::MinificationFilter::NEAREST,Texture::MagnificationFilter::NEAREST };
	texParams.numColorChannels	 = 4; // These are the channels of the texture, not the internal image
	texParams.specialFormat		 = imageData.imgChannelType == Image::ImageChannelType::UNSIGNED_BYTE ? Texture::SpecialFormat::NONE : Texture::SpecialFormat::HDR;

	std::shared_ptr<JFF::Texture> equirectangularTex = createTexture(engine, texParams);

	// ------------------------------ BUILD MATERIALS ------------------------------ //

	equirectangularToCubemapMaterial = createMaterial(engine, "Equirectangular to cubemap material");
	equirectangularToCubemapMaterial->setDomain(Material::MaterialDomain::EQUIRECTANGULAR_TO_CUBEMAP);
	equirectangularToCubemapMaterial->addTexture(equirectangularTex);
	equirectangularToCubemapMaterial->cook();

	// ------------------------------ BUILD FRAMEBUFFER PARAMS ------------------------------ //

	Framebuffer::AttachmentData textureData;
	textureData.width				= cubemapWidth;
	textureData.height				= cubemapWidth;
	textureData.renderBuffer		= false;
	textureData.texType				= Framebuffer::TextureType::TEXTURE_2D;
	textureData.wrapMode			= { Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE };
	textureData.filterMode			= { Framebuffer::MinificationFilter::NEAREST, Framebuffer::MagnificationFilter::NEAREST };
	textureData.HDR					= imageData.imgChannelType == Image::ImageChannelType::UNSIGNED_BYTE ? false : true;
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

	viewMatrixRight		= math->lookAt(worldCenter, Vec3::RIGHT, Vec3::DOWN);
	viewMatrixLeft		= math->lookAt(worldCenter, Vec3::LEFT, Vec3::DOWN);
	viewMatrixTop		= math->lookAt(worldCenter, Vec3::UP, Vec3::BACKWARD);
	viewMatrixBottom	= math->lookAt(worldCenter, Vec3::DOWN, Vec3::FORWARD);
	viewMatrixFront		= math->lookAt(worldCenter, Vec3::FORWARD, Vec3::DOWN);
	viewMatrixBack		= math->lookAt(worldCenter, Vec3::BACKWARD, Vec3::DOWN);
	
	float fovyRad	= math->radians(90.0f); // 90 degrees takes exactly one face of the cubemap
	float aspect	= 1.0f; // Aspect ratio: cubemapWidth / cubemapWidth
	float zNear		= 0.1f;
	float zFar		= 1.0f;

	projectionMatrix = math->perspective(fovyRad, aspect, zNear, zFar);
}

JFF::PreprocessEquirectangularToCubemap::~PreprocessEquirectangularToCubemap()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor PreprocessEquirectangularToCubemap")

	equirectangularToCubemapMaterial->destroy();
	fbo->destroy();
}

void JFF::PreprocessEquirectangularToCubemap::execute()
{
	auto renderer = engine->renderer.lock();

	// Adjust the viewport to the size of one face of the cubemap
	renderer->setViewport(0, 0, cubemapWidth, cubemapWidth);

	// The order of layer:cubemap-face is: 0:right 1:left 2:top 3:bottom 4:back 5:front, so each layer must match its corresponding cubemap face
	Mat4 viewMatrices[] = { viewMatrixRight, viewMatrixLeft, viewMatrixTop, viewMatrixBottom, viewMatrixBack, viewMatrixFront };
	std::string pathAppendix[] = { "_posx", "_negx", "_posy", "_negy", "_posz", "_negz" };

	for (int i = 0; i < 6; ++i)
	{
		// Enable the FBO as render target (also clear buffers)
		fbo->enable();

		// Use material
		equirectangularToCubemapMaterial->use();

		// View projection
		equirectangularToCubemapMaterial->sendMat4(ShaderCodeBuilder::VIEW_MATRIX.c_str(), viewMatrices[i]);
		equirectangularToCubemapMaterial->sendMat4(ShaderCodeBuilder::PROJECTION_MATRIX.c_str(), projectionMatrix);

		// Because we are rendering from inside the cube, ensure the correct face culling
		renderer->faceCulling(Renderer::FaceCullOp::CULL_FRONT_FACES);

		// Draw
		mesh->draw();

		// Restore face culling
		renderer->restoreFaceCulling();

		// Write to file
		std::string path = std::regex_replace(imageFilePath, std::regex(R"raw(\..*)raw"), pathAppendix[i]);
		fbo->writeToFile(path.c_str());
	}
	
	// Restore viewport
	renderer->restoreViewport();
}
