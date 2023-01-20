/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassPointLightingDeferred.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassPointLightingDeferred::RenderPassPointLightingDeferred(Engine* const engine) :
	engine(engine),

	renderable(nullptr),
	pointLights()
{
	JFF_LOG_INFO("Ctor RenderPassPointLightingDeferred")
}

JFF::RenderPassPointLightingDeferred::~RenderPassPointLightingDeferred()
{
	JFF_LOG_INFO("Dtor RenderPassPointLightingDeferred")
}

void JFF::RenderPassPointLightingDeferred::execute()
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
	for (PointLightComponent* light : pointLights)
	{
		if (!light->isEnabled())
			continue;

		light->sendLightParams(renderable);
		renderable->draw();
	}
}

void JFF::RenderPassPointLightingDeferred::addRenderable(RenderComponent* renderable)
{
	if (this->renderable)
	{
		JFF_LOG_WARNING("Cannot add more than one RenderComponent. Operation aborted")
			return;
	}

	if (renderable->getMaterialDomain() != Material::MaterialDomain::POINT_LIGHTING_DEFERRED)
	{
		JFF_LOG_WARNING("Only renderables with material domain POINT_LIGHTING_DEFERRED is allowed. Operation aborted")
			return;
	}

	this->renderable = renderable;
}

void JFF::RenderPassPointLightingDeferred::removeRenderable(RenderComponent* renderable)
{
	if (!this->renderable || this->renderable != renderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent because it's not present. Operation aborted")
			return;
	}

	this->renderable = nullptr;
}

void JFF::RenderPassPointLightingDeferred::addLight(LightComponent* const light)
{
	// Check the concrete type of light
	if (PointLightComponent* pointLight = dynamic_cast<PointLightComponent*>(light))
	{
		pointLights.push_back(pointLight);
	}
}

void JFF::RenderPassPointLightingDeferred::removeLight(LightComponent* const light)
{	
	// Check the concrete type of light
	if (PointLightComponent* pointLight = dynamic_cast<PointLightComponent*>(light))
	{
		// NOTE: This will remove ALL lights that points the same memory. Do no share LightComponent between GameObjects
		auto iter = std::remove(pointLights.begin(), pointLights.end(), pointLight);
		pointLights.erase(iter, pointLights.end());
	}
}

void JFF::RenderPassPointLightingDeferred::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Adding environment map to deferred point light pass is invalid")
}

void JFF::RenderPassPointLightingDeferred::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Removing environment map from deferred point light pass is invalid")
}
