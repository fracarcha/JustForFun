/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassRenderToScreen.h"

#include "Log.h"
#include "Engine.h"

JFF::RenderPassRenderToScreen::RenderPassRenderToScreen(Engine* const engine) :
	engine(engine),
	renderToScreenRenderable(nullptr),
	postProcessRenderable(nullptr)
{
	JFF_LOG_INFO("Ctor RenderPassRenderToScreen")
}

JFF::RenderPassRenderToScreen::~RenderPassRenderToScreen()
{
	JFF_LOG_INFO("Dtor RenderPassRenderToScreen")
}

void JFF::RenderPassRenderToScreen::execute()
{
	// Return if render-to-screen component is not present
	if (!renderToScreenRenderable)
	{
		JFF_LOG_ERROR("Render-to-screen RenderComponent is not present. Please check if your scene's root node is properly configured")
		return;
	}

	// If this component is not enabled, skip its rendering
	if (!renderToScreenRenderable->isEnabled())
	{
		JFF_LOG_ERROR("Render-to-screen RenderComponent is disabled. Please check if your scene's root node is properly configured")
		return;
	}

	// Enable component material and bind all textures
	renderToScreenRenderable->useMaterial();

	// Send post process textures obtained from the last used framebuffer
	if (postProcessRenderable && postProcessRenderable->isEnabled())
	{
		renderToScreenRenderable->sendPostProcessingTextures(postProcessRenderable->getFramebuffer());
	}
	else
	{
		auto fbo = engine->renderer.lock()->getFramebuffer();
		renderToScreenRenderable->sendPostProcessingTextures(fbo);
	}

	// Execute the draw call
	renderToScreenRenderable->draw();
}

void JFF::RenderPassRenderToScreen::addRenderable(RenderComponent* renderable)
{
	PostProcessRenderComponent* ppRenderable = dynamic_cast<PostProcessRenderComponent*>(renderable);
	if (ppRenderable)
	{
		if (postProcessRenderable)
		{
			JFF_LOG_WARNING("Cannot add more than one PostProcessRenderComponent. Operation aborted")
			return;
		}

		// Store post processing renderables here because we need the last post processing textures
		postProcessRenderable = ppRenderable;
	}
	else
	{
		if (renderToScreenRenderable)
		{
			JFF_LOG_WARNING("Cannot add more than one render-to-screen RenderComponent. Operation aborted")
			return;
		}

		renderToScreenRenderable = renderable;
	}
}

void JFF::RenderPassRenderToScreen::removeRenderable(RenderComponent* renderable)
{
	PostProcessRenderComponent* ppRenderable = dynamic_cast<PostProcessRenderComponent*>(renderable);
	if (ppRenderable)
	{
		if (!postProcessRenderable || postProcessRenderable != ppRenderable)
		{
			JFF_LOG_WARNING("Couldn't remove PostProcessRenderComponent because it's not present. Operation aborted")
			return;
		}

		postProcessRenderable = nullptr;
	}
	else
	{
		if (!renderToScreenRenderable || renderToScreenRenderable != renderable)
		{
			JFF_LOG_WARNING("Couldn't remove render-to-screen RenderComponent because it's not present. Operation aborted")
			return;
		}

		renderToScreenRenderable = nullptr;
	}
}

void JFF::RenderPassRenderToScreen::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Cannot add a light to render-to-screen render pass. Operation aborted")
}

void JFF::RenderPassRenderToScreen::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Cannot remove a light to render-to-screen render pass. Operation aborted")
}

void JFF::RenderPassRenderToScreen::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot add an environment map to render-to-screen render pass. Operation aborted")
}

void JFF::RenderPassRenderToScreen::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot remove an environment map to render-to-screen render pass. Operation aborted")
}
