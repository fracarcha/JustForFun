/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassGeometryDeferred.h"

#include "Log.h"
#include "Engine.h"

#include "ShaderCodeBuilder.h"

JFF::RenderPassGeometryDeferred::RenderPassGeometryDeferred(Engine* const engine) : 
	engine(engine),
	renderables()
{
	JFF_LOG_INFO("Ctor RenderPassGeometryDeferred")
}

JFF::RenderPassGeometryDeferred::~RenderPassGeometryDeferred()
{
	JFF_LOG_INFO("Dtor RenderPassGeometryDeferred")
}

void JFF::RenderPassGeometryDeferred::execute()
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

void JFF::RenderPassGeometryDeferred::addRenderable(RenderComponent* renderable)
{
	renderables.push_back(renderable);
}

void JFF::RenderPassGeometryDeferred::removeRenderable(RenderComponent* renderable)
{
	// NOTE: This will remove ALL renderables that points the same memory. Do no share RenderComponent between GameObjects
	auto iter = std::remove(renderables.begin(), renderables.end(), renderable);
	renderables.erase(iter, renderables.end());
}

void JFF::RenderPassGeometryDeferred::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Adding light to deferred geometry pass is invalid")
}

void JFF::RenderPassGeometryDeferred::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Removing light from deferred geometry pass is invalid")
}

void JFF::RenderPassGeometryDeferred::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Adding environment map to deferred geometry pass is invalid")
}

void JFF::RenderPassGeometryDeferred::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Removing environment map from deferred geometry pass is invalid")
}

inline void JFF::RenderPassGeometryDeferred::renderPass()
{
	auto renderer = engine->renderer.lock();

	std::for_each(renderables.begin(), renderables.end(), [this, &renderer](RenderComponent* renderComponent) 
		{
			// If this component is not enabled, skip its rendering
			if (!renderComponent->isEnabled())
				return; // Technically, this is a 'continue' statement on a usual for loop

			// Enable component material and bind all textures
			renderComponent->useMaterial();

			/*
			* Check which face of the model will be drawn and which one will be discarded.
			* Because RendererGL default option is glCullFace(GL_BACK), default option is FRONT
			*/
			switch (renderComponent->getMaterialSide())
			{
			case Material::Side::BACK:
				renderer->faceCulling(Renderer::FaceCullOp::CULL_FRONT_FACES); // Discard front faces
				break;
			case Material::Side::TWO_SIDED:
				renderer->faceCulling(Renderer::FaceCullOp::DISABLE); // Don't discard anything
				break;
			case Material::Side::FRONT:
			default:
				// Default option is already configured. Do nothing
				break;
			}

			// Send Model and normal matrix of this renderable
			renderComponent->sendMat4(ShaderCodeBuilder::MODEL_MATRIX.c_str(), renderComponent->gameObject->transform.getModelMatrix());
			renderComponent->sendMat3(ShaderCodeBuilder::NORMAL_MATRIX.c_str(), renderComponent->gameObject->transform.getNormalMatrix());

			// Execute the draw call
			renderComponent->draw();

			// Revert material's default draw side
			switch (renderComponent->getMaterialSide())
			{
			case Material::Side::BACK:
			case Material::Side::TWO_SIDED:
				renderer->restoreFaceCulling();
				break;
			case Material::Side::FRONT:
			default:
				// Default option is already configured. Do nothing
				break;
			}

		});
}
