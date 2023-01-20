/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "PostProcessFXBloom.h"

#include "Engine.h"
#include "ShaderCodeBuilder.h"

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);
extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(const JFF::Framebuffer::Params& params);

JFF::PostProcessFXBloom::PostProcessFXBloom(Engine* const engine, int bufferWidth, int bufferHeight, float threshold, float intensity) :
	engine(engine),

	threshold(threshold),
	intensity(intensity),

	colorCopyMaterial(),
	highPassFilterMaterial(),
	gaussianBlurHorizontalMaterial(),
	gaussianBlurVerticalMaterial(),
	colorAdditionMaterial(),

	highPassFilterFBO(),
	bloomResultFBO(),

	bloomNumPasses(0u),
	gaussianBlurHorizontalFBOs(),
	gaussianBlurVerticalFBOs()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor PostProcessFXBloom")

	// ------------------------------ BUILD MATERIALS ------------------------------ //

	colorCopyMaterial = createMaterial(engine, "Color copy material");
	colorCopyMaterial->setDomain(Material::MaterialDomain::COLOR_COPY);
	colorCopyMaterial->cook();

	highPassFilterMaterial = createMaterial(engine, "High pass filter material");
	highPassFilterMaterial->setDomain(Material::MaterialDomain::HIGH_PASS_FILTER);
	highPassFilterMaterial->cook();

	gaussianBlurHorizontalMaterial = createMaterial(engine, "Gaussian blur horizontal material");
	gaussianBlurHorizontalMaterial->setDomain(Material::MaterialDomain::GAUSSIAN_BLUR_HORIZONTAL);
	gaussianBlurHorizontalMaterial->cook();

	gaussianBlurVerticalMaterial = createMaterial(engine, "Gaussian blur vertical material");
	gaussianBlurVerticalMaterial->setDomain(Material::MaterialDomain::GAUSSIAN_BLUR_VERTICAL);
	gaussianBlurVerticalMaterial->cook();

	colorAdditionMaterial = createMaterial(engine, "Color addition material");
	colorAdditionMaterial->setDomain(Material::MaterialDomain::COLOR_ADDITION);
	colorAdditionMaterial->cook();

	// ------------------------------ BUILD FRAMEBUFFER PARAMS ------------------------------ //

	// TODO: Modify createFramebuffer() to allow create one single texture and use all its mipmaps as attachment points

	Framebuffer::AttachmentData textureData;
	textureData.width = bufferWidth;
	textureData.height = bufferHeight;
	textureData.renderBuffer = false;
	textureData.texType = Framebuffer::TextureType::TEXTURE_2D;
	textureData.wrapMode = { Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE };
	// IMPORTANT: Textures that doesn't have mipmaps should never use mip filters. Otherwise, sampling to it will result in black screen
	// IMPORTANT: In order to sample to mipmaps in a texture, it's essential to set minification filter to one that uses mipmaps
	textureData.filterMode = { Framebuffer::MinificationFilter::LINEAR_NEAREST_MIP, Framebuffer::MagnificationFilter::NEAREST };
	textureData.HDR = true;
	textureData.numColorChannels = 4;
	textureData.mipmapLevel = 0;

	Framebuffer::Params params;
	params.samplesPerPixel = 0u;
	params.attachments[Framebuffer::AttachmentPoint::COLOR_0] = textureData; // Final color channel

	// ------------------------------ BUILD HIGH PASS FILTER AND COLOR RESULT FBO ------------------------------ //

	// Select mipmap level halving the size of original FBO size
	params.attachments[Framebuffer::AttachmentPoint::COLOR_0].mipmapLevel = 1;

	highPassFilterFBO = createFramebuffer(params);
	bloomResultFBO = createFramebuffer(params); // This has the same mipmap level than high pass filter

	// ------------------------------ BUILD GAUSSIAN BLUR FBOs ------------------------------ //

	bloomNumPasses = 6; // TODO: glTexStorage2D guarantees the number of mipmaps
	for (unsigned int i = 0; i < bloomNumPasses; ++i)
	{
		// Increase mipmap level progresively on each pass to get lower res framebuffers on each step
		params.attachments[Framebuffer::AttachmentPoint::COLOR_0].mipmapLevel++;

		// Horizontal blur FBOs
		auto horizontalFBO = createFramebuffer(params);
		gaussianBlurHorizontalFBOs.push_back(horizontalFBO);

		// Vertical blur FBO
		auto verticalFBO = createFramebuffer(params);
		gaussianBlurVerticalFBOs.push_back(verticalFBO);
	}
}

JFF::PostProcessFXBloom::~PostProcessFXBloom()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor PostProcessFXBloom")

	colorCopyMaterial->destroy();
	highPassFilterMaterial->destroy();
	gaussianBlurHorizontalMaterial->destroy();
	gaussianBlurVerticalMaterial->destroy();
	colorAdditionMaterial->destroy();

	highPassFilterFBO->destroy();
	bloomResultFBO->destroy();

	for (int i = 0; i < gaussianBlurHorizontalFBOs.size(); ++i)
	{
		gaussianBlurHorizontalFBOs[i]->destroy();
		gaussianBlurVerticalFBOs[i]->destroy();
	}
}

void JFF::PostProcessFXBloom::execute(
	const std::weak_ptr<Framebuffer>& ppFBO, 
	const std::weak_ptr<Framebuffer>& ppFBO2,
	const std::weak_ptr<MeshComponent>& planeMesh)
{
	auto renderer = engine->renderer.lock();
	auto mesh = planeMesh.lock();
	auto inputFBO = ppFBO.lock();

	unsigned int width = 0;
	unsigned int height = 0;
	int mipLevel = 0;

	// Execute a high pass filter because bloom affects fragments beyond a certain threshold
	highPassFilterFBO->enable();

	highPassFilterFBO->getSize(Framebuffer::AttachmentPoint::COLOR_0, width, height);
	renderer->setViewport(0, 0, width, height);

	highPassFilterMaterial->use();
	highPassFilterMaterial->sendPostProcessingTextures(ppFBO);
	highPassFilterMaterial->sendFloat(ShaderCodeBuilder::MIPMAP_LEVEL.c_str(), (float)mipLevel);
	highPassFilterMaterial->sendFloat(ShaderCodeBuilder::THRESHOLD.c_str(), threshold);
	mesh->draw();

	mipLevel++;

	// Execute blur (horizontally and vertically) on downsampled versions of filtered FBO
	for (unsigned int i = 0; i < bloomNumPasses; ++i)
	{
		// Render horizontal gaussian blur
		gaussianBlurHorizontalFBOs[i]->enable();

		gaussianBlurHorizontalFBOs[i]->getSize(Framebuffer::AttachmentPoint::COLOR_0, width, height);
		renderer->setViewport(0, 0, width, height);

		gaussianBlurHorizontalMaterial->use();
		gaussianBlurHorizontalMaterial->sendPostProcessingTextures(
			i == 0 ? highPassFilterFBO : gaussianBlurVerticalFBOs[i - 1]);
		gaussianBlurHorizontalMaterial->sendFloat(ShaderCodeBuilder::MIPMAP_LEVEL.c_str(), (float)mipLevel);
		mesh->draw();

		mipLevel++;

		// Render vertical gaussian blur
		gaussianBlurVerticalFBOs[i]->enable();

		gaussianBlurVerticalFBOs[i]->getSize(Framebuffer::AttachmentPoint::COLOR_0, width, height);
		renderer->setViewport(0, 0, width, height);

		gaussianBlurVerticalMaterial->use();
		gaussianBlurVerticalMaterial->sendPostProcessingTextures(gaussianBlurHorizontalFBOs[i]);
		gaussianBlurVerticalMaterial->sendFloat(ShaderCodeBuilder::MIPMAP_LEVEL.c_str(), (float)mipLevel);
		mesh->draw();
	}

	// Rejoin blurred buffers from low to high resolution. Recycle gaussianBlurHorizontalFBOs to store results
	colorAdditionMaterial->use(); // Use this material for next passes

	std::shared_ptr<Framebuffer> currentLowerResBlurredFBO = gaussianBlurVerticalFBOs[gaussianBlurVerticalFBOs.size() - 1];
	std::shared_ptr<Framebuffer> currentHigherResBlurredFBO = gaussianBlurVerticalFBOs[gaussianBlurVerticalFBOs.size() - 2];
	std::shared_ptr<Framebuffer> currentResultFBO = gaussianBlurHorizontalFBOs[gaussianBlurHorizontalFBOs.size() - 2];

	for (int i = bloomNumPasses - 2; i >= 0; --i)
	{
		// Combine two framebuffers into a third one using color addition material
		currentResultFBO->enable();

		currentResultFBO->getSize(Framebuffer::AttachmentPoint::COLOR_0, width, height);
		renderer->setViewport(0, 0, width, height);

		colorAdditionMaterial->sendPostProcessingTextures(currentHigherResBlurredFBO, currentLowerResBlurredFBO);
		colorAdditionMaterial->sendVec2(ShaderCodeBuilder::MIPMAP_LEVELS.c_str(), Vec2((float)(mipLevel - 1), (float)mipLevel));
		mesh->draw();

		mipLevel--;

		// Select actors of next operation
		if (i > 0)
		{
			currentLowerResBlurredFBO = currentResultFBO;
			currentHigherResBlurredFBO = gaussianBlurVerticalFBOs[(size_t)i - 1];
			currentResultFBO = gaussianBlurHorizontalFBOs[(size_t)i - 1];
		}
	}

	// Combine the last blur combination result with high pass filter result
	bloomResultFBO->enable();

	bloomResultFBO->getSize(Framebuffer::AttachmentPoint::COLOR_0, width, height);
	renderer->setViewport(0, 0, width, height);

	colorAdditionMaterial->sendPostProcessingTextures(highPassFilterFBO, currentResultFBO);
	colorAdditionMaterial->sendVec2(ShaderCodeBuilder::MIPMAP_LEVELS.c_str(), Vec2((float)(mipLevel - 1), (float)mipLevel));
	mesh->draw();

	mipLevel--;

	// Combine bloom result with incoming framebuffer color
	inputFBO->enable(/* clearBuffers = */ false);
	colorCopyMaterial->use();

	renderer->restoreViewport(); // Restore the original viewport size

	colorCopyMaterial->sendPostProcessingTextures(bloomResultFBO);
	colorCopyMaterial->sendFloat(ShaderCodeBuilder::MIPMAP_LEVEL.c_str(), (float)mipLevel);
	colorCopyMaterial->sendFloat(ShaderCodeBuilder::INTENSITY.c_str(), intensity);

	renderer->disableDepthTest();
	renderer->enableBlending(Renderer::BlendOp::ADDITIVE);
	mesh->draw();
	renderer->disableBlending();
	renderer->restoreDepthTest();

	// An explicit call to disable FBO is important here because ppFBO could be a multisample buffer and it must 'resolve' 
	// to an auxiliary FBO (Check Framebuffer class)
	inputFBO->disable();
}

void JFF::PostProcessFXBloom::updateFramebufferSize(int width, int height)
{
	highPassFilterFBO->setSize(width, height);
	bloomResultFBO->setSize(width, height);

	for (int i = 0; i < gaussianBlurHorizontalFBOs.size(); ++i)
	{
		gaussianBlurHorizontalFBOs[i]->setSize(width, height);
		gaussianBlurVerticalFBOs[i]->setSize(width, height);
	}
}
