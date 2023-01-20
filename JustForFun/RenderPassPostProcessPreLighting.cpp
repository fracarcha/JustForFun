/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassPostProcessPreLighting.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassPostProcessPreLighting::RenderPassPostProcessPreLighting(Engine* const engine) :
	engine(engine),
	renderable(nullptr)
{
	JFF_LOG_INFO("Ctor RenderPassPostProcessPreLighting")
}

JFF::RenderPassPostProcessPreLighting::~RenderPassPostProcessPreLighting()
{
	JFF_LOG_INFO("Dtor RenderPassPostProcessPreLighting")
}

void JFF::RenderPassPostProcessPreLighting::execute()
{
	// Return if post process renderable is no present
	if (!renderable)
		return;

	// If this component is not enabled, skip its rendering
	if (!renderable->isEnabled())
		return;

	auto renderer = engine->renderer.lock();

	// Execute custom post-process passes in first place
	renderable->setExecutionMode(PostProcessRenderComponent::ExecutionMode::POST_PROCESS_PRE_LIGHTING);
	renderable->executeCustomRenderPass(renderer->getFramebuffer(), renderer->getGeometryFramebuffer());
}

void JFF::RenderPassPostProcessPreLighting::addRenderable(RenderComponent* renderable)
{
	if (this->renderable)
	{
		JFF_LOG_WARNING("Cannot add more than one PostProcessRenderComponent. Operation aborted")
		return;
	}

	PostProcessRenderComponent* ppRenderable = dynamic_cast<PostProcessRenderComponent*>(renderable);
	if (!ppRenderable)
	{
		JFF_LOG_WARNING("Only PostProcessRenderComponent can be combined with materials which domain is POST_PROCESS. The RenderComponent won't be added")
		return;
	}

	this->renderable = ppRenderable;
}

void JFF::RenderPassPostProcessPreLighting::removeRenderable(RenderComponent* renderable)
{
	if (!this->renderable)
	{
		JFF_LOG_WARNING("Couldn't remove PostProcessRenderComponent because it's not present. Operation aborted")
		return;
	}

	PostProcessRenderComponent* ppRenderable = dynamic_cast<PostProcessRenderComponent*>(renderable);
	if (!ppRenderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent from RenderPassPostProcessPreLighting because it's not PostProcessRenderComponent. Aborted.")
		return;
	}

	if (this->renderable != ppRenderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent from RenderPassPostProcessPreLighting because it's not present. Aborted.")
		return;
	}

	this->renderable = nullptr;
}

void JFF::RenderPassPostProcessPreLighting::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Adding light to post-process pre-lighting workflow is invalid")
}

void JFF::RenderPassPostProcessPreLighting::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Removing light from post-process pre-lighting workflow is invalid")
}

void JFF::RenderPassPostProcessPreLighting::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Adding environment map to post-process pre-lighting workflow is invalid")
}

void JFF::RenderPassPostProcessPreLighting::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Removing environment map from post-process pre-lighting workflow is invalid")
}
