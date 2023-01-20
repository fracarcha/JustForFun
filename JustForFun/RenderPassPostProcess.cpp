/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassPostProcess.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassPostProcess::RenderPassPostProcess(Engine* const engine) :
	engine(engine),
	renderable(nullptr)
{
	JFF_LOG_INFO("Ctor RenderPassPostProcess")
}

JFF::RenderPassPostProcess::~RenderPassPostProcess()
{
	JFF_LOG_INFO("Dtor RenderPassPostProcess")
}

void JFF::RenderPassPostProcess::execute()
{
	// Return if post process renderable is no present
	if (!renderable)
		return;

	// If this component is not enabled, skip its rendering
	if (!renderable->isEnabled())
		return;

	auto renderer = engine->renderer.lock();

	// Execute custom post-process passes in first place
	renderable->setExecutionMode(PostProcessRenderComponent::ExecutionMode::POST_PROCESS);
	renderable->executeCustomRenderPass(renderer->getFramebuffer(), renderer->getGeometryFramebuffer());

	// Execute normal post-process pass
	renderable->enablePostProcessFramebuffer();							// Use the renderComponent's fbo
	renderable->useMaterial();											// Enable component material and bind all textures
	renderable->sendPostProcessingTextures(renderer->getFramebuffer(), renderer->getGeometryFramebuffer()); // Use previous fbo's texture
	// TODO: Deferred shading path: Send lights and environment maps
	renderable->draw();													// Execute the draw call
	renderable->disablePostProcessFramebuffer();						// Disable current active fbo
}

void JFF::RenderPassPostProcess::addRenderable(RenderComponent* renderable)
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

void JFF::RenderPassPostProcess::removeRenderable(RenderComponent* renderable)
{
	if (!this->renderable)
	{
		JFF_LOG_WARNING("Couldn't remove PostProcessRenderComponent because it's not present. Operation aborted")
		return;
	}

	PostProcessRenderComponent* ppRenderable = dynamic_cast<PostProcessRenderComponent*>(renderable);
	if (!ppRenderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent from RenderPassPostProcess because it's not PostProcessRenderComponent. Aborted.")
		return;
	}

	if (this->renderable != ppRenderable)
	{
		JFF_LOG_WARNING("Couldn't remove RenderComponent from RenderPassPostProcess because it's not present. Aborted.")
		return;
	}

	this->renderable = nullptr;
}

void JFF::RenderPassPostProcess::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Adding light to post-process workflow is invalid")
}

void JFF::RenderPassPostProcess::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Removing light to post-process workflow is invalid")
}

void JFF::RenderPassPostProcess::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Adding environment map to post-process workflow is invalid")
}

void JFF::RenderPassPostProcess::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Removing environment map to post-process workflow is invalid")
}
