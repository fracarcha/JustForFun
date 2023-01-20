/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "PreprocessBRDFIntegrationMapGenerator.h"

#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include <regex>

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);
extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(const JFF::Framebuffer::Params& params);
extern std::shared_ptr<JFF::MeshObject> createMeshObject(JFF::Engine* const engine, const JFF::MeshObject::BasicMesh& predefinedShape);

JFF::PreprocessBRDFIntegrationMapGenerator::PreprocessBRDFIntegrationMapGenerator(Engine* const engine, unsigned int textureWidth) :
	engine(engine),

	textureWidth(textureWidth),

	BRDFIntegrationMapGeneratorMaterial(),
	fbo(),
	mesh()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor PreprocessBRDFIntegrationMapGenerator")

	// ------------------------------ BUILD MATERIALS ------------------------------ //

	BRDFIntegrationMapGeneratorMaterial = createMaterial(engine, "BRDF integration map generator material");
	BRDFIntegrationMapGeneratorMaterial->setDomain(Material::MaterialDomain::BRDF_INTEGRATION_MAP_GENERATOR);
	BRDFIntegrationMapGeneratorMaterial->cook();

	// ------------------------------ BUILD FRAMEBUFFER PARAMS ------------------------------ //

	Framebuffer::AttachmentData textureData;
	textureData.width				= textureWidth;
	textureData.height				= textureWidth;
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

	// ------------------------------ CREATE PLANE MESH ------------------------------ //

	mesh = createMeshObject(engine, MeshObject::BasicMesh::PLANE);
	mesh->cook();
}

JFF::PreprocessBRDFIntegrationMapGenerator::~PreprocessBRDFIntegrationMapGenerator()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor PreprocessBRDFIntegrationMapGenerator")

	BRDFIntegrationMapGeneratorMaterial->destroy();
	fbo->destroy();
}

void JFF::PreprocessBRDFIntegrationMapGenerator::execute()
{
	auto renderer = engine->renderer.lock();

	// Adjust the viewport to the size of the texture
	renderer->setViewport(0, 0, textureWidth, textureWidth);

	// Enable the FBO as render target (also clear buffers)
	fbo->enable();

	// Use material
	BRDFIntegrationMapGeneratorMaterial->use();

	// Draw
	mesh->draw();

	// Write buffer to file
	fbo->writeToFile("BRDFIntegrationMap");

	// Restore viewport
	renderer->restoreViewport();
}
