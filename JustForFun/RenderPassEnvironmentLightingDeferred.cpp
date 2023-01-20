/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassEnvironmentLightingDeferred.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassEnvironmentLightingDeferred::RenderPassEnvironmentLightingDeferred(Engine* const engine) :
	engine(engine),

	renderable(nullptr),
	environmentMaps()
{
	JFF_LOG_INFO("Ctor RenderPassEnvironmentLightingDeferred")
}

JFF::RenderPassEnvironmentLightingDeferred::~RenderPassEnvironmentLightingDeferred()
{
	JFF_LOG_INFO("Dtor RenderPassEnvironmentLightingDeferred")
}

void JFF::RenderPassEnvironmentLightingDeferred::execute()
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
	for (EnvironmentMapComponent* envMap : environmentMaps)
	{
		if (!envMap->isEnabled())
			continue;

		envMap->sendEnvironmentMap(renderable);
		renderable->draw();
	}
}

void JFF::RenderPassEnvironmentLightingDeferred::addRenderable(RenderComponent* renderable)
{
	if (this->renderable)
	{
		JFF_LOG_WARNING("Cannot add more than one RenderComponent. Operation aborted")
		return;
	}

	if (renderable->getMaterialDomain() != Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED)
	{
		JFF_LOG_WARNING("Only renderables with material domain ENVIRONMENT_LIGHTING_DEFERRED is allowed. Operation aborted")
		return;
	}

	this->renderable = renderable;
}

void JFF::RenderPassEnvironmentLightingDeferred::removeRenderable(RenderComponent* renderable)
{
	if (!this->renderable || this->renderable != renderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent because it's not present. Operation aborted")
		return;
	}

	this->renderable = nullptr;
}

void JFF::RenderPassEnvironmentLightingDeferred::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Adding light to deferred environment light pass is invalid")
}

void JFF::RenderPassEnvironmentLightingDeferred::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Removing light from deferred environment light pass is invalid")
}

void JFF::RenderPassEnvironmentLightingDeferred::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	environmentMaps.push_back(envMap);
}

void JFF::RenderPassEnvironmentLightingDeferred::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	// NOTE: This will remove ALL environment maps that points the same memory. Do no share EnvironmentMapComponent between GameObjects
	auto iter = std::remove(environmentMaps.begin(), environmentMaps.end(), envMap);
	environmentMaps.erase(iter, environmentMaps.end());
}
