#include "ReflectionProbeComponent.h"

#include "Log.h"
#include "Engine.h"

extern std::shared_ptr<JFF::Cubemap> createCubemap(JFF::Engine* const engine, const char* name, const char* assetFilePath);

JFF::ReflectionProbeComponent::ReflectionProbeComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled,
	const char* cubemapAssetFilepath) :
	EnvironmentMapComponent(gameObject, name, initiallyEnabled),
	assetFilepath(cubemapAssetFilepath),
	envMap()
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
	envMap = createCubemap(gameObject->engine, "Reflection probe cubemap", assetFilepath.c_str());

	// Register the environment map in renderer
	gameObject->engine->renderer.lock()->addEnvironmentMap(this);
}

void JFF::ReflectionProbeComponent::onDestroy() noexcept
{
	// Unregister the environment map in Renderer
	gameObject->engine->renderer.lock()->removeEnvironmentMap(this);

	// Destroy the cubemap
	envMap->destroy();
}

void JFF::ReflectionProbeComponent::sendEnvironmentMap(RenderComponent* const renderComponent)
{
	// Send the cubemap to the shader for its use
	renderComponent->sendEnvironmentMap(envMap);
}
