/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassDebug.h"

#include "Log.h"

#include "Engine.h"
#include "ShaderCodeBuilder.h"
#include <algorithm>

JFF::RenderPassDebug::RenderPassDebug(Engine* const engine) : 
	engine(engine),
	renderables()
{
	JFF_LOG_INFO("Ctor RenderPassDebug")
}

JFF::RenderPassDebug::~RenderPassDebug()
{
	JFF_LOG_INFO("Dtor RenderPassDebug")
}

void JFF::RenderPassDebug::execute()
{
	// Return if there aren't renderables
	if (renderables.size() <= 0)
		return;

	// Check if there is an active camera to display the image on
	auto cameraManager = engine->camera.lock();
	if (!cameraManager->hasAnyActiveCamera())
	{
		JFF_LOG_ERROR("No active camera is configured for rendering")
			return;
	}

	// Draw all enabled renderables
	renderPass();
}

void JFF::RenderPassDebug::addRenderable(RenderComponent* renderable)
{
	renderables.push_back(renderable);
}

void JFF::RenderPassDebug::removeRenderable(RenderComponent* renderable)
{
	// NOTE: This will remove ALL renderables that points the same memory. Do no share RenderComponent between GameObjects
	auto iter = std::remove(renderables.begin(), renderables.end(), renderable);
	renderables.erase(iter, renderables.end());
}

void JFF::RenderPassDebug::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Cannot add a light to debug render pass. Operation aborted")
}

void JFF::RenderPassDebug::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Cannot remove a light from debug render pass. Operation aborted")
}

void JFF::RenderPassDebug::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot add an environment map to debug render pass. Operation aborted")
}

void JFF::RenderPassDebug::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot remove an environment map from debug render pass. Operation aborted")
}

// -------------------------------- HELPER FUNCTIONS -------------------------------- //

inline void JFF::RenderPassDebug::renderPass()
{
	auto renderer = engine->renderer.lock();

	std::for_each(renderables.begin(), renderables.end(), [this, &renderer](RenderComponent* renderComponent) 
		{
			// If this component is not enabled, skip its rendering
			if (!renderComponent->isEnabled())
				return; // Technically, this is a 'continue' statement on a usual for loop

			// Enable component material and bind all textures
			renderComponent->useMaterial();

			// Choose OpenGL options from debug options
			switch (renderComponent->getDebugDisplay())
			{
			case Material::DebugDisplay::POLYGONS:
				renderer->disableDepthTest();
				renderer->enableWireframeMode();
				break;
			case Material::DebugDisplay::NO_DISPLAY:
			case Material::DebugDisplay::NORMALS:
			default:
				break;
			}

			// Send Model and rotation matrix of this renderable
			renderComponent->sendMat4(ShaderCodeBuilder::MODEL_MATRIX.c_str(), renderComponent->gameObject->transform.getModelMatrix());
			renderComponent->sendMat3(ShaderCodeBuilder::NORMAL_MATRIX.c_str(), renderComponent->gameObject->transform.getNormalMatrix());

			// Execute the draw call
			renderComponent->draw();

			// Choose OpenGL options from debug options
			switch (renderComponent->getDebugDisplay())
			{
			case Material::DebugDisplay::POLYGONS:
				renderer->restoreDepthTest();
				renderer->disableWireframeMode();
				break;
			case Material::DebugDisplay::NO_DISPLAY:
			case Material::DebugDisplay::NORMALS:
			default:
				break;
			}
		});
}
