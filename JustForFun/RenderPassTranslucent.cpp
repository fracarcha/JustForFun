/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassTranslucent.h"

#include "Log.h"
#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include <algorithm>

JFF::RenderPassTranslucent::RenderPassTranslucent(Engine* const engine) :
	engine(engine),
	renderables(),

	directionalLights(),
	pointLights(),
	spotLights(),

	environmentMaps()
{
	JFF_LOG_INFO("Ctor RenderPassTranslucent")
}

JFF::RenderPassTranslucent::~RenderPassTranslucent()
{
	JFF_LOG_INFO("Dtor RenderPassTranslucent")
}

void JFF::RenderPassTranslucent::execute()
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

	// Disable writing on Depth buffer. This prevents from one translucent object covers another one
	renderer->enableDepthTest(false);

	// Enable alpha blending. Check RendererGL to see blend options
	renderer->enableBlending();

	// Draw back faces only. Check RendererGL to see cull options
	renderPass(/* cullFrontFaces */ true);

	// Draw front face only. Check RendererGL to see cull options
	renderPass(/* cullFrontFaces */ false);

	// Reset fixed pipeline options
	renderer->restoreDepthTest();
	renderer->disableBlending();
}

void JFF::RenderPassTranslucent::addRenderable(RenderComponent* renderable)
{
	renderables.push_back(renderable);
}

void JFF::RenderPassTranslucent::removeRenderable(RenderComponent* renderable)
{
	// NOTE: This will remove ALL renderables that points the same memory. Do no share RenderComponent between GameObjects
	auto iter = std::remove(renderables.begin(), renderables.end(), renderable);
	renderables.erase(iter, renderables.end());
}

void JFF::RenderPassTranslucent::addLight(LightComponent* const light)
{
	auto renderer = engine->renderer.lock();

	// TODO: Study the limitation of lights in deferred shading. For the moment, beyond max lights won't be added

	// Check the concrete type of light
	if (DirectionalLightComponent* dirLight = dynamic_cast<DirectionalLightComponent*>(light))
	{
		if (directionalLights.size() >= renderer->getForwardShadingMaxDirectionalLights())
		{
			if (renderer->getRenderPath() == Renderer::RenderPath::FORWARD)
			{
				JFF_LOG_WARNING("Reached max number of directional lights. New light won't be visible")
			}
		}
		else
		{
			directionalLights.push_back(dirLight);
		}
	}
	else if (PointLightComponent* pointLight = dynamic_cast<PointLightComponent*>(light))
	{
		if (pointLights.size() >= renderer->getForwardShadingMaxPointLights())
		{
			if (renderer->getRenderPath() == Renderer::RenderPath::FORWARD)
			{
				JFF_LOG_WARNING("Reached max number of point lights. New light won't be visible")
			}
		}
		else
		{
			pointLights.push_back(pointLight);
		}
	}
	else if (SpotLightComponent* spotLight = dynamic_cast<SpotLightComponent*>(light))
	{
		if (spotLights.size() >= renderer->getForwardShadingMaxSpotLights())
		{
			if (renderer->getRenderPath() == Renderer::RenderPath::FORWARD)
			{
				JFF_LOG_WARNING("Reached max number of spot lights. New light won't be visible")
			}
		}
		else
		{
			spotLights.push_back(spotLight);
		}
	}
	else
	{
		JFF_LOG_ERROR("Unknown light type")
	}
}

void JFF::RenderPassTranslucent::removeLight(LightComponent* const light)
{
	// Check the concrete type of light
	if (DirectionalLightComponent* dirLight = dynamic_cast<DirectionalLightComponent*>(light))
	{
		// NOTE: This will remove ALL lights that points the same memory. Do no share LightComponent between GameObjects
		auto iter = std::remove(directionalLights.begin(), directionalLights.end(), dirLight);
		directionalLights.erase(iter, directionalLights.end());
	}
	else if (PointLightComponent* pointLight = dynamic_cast<PointLightComponent*>(light))
	{
		// NOTE: This will remove ALL lights that points the same memory. Do no share LightComponent between GameObjects
		auto iter = std::remove(pointLights.begin(), pointLights.end(), pointLight);
		pointLights.erase(iter, pointLights.end());
	}
	else if (SpotLightComponent* spotLight = dynamic_cast<SpotLightComponent*>(light))
	{
		// NOTE: This will remove ALL lights that points the same memory. Do no share LightComponent between GameObjects
		auto iter = std::remove(spotLights.begin(), spotLights.end(), spotLight);
		spotLights.erase(iter, spotLights.end());
	}
	else
	{
		JFF_LOG_ERROR("Unknown light type")
	}
}

void JFF::RenderPassTranslucent::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	auto renderer = engine->renderer.lock();

	// TODO: Study the limitation of environment maps in deferred shading. For the moment, beyond max lights won't be added

	// Check the concrete type of light
	if (environmentMaps.size() >= renderer->getForwardShadingMaxEnvironmentMaps())
	{
		if (renderer->getRenderPath() == Renderer::RenderPath::FORWARD)
		{
			JFF_LOG_WARNING("Reached max number of environment maps. New map won't be reflected")
		}
	}
	else
	{
		environmentMaps.push_back(envMap);
	}
}

void JFF::RenderPassTranslucent::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	// NOTE: This will remove ALL environment maps that points the same memory. Do no share EnvironmentMapComponent between GameObjects
	auto iter = std::remove(environmentMaps.begin(), environmentMaps.end(), envMap);
	environmentMaps.erase(iter, environmentMaps.end());
}

inline void JFF::RenderPassTranslucent::renderPass(bool cullFrontFaces)
{
	// Get max num lights
	auto renderer = engine->renderer.lock();
	const int maxDirLights = renderer->getForwardShadingMaxDirectionalLights();
	const int maxPointLights = renderer->getForwardShadingMaxPointLights();
	const int maxSpotLights = renderer->getForwardShadingMaxSpotLights();

	// Cull selected faces for all renderables
	renderer->faceCulling(cullFrontFaces ? Renderer::FaceCullOp::CULL_FRONT_FACES : Renderer::FaceCullOp::CULL_BACK_FACES);

	std::for_each(renderables.begin(), renderables.end(), [this, cullFrontFaces, &maxDirLights, &maxPointLights, &maxSpotLights](RenderComponent* renderComponent)
		{
			// If this component is not enabled, skip its rendering
			if (!renderComponent->isEnabled())
				return; // Technically, this is a 'continue' statement on a usual for loop

			// Enable component material and bind all textures
			renderComponent->useMaterial();

			/*
			* Check which face of the model will be drawn and which one will be discarded.
			*/
			switch (renderComponent->getMaterialSide())
			{
			case Material::Side::BACK:
				if (!cullFrontFaces) // Discard this rendering if translucent pass culls back faces
					return;
				break;
			case Material::Side::FRONT:
				if (cullFrontFaces) // Discard this rendering if translucent pass culls front faces
					return;
				break;
			case Material::Side::TWO_SIDED:
			default:
				// Default option is already configured. Do nothing
				break;
			}

			// Send Model and rotation matrix of this renderable
			renderComponent->sendMat4(ShaderCodeBuilder::MODEL_MATRIX.c_str(), renderComponent->gameObject->transform.getModelMatrix());
			renderComponent->sendMat3(ShaderCodeBuilder::NORMAL_MATRIX.c_str(), renderComponent->gameObject->transform.getNormalMatrix());

			// Add each environment map
			if (environmentMaps.size() <= 0)
			{
				renderComponent->sendEnvironmentMap(); // Send environment map empty
			}
			else
			{
				for (auto envMap : environmentMaps)
				{
					if (envMap->isEnabled())
						envMap->sendEnvironmentMap(renderComponent);
				}
			}

			// Add each light info
			for (int i = 0; i < directionalLights.size(); ++i)
			{
				if (directionalLights[i]->isEnabled())
					directionalLights[i]->sendLightParams(renderComponent, i);
			} 
			for (int i = (int) directionalLights.size(); i < maxDirLights; ++i)
				renderComponent->sendDirLightShadowMap(i); // Send empty dir light shadow maps

			for (int i = 0; i < pointLights.size(); ++i)
			{
				if (pointLights[i]->isEnabled())
					pointLights[i]->sendLightParams(renderComponent, i);
			}
			for (int i = (int) pointLights.size(); i < maxPointLights; ++i)
				renderComponent->sendPointLightShadowCubemap(i); // Send empty point light shadow cubemaps

			for (int i = 0; i < spotLights.size(); ++i)
			{
				if (spotLights[i]->isEnabled())
					spotLights[i]->sendLightParams(renderComponent, i);
			} 
			for (int i = (int) spotLights.size(); i < maxSpotLights; ++i)
				renderComponent->sendSpotLightShadowMap(i); // Send empty spot light shadow maps

			// Execute the draw call
			renderComponent->draw();

		});
}
