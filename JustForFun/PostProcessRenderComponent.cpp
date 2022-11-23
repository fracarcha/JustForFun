#include "PostProcessRenderComponent.h"

#include "Engine.h"

// FX
#include "PostProcessFXBloom.h"


extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name, const char* assetFilePath);

extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(JFF::Framebuffer::PrefabFramebuffer fboType,
	unsigned int width, unsigned int height, unsigned int samplesPerPixel = 0);

JFF::PostProcessRenderComponent::PostProcessRenderComponent(
	GameObject* const gameObject, 
	const char* name, 
	bool initiallyEnabled, 
	const char* materialAssetFilepath) :
	RenderComponent(gameObject, name, initiallyEnabled),

	materialAssetFilepath(materialAssetFilepath),
	FBOSizeCallbackHandler(0ull),

	mesh(),

	material(),
	fbo(),

	fx()
{
	JFF_LOG_INFO("Ctor PostProcessRenderComponent")
}

JFF::PostProcessRenderComponent::PostProcessRenderComponent(
	GameObject* const gameObject, 
	const char* name, 
	bool initiallyEnabled, 
	const std::shared_ptr<Material>& material) :
	RenderComponent(gameObject, name, initiallyEnabled),

	materialAssetFilepath(),
	FBOSizeCallbackHandler(0ull),
	
	mesh(),

	material(material),
	fbo(),

	fx()
{
	JFF_LOG_INFO("Ctor PostProcessRenderComponent")
}

JFF::PostProcessRenderComponent::~PostProcessRenderComponent()
{
	JFF_LOG_INFO("Dtor PostProcessRenderComponent")
}

void JFF::PostProcessRenderComponent::onStart()
{
	// Load material from file if it's null
	if (!material)
		material = createMaterial(gameObject->engine, materialAssetFilepath.c_str(), materialAssetFilepath.c_str());

	// Find a MeshComponent on this gameObject
	mesh = gameObject->getComponent<MeshComponent>();
	if (mesh.expired()) // If this GameObject doesn't contain a MeshComponent or there is another error
	{
		JFF_LOG_ERROR("A MeshRenderComponent needs a MeshComponent attached to the gameObject in order to work")
			return;
	}

	// Build a post processing FBO
	auto context = gameObject->engine->context.lock();
	int fboWidth, fboHeight;
	context->getFramebufferSizeInPixels(fboWidth, fboHeight);
	fbo = createFramebuffer(Framebuffer::PrefabFramebuffer::FBO_POST_PROCESS, fboWidth, fboHeight);

	// Build custom FXs
	buildCustomFX(fboWidth, fboHeight);

	// Ensure its width and height adapts to window size changes
	FBOSizeCallbackHandler = context->addOnFramebufferSizeChangedListener([this](int width, int height)
		{
			fbo->setSize(width, height);

			// Update custom FX buffers size
			std::for_each(fx.begin(), fx.end(), [width, height](auto& ppFX) { ppFX->updateFramebufferSize(width, height); });
		});

	// Send this RenderComponent to Renderer
	gameObject->engine->renderer.lock()->addRenderable(this);
}

void JFF::PostProcessRenderComponent::onDestroy() noexcept
{
	// Destroy the associated material
	material->destroy();

	// Unregister from Context's framebuffer change callback
	gameObject->engine->context.lock()->removeOnFramebufferSizeChangedListener(FBOSizeCallbackHandler);

	// Destroy the post processing FBO
	fbo->destroy();

	// Destroy custom FXs
	std::for_each(fx.begin(), fx.end(), [](auto& ppFX) { ppFX.reset(); });

	// Remove this RenderComponent from Renderer
	gameObject->engine->renderer.lock()->removeRenderable(this);
}

JFF::Material::MaterialDomain JFF::PostProcessRenderComponent::getMaterialDomain() const
{
	return material->getDomain();
}

JFF::Material::LightModel JFF::PostProcessRenderComponent::getLightModel() const
{
	return material->getLightModel();
}

JFF::Material::Side JFF::PostProcessRenderComponent::getMaterialSide() const
{
	return material->getSide();
}

JFF::Material::DebugDisplay JFF::PostProcessRenderComponent::getDebugDisplay() const
{
	return material->getDebugDisplay();
}

void JFF::PostProcessRenderComponent::useMaterial()
{
	material->use();
}

void JFF::PostProcessRenderComponent::sendMat4(const char* variableName, const Mat4& matrix)
{
	material->sendMat4(variableName, matrix);
}

void JFF::PostProcessRenderComponent::sendMat3(const char* variableName, const Mat3& matrix)
{
	material->sendMat3(variableName, matrix);
}

void JFF::PostProcessRenderComponent::sendVec3(const char* variableName, const Vec3& vec)
{
	material->sendVec3(variableName, vec);
}

void JFF::PostProcessRenderComponent::sendVec4(const char* variableName, const Vec4& vec)
{
	material->sendVec4(variableName, vec);
}

void JFF::PostProcessRenderComponent::sendFloat(const char* variableName, float f)
{
	material->sendFloat(variableName, f);
}

void JFF::PostProcessRenderComponent::sendEnvironmentMap(const std::shared_ptr<Cubemap>& envMap)
{
	material->sendEnvironmentMap(envMap);
}

void JFF::PostProcessRenderComponent::sendDirLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO)
{
	material->sendDirLightShadowMap(index, shadowMapFBO);
}

void JFF::PostProcessRenderComponent::sendPointLightShadowCubemap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowCubemapFBO)
{
	material->sendPointLightShadowCubemap(index, shadowCubemapFBO);
}

void JFF::PostProcessRenderComponent::sendSpotLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO)
{
	material->sendSpotLightShadowMap(index, shadowMapFBO);
}

void JFF::PostProcessRenderComponent::sendPostProcessingTextures(const std::weak_ptr<Framebuffer>& ppFBO)
{
	material->sendPostProcessingTextures(ppFBO);
}

void JFF::PostProcessRenderComponent::draw()
{
	mesh.lock()->draw();
}


void JFF::PostProcessRenderComponent::executeCustomRenderPass(const std::weak_ptr<Framebuffer>& ppFBO)
{
	std::for_each(fx.begin(), fx.end(), [this, &ppFBO](auto& ppFX) { ppFX->execute(ppFBO, mesh); });
}

void JFF::PostProcessRenderComponent::enablePostProcessFramebuffer()
{
	fbo->enable();
}

void JFF::PostProcessRenderComponent::disablePostProcessFramebuffer()
{
	fbo->disable();
}

std::weak_ptr<JFF::Framebuffer> JFF::PostProcessRenderComponent::getFramebuffer() const
{
	return fbo;
}


inline void JFF::PostProcessRenderComponent::buildCustomFX(int bufferWidth, int bufferHeight)
{
	Material::PostProcessParams postProcessParams = material->getPostProcessParams();

	if (postProcessParams.bloomEnabled)
	{
		auto bloomFX = std::make_shared<PostProcessFXBloom>(gameObject->engine, bufferWidth, bufferHeight,
			postProcessParams.bloomThreshold, postProcessParams.bloomIntensity);

		fx.push_back(bloomFX);
	}

	// TODO: More FX here
}


