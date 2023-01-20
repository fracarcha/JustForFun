/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassDirectionalLightingDeferred.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassDirectionalLightingDeferred::RenderPassDirectionalLightingDeferred(Engine* const engine) :
	engine(engine),

	renderable(nullptr),
	directionalLights()
{
	JFF_LOG_INFO("Ctor RenderPassDirectionalLightingDeferred")
}

JFF::RenderPassDirectionalLightingDeferred::~RenderPassDirectionalLightingDeferred()
{
	JFF_LOG_INFO("Dtor RenderPassDirectionalLightingDeferred")
}

void JFF::RenderPassDirectionalLightingDeferred::execute()
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
	for (DirectionalLightComponent* light : directionalLights)
	{
		if (!light->isEnabled())
			continue;

		light->sendLightParams(renderable);
		renderable->draw();
	}
}

void JFF::RenderPassDirectionalLightingDeferred::addRenderable(RenderComponent* renderable)
{
	if (this->renderable)
	{
		JFF_LOG_WARNING("Cannot add more than one RenderComponent. Operation aborted")
		return;
	}

	if (renderable->getMaterialDomain() != Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED)
	{
		JFF_LOG_WARNING("Only renderables with material domain DIRECTIONAL_LIGHTING_DEFERRED is allowed. Operation aborted")
		return;
	}

	this->renderable = renderable;
}

void JFF::RenderPassDirectionalLightingDeferred::removeRenderable(RenderComponent* renderable)
{
	if (!this->renderable || this->renderable != renderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent because it's not present. Operation aborted")
		return;
	}

	this->renderable = nullptr;
}

void JFF::RenderPassDirectionalLightingDeferred::addLight(LightComponent* const light)
{
	// Check the concrete type of light
	if (DirectionalLightComponent* dirLight = dynamic_cast<DirectionalLightComponent*>(light))
	{
		directionalLights.push_back(dirLight);
	}
}

void JFF::RenderPassDirectionalLightingDeferred::removeLight(LightComponent* const light)
{
	// Check the concrete type of light
	if (DirectionalLightComponent* dirLight = dynamic_cast<DirectionalLightComponent*>(light))
	{
		// NOTE: This will remove ALL lights that points the same memory. Do no share LightComponent between GameObjects
		auto iter = std::remove(directionalLights.begin(), directionalLights.end(), dirLight);
		directionalLights.erase(iter, directionalLights.end());
	}
}

void JFF::RenderPassDirectionalLightingDeferred::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Adding environment map to deferred direction light pass is invalid")
}

void JFF::RenderPassDirectionalLightingDeferred::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Removing environment map from deferred direction light pass is invalid")
}
