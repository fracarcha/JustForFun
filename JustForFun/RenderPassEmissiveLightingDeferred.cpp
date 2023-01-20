/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassEmissiveLightingDeferred.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassEmissiveLightingDeferred::RenderPassEmissiveLightingDeferred(Engine* const engine) :
	engine(engine),
	renderable(nullptr)
{
	JFF_LOG_INFO("Ctor RenderPassEmissiveLightingDeferred")
}

JFF::RenderPassEmissiveLightingDeferred::~RenderPassEmissiveLightingDeferred()
{
	JFF_LOG_INFO("Dtor RenderPassEmissiveLightingDeferred")
}

void JFF::RenderPassEmissiveLightingDeferred::execute()
{
	// Return if post process renderable is no present
	if (!renderable)
		return;

	// If this component is not enabled, skip its rendering
	if (!renderable->isEnabled())
		return;

	auto renderer = engine->renderer.lock();

	// Enable component material and bind all internal textures
	renderable->useMaterial();

	// Use geometry fbo's textures
	renderable->sendPostProcessingTextures(renderer->getGeometryFramebuffer());

	// Execute draw call
	renderable->draw();
}

void JFF::RenderPassEmissiveLightingDeferred::addRenderable(RenderComponent* renderable)
{
	if (this->renderable)
	{
		JFF_LOG_WARNING("Cannot add more than one RenderComponent. Operation aborted")
		return;
	}

	if (renderable->getMaterialDomain() != Material::MaterialDomain::EMISSIVE_LIGHTING_DEFERRED)
	{
		JFF_LOG_WARNING("Only renderables with material domain EMISSIVE_LIGHTING_DEFERRED is allowed. Operation aborted")
		return;
	}

	this->renderable = renderable;
}

void JFF::RenderPassEmissiveLightingDeferred::removeRenderable(RenderComponent* renderable)
{
	if (!this->renderable || this->renderable != renderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent because it's not present. Operation aborted")
		return;
	}

	this->renderable = nullptr;
}

void JFF::RenderPassEmissiveLightingDeferred::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Adding lights to deferred emissive light pass is invalid")
}

void JFF::RenderPassEmissiveLightingDeferred::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Removing lights from deferred emissive light pass is invalid")
}

void JFF::RenderPassEmissiveLightingDeferred::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Adding environment map to deferred emissive light pass is invalid")
}

void JFF::RenderPassEmissiveLightingDeferred::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Removing environment map from deferred emissive light pass is invalid")
}
