/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassBackground.h"

#include "Log.h" // Log before Glew to calm down Windows.h redefinition warnings

#include "Engine.h"
#include "ShaderCodeBuilder.h"
#include <algorithm>

JFF::RenderPassBackground::RenderPassBackground(Engine* const engine) : 
	engine(engine),
	renderables()
{
	JFF_LOG_INFO("Ctor RenderPassBackground")
}

JFF::RenderPassBackground::~RenderPassBackground()
{
	JFF_LOG_INFO("Dtor RenderPassBackground")
}

void JFF::RenderPassBackground::execute()
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

	auto renderer = engine->renderer.lock();

	// Disable writing on depth buffer and use less-equal as depth test function, because z == 1.0
	renderer->enableDepthTest(false, Renderer::DepthOp::PASS_IF_DEPTH_IS_LESS_OR_EQUAL);

	// Draw all enabled renderables
	renderPass();

	// Reset fixed pipeline options
	renderer->restoreDepthTest();
}

void JFF::RenderPassBackground::addRenderable(RenderComponent* renderable)
{
	renderables.push_back(renderable);
}

void JFF::RenderPassBackground::removeRenderable(RenderComponent* renderable)
{
	// NOTE: This will remove ALL renderables that points the same memory. Do no share RenderComponent between GameObjects
	auto iter = std::remove(renderables.begin(), renderables.end(), renderable);
	renderables.erase(iter, renderables.end());
}

void JFF::RenderPassBackground::addLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Cannot add a light to background render pass because it's unlit. Operation aborted")
}

void JFF::RenderPassBackground::removeLight(LightComponent* const light)
{
	JFF_LOG_WARNING("Cannot remove a light to background render pass because it's unlit. Operation aborted")
}

void JFF::RenderPassBackground::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot add an environment map to background render pass. Operation aborted")
}

void JFF::RenderPassBackground::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot remove an environment map to background render pass. Operation aborted")
}

// --------------------------- HELPER FUNCTIONS --------------------------- //

inline void JFF::RenderPassBackground::renderPass()
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

			// Send Model and rotation matrix of this renderable
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
