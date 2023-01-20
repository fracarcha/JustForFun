/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassSpotLightingDeferred.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassSpotLightingDeferred::RenderPassSpotLightingDeferred(Engine* const engine) :
	engine(engine),

	renderable(nullptr),
	spotLights()
{
	JFF_LOG_INFO("Ctor RenderPassSpotLightingDeferred")
}

JFF::RenderPassSpotLightingDeferred::~RenderPassSpotLightingDeferred()
{
	JFF_LOG_INFO("Dtor RenderPassSpotLightingDeferred")
}

void JFF::RenderPassSpotLightingDeferred::execute()
{	
	// Return if post process renderable is no present
	if (!renderable)
		return;

	// If this component is not enabled, skip its rendering
	if (!renderable->isEnabled())
		return;

	// ------------ Draw each light contribution in a draw call ------------ //

	auto renderer = engine->renderer.lock();

	// Enable component material and bind all internal textures
	renderable->useMaterial();

	// Use geometry fbo's textures
	renderable->sendPostProcessingTextures(renderer->getGeometryFramebuffer());

	// Draw directional light contribution in a separate draw call
	for (SpotLightComponent* light : spotLights)
	{
		if (!light->isEnabled())
			continue;

		light->sendLightParams(renderable);
		renderable->draw();
	}
}

void JFF::RenderPassSpotLightingDeferred::addRenderable(RenderComponent* renderable)
{
	if (this->renderable)
	{
		JFF_LOG_WARNING("Cannot add more than one RenderComponent. Operation aborted")
			return;
	}

	if (renderable->getMaterialDomain() != Material::MaterialDomain::SPOT_LIGHTING_DEFERRED)
	{
		JFF_LOG_WARNING("Only renderables with material domain SPOT_LIGHTING_DEFERRED is allowed. Operation aborted")
			return;
	}

	this->renderable = renderable;
}

void JFF::RenderPassSpotLightingDeferred::removeRenderable(RenderComponent* renderable)
{
	if (!this->renderable || this->renderable != renderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent because it's not present. Operation aborted")
			return;
	}

	this->renderable = nullptr;
}

void JFF::RenderPassSpotLightingDeferred::addLight(LightComponent* const light)
{
	// Check the concrete type of light
	if (SpotLightComponent* spotLight = dynamic_cast<SpotLightComponent*>(light))
	{
		spotLights.push_back(spotLight);
	}
}

void JFF::RenderPassSpotLightingDeferred::removeLight(LightComponent* const light)
{
	// Check the concrete type of light
	if (SpotLightComponent* spotLight = dynamic_cast<SpotLightComponent*>(light))
	{
		// NOTE: This will remove ALL lights that points the same memory. Do no share LightComponent between GameObjects
		auto iter = std::remove(spotLights.begin(), spotLights.end(), spotLight);
		spotLights.erase(iter, spotLights.end());
	}
}

void JFF::RenderPassSpotLightingDeferred::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Adding environment map to deferred spot light pass is invalid")
}

void JFF::RenderPassSpotLightingDeferred::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Removing environment map from deferred spot light pass is invalid")
}
