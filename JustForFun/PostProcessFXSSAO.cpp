/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "PostProcessFXSSAO.h"

#include "Engine.h"
#include "ShaderCodeBuilder.h"
#include "Texture.h"

#include <algorithm>
#include <sstream>
#include <random>

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);
extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(const JFF::Framebuffer::Params& params);
extern std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const JFF::Texture::Params& params);

JFF::PostProcessFXSSAO::PostProcessFXSSAO(
	Engine* const engine,
	int bufferWidth, int bufferHeight, 
	unsigned int numSamples, float sampleHemisphereRadius, 
	unsigned int numBlurSteps, float intensity) :
	engine(engine),

	numHemisphereSamples(numSamples),
	sampleHemisphereRadius(sampleHemisphereRadius),
	numBlurSteps(numBlurSteps),
	intensity(intensity),

	SSAOMaterial(),
	gaussianBlurHorizontalMaterial(),
	gaussianBlurVerticalMaterial(),
	colorCopyMaterial(),

	SSAO_FBO(),
	gaussianBlurHorizontalFBO(),
	gaussianBlurVerticalFBO()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor PostProcessFXSSAO")

	// Ensure that the number of hemisphere samples never exceed MAX_NUM_SAMPLES in shader
	if (numHemisphereSamples > PostProcessFXSSAO::MAX_NUM_SAMPLES)
	{
		JFF_LOG_WARNING("Cannot exceed the maximum number of samples. The maximum number of samples will be used: " << PostProcessFXSSAO::MAX_NUM_SAMPLES)
		numHemisphereSamples = PostProcessFXSSAO::MAX_NUM_SAMPLES;
	}

	// ------------ BUILD RANDOM TANGENTS NOISE TEXTURE AND HEMISPHERE OF SAMPLES ------------  //

	std::shared_ptr<Texture> randomTangentsTex = generateRandomTangentsTexture();
	generateHemisphereSamples();

	// ------------------------------ BUILD MATERIALS ------------------------------ //

	SSAOMaterial = createMaterial(engine, "SSAO material");
	SSAOMaterial->setDomain(Material::MaterialDomain::SSAO);
	SSAOMaterial->addTexture(randomTangentsTex);
	SSAOMaterial->cook();

	gaussianBlurHorizontalMaterial = createMaterial(engine, "Gaussian blur horizontal material");
	gaussianBlurHorizontalMaterial->setDomain(Material::MaterialDomain::GAUSSIAN_BLUR_HORIZONTAL);
	gaussianBlurHorizontalMaterial->cook();

	gaussianBlurVerticalMaterial = createMaterial(engine, "Gaussian blur vertical material");
	gaussianBlurVerticalMaterial->setDomain(Material::MaterialDomain::GAUSSIAN_BLUR_VERTICAL);
	gaussianBlurVerticalMaterial->cook();

	colorCopyMaterial = createMaterial(engine, "Color copy material");
	colorCopyMaterial->setDomain(Material::MaterialDomain::COLOR_COPY);
	colorCopyMaterial->cook();

	// ------------------------------ BUILD FRAMEBUFFER PARAMS ------------------------------ //

	Framebuffer::AttachmentData textureData;
	textureData.width				= bufferWidth;
	textureData.height				= bufferHeight;
	textureData.renderBuffer		= false;
	textureData.texType				= Framebuffer::TextureType::TEXTURE_2D;
	textureData.wrapMode			= { Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE, Framebuffer::Wrap::CLAMP_TO_EDGE };
	textureData.filterMode			= { Framebuffer::MinificationFilter::NEAREST, Framebuffer::MagnificationFilter::NEAREST };
	textureData.HDR					= false;
	textureData.numColorChannels	= 4; // TODO: In SSAO, this could be 1, but the output in shaders should be changed from vec4 to float
	textureData.mipmapLevel			= 0;

	Framebuffer::Params params;
	params.samplesPerPixel = 0u;
	params.attachments[Framebuffer::AttachmentPoint::COLOR_0] = textureData; // Final color channel

	// ------------------------------ BUILD FBOs ------------------------------ //

	SSAO_FBO					= createFramebuffer(params);
	gaussianBlurHorizontalFBO	= createFramebuffer(params);
	gaussianBlurVerticalFBO		= createFramebuffer(params);
}

JFF::PostProcessFXSSAO::~PostProcessFXSSAO()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor PostProcessFXSSAO")

	SSAOMaterial->destroy();
	gaussianBlurHorizontalMaterial->destroy();
	gaussianBlurVerticalMaterial->destroy();
	colorCopyMaterial->destroy();

	SSAO_FBO->destroy();
	gaussianBlurHorizontalFBO->destroy();
	gaussianBlurVerticalFBO->destroy();
}

void JFF::PostProcessFXSSAO::execute(
	const std::weak_ptr<Framebuffer>& ppFBO,
	const std::weak_ptr<Framebuffer>& ppFBO2, 
	const std::weak_ptr<MeshComponent>& planeMesh)
{
	auto renderer = engine->renderer.lock();
	auto mesh = planeMesh.lock();
	auto inputFBO = ppFBO.lock();

	// Execute SSAO draw call
	SSAO_FBO->enable();
	SSAOMaterial->use();
	SSAOMaterial->sendPostProcessingTextures(ppFBO, ppFBO2);
	SSAOMaterial->sendFloat(ShaderCodeBuilder::INTENSITY.c_str(), intensity);
	SSAOMaterial->sendFloat(ShaderCodeBuilder::HEMISPHERE_RADIUS.c_str(), sampleHemisphereRadius);
	SSAOMaterial->sendInt(ShaderCodeBuilder::NUM_HEMISPHERE_SAMPLES.c_str(), numHemisphereSamples);
	sendHemisphereSamples();
	mesh->draw();

	// Blur the result of SSAO
	for (unsigned int i = 0; i < numBlurSteps; ++i)
	{
		// Horizontal gaussian blur
		gaussianBlurHorizontalFBO->enable();
		gaussianBlurHorizontalMaterial->use();
		gaussianBlurHorizontalMaterial->sendPostProcessingTextures(i == 0 ? SSAO_FBO : gaussianBlurVerticalFBO);
		gaussianBlurHorizontalMaterial->sendFloat(ShaderCodeBuilder::MIPMAP_LEVEL.c_str(), (float)0);
		mesh->draw();

		// Vertical gaussian blur
		gaussianBlurVerticalFBO->enable();
		gaussianBlurVerticalMaterial->use();
		gaussianBlurVerticalMaterial->sendPostProcessingTextures(gaussianBlurHorizontalFBO);
		gaussianBlurVerticalMaterial->sendFloat(ShaderCodeBuilder::MIPMAP_LEVEL.c_str(), (float)0);
		mesh->draw();
	}

	// Combine SSAO result with incoming framebuffer color
	inputFBO->enable(/* clearBuffers = */ false);
	colorCopyMaterial->use();

	colorCopyMaterial->sendPostProcessingTextures(gaussianBlurVerticalFBO); // Vertical FBO has the SSAO result
	colorCopyMaterial->sendFloat(ShaderCodeBuilder::MIPMAP_LEVEL.c_str(), 0.0f);
	colorCopyMaterial->sendFloat(ShaderCodeBuilder::INTENSITY.c_str(), 1.0f);

	renderer->disableDepthTest();
	renderer->enableBlending(Renderer::BlendOp::MULTIPLY);
	mesh->draw();
	renderer->disableBlending();
	renderer->restoreDepthTest();

	// An explicit call to disable FBO is important here because ppFBO could be a multisample buffer and it must 'resolve' 
	// to an auxiliary FBO (Check Framebuffer class)
	inputFBO->disable();
}

void JFF::PostProcessFXSSAO::updateFramebufferSize(int width, int height)
{
	SSAO_FBO->setSize(width, height);
	gaussianBlurHorizontalFBO->setSize(width, height);
	gaussianBlurVerticalFBO->setSize(width, height);
}

inline std::shared_ptr<JFF::Texture> JFF::PostProcessFXSSAO::generateRandomTangentsTexture() const
{
	// ------------------------- IMAGE CREATION ------------------------- //

	std::string imgName		= "SSAORandomTangentsImage";
	int width				= 4;
	int height				= 4;
	int numChannelsPerPixel	= 3;
	std::vector<float> randomTangentsImgRaw;

	std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f); // Generates a random float in range [0,1]
	std::default_random_engine generator; // This is the algorithm used to generate random numbers. The defualt one is implementation-defined

	// Generate random vectors (tangents in tangent space) and store them in an image
	for (int i = 0; i < width * height; ++i)
	{
		randomTangentsImgRaw.push_back(randomFloat(generator) * 2.0f - 1.0f); // x
		randomTangentsImgRaw.push_back(randomFloat(generator) * 2.0f - 1.0f); // y
		randomTangentsImgRaw.push_back(0.0f); // z component of a tangent in tangent space is always zero
	}

	// Create the image holding the random tangents
	auto io = engine->io.lock();
	std::shared_ptr<JFF::Image> randomTangentsImg = io->loadImage(imgName.c_str(), width, height, numChannelsPerPixel, randomTangentsImgRaw);

	// ------------------------- TEXTURE CREATION ------------------------- //

	// Create a texture using previous image
	Texture::Params texParams;

	texParams.folder = "";
	texParams.shaderVariableName	= ShaderCodeBuilder::RANDOM_ROTATED_TANGENTS;
	texParams.img					= randomTangentsImg;
	texParams.coordsWrapMode		= { Texture::Wrap::REPEAT, Texture::Wrap::REPEAT , Texture::Wrap::REPEAT }; // Important to be REPEAT
	texParams.filterMode			= { Texture::MinificationFilter::NEAREST,Texture::MagnificationFilter::NEAREST };
	texParams.numColorChannels		= 4; // These are the channels of the texture, not the internal image
	texParams.specialFormat			= Texture::SpecialFormat::HDR; // We need GL_RGBA16F channel to be coherent to floating point image generated and to have an unbound vector precision 

	std::shared_ptr<JFF::Texture> randomTangentsTexture = createTexture(engine, texParams);

	return randomTangentsTexture;
}

inline void JFF::PostProcessFXSSAO::generateHemisphereSamples()
{
	// NOTE: Samples are generated in tangent space. In shader, this sample is multiplied by TBN matrix to transform it to world space

	auto math = engine->math.lock();

	std::uniform_real_distribution<float> randomFloat(0.0f, 1.0f); // Generates a random float in range [0,1]
	std::default_random_engine generator; // This is the algorithm used to generate random numbers. The defualt one is implementation-defined

	for (unsigned int i = 0; i < numHemisphereSamples; ++i)
	{
		// NOTE: The z component's range is [0,1] to create an hemisphere in tangent space. Otherwise, it would be a complete sphere
		Vec3 sample(
			randomFloat(generator) * 2.0f - 1.0f,
			randomFloat(generator) * 2.0f - 1.0f,
			randomFloat(generator)
		);

		// Normalize it to keep the sample inside the hemisphere
		sample = math->normalize(sample);

		// Previous normalize() call put all samples on the hemisphere surface. Next line re-randomize the distance to the center
		sample *= randomFloat(generator);

		/* 
		* Currently, all samples are randomly distributed in the sample kernel, but we’d rather place a
		* larger weight on occlusions close to the actual fragment. We want to distribute more kernel samples
		* closer to the origin. We can do this with an accelerating interpolation function:
		*/
		float scale = (float)i / numHemisphereSamples;
		scale = math->lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		// Add it to sample list
		hemisphereSamplesTangentSpace.push_back(sample);
	}
}

inline void JFF::PostProcessFXSSAO::sendHemisphereSamples()
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	for (unsigned int i = 0; i < numHemisphereSamples; ++i)
	{
		ss.str(emptyString);
		ss << ShaderCodeBuilder::HEMISPHERE_SAMPLES << "[" << i << "]";
		SSAOMaterial->sendVec3(ss.str().c_str(), hemisphereSamplesTangentSpace[i]);
	}
}
