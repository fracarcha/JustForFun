/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RenderPassShadowCast.h"

#include "Log.h" // Log before Glew to calm down Windows.h redefinition warnings

#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"

#include <algorithm>

JFF::RenderPassShadowCast::RenderPassShadowCast(Engine* const engine) : 
	engine(engine),
	renderables(),

	directionalLights(),
	pointLights(),
	spotLights()
{
	JFF_LOG_INFO("Ctor RenderPassShadowCast")
}

JFF::RenderPassShadowCast::~RenderPassShadowCast()
{
	JFF_LOG_INFO("Dtor RenderPassShadowCast")
}

void JFF::RenderPassShadowCast::execute()
{
	// Return if there aren't lights
	if (directionalLights.size() <= 0 && pointLights.size() <= 0 && spotLights.size() <= 0)
		return;

	auto renderer = engine->renderer.lock();

	// Render depth shadows in back face to correct "peter panning" artifact
	renderer->faceCulling(Renderer::FaceCullOp::CULL_FRONT_FACES);

	// Render shadows on each type of light
	renderLights(directionalLights);
	renderOmnidirectionalLights();
	renderLights(spotLights);

	// Reset fixed pipeline options
	renderer->restoreFaceCulling();
}

void JFF::RenderPassShadowCast::addRenderable(RenderComponent* renderable)
{
	renderables.push_back(renderable);
}

void JFF::RenderPassShadowCast::removeRenderable(RenderComponent* renderable)
{
	// NOTE: This will remove ALL renderables that points the same memory. Do no share RenderComponent between GameObjects
	auto iter = std::remove(renderables.begin(), renderables.end(), renderable);
	renderables.erase(iter, renderables.end());
}

void JFF::RenderPassShadowCast::addLight(LightComponent* const light)
{
	// Ensure added light casts shadows
	if (!light->castShadows())
	{
		JFF_LOG_WARNING("Failed on adding light to shadow cast render pass. This light doesn't cast shadows. Operation aborted")
		return;
	}

	auto renderer = engine->renderer.lock();

	// Check the concrete type of light
	if (DirectionalLightComponent* dirLight = dynamic_cast<DirectionalLightComponent*>(light))
	{
		if (renderer->getRenderPath() == Renderer::RenderPath::FORWARD)
		{
			if (directionalLights.size() >= renderer->getForwardShadingMaxDirectionalLights())
			{
				JFF_LOG_WARNING("Reached max number of directional lights. New light won't be visible")
			}
			else
			{
				directionalLights.push_back(dirLight);
			}
		}
		else
		{
			directionalLights.push_back(dirLight);
		}
	}
	else if (PointLightComponent* pointLight = dynamic_cast<PointLightComponent*>(light))
	{
		if (renderer->getRenderPath() == Renderer::RenderPath::FORWARD)
		{
			if (pointLights.size() >= renderer->getForwardShadingMaxPointLights())
			{
				JFF_LOG_WARNING("Reached max number of point lights. New light won't be visible")
			}
			else
			{
				pointLights.push_back(pointLight);
			}
		}
		else
		{
			pointLights.push_back(pointLight);
		}
	}
	else if (SpotLightComponent* spotLight = dynamic_cast<SpotLightComponent*>(light))
	{
		if (renderer->getRenderPath() == Renderer::RenderPath::FORWARD)
		{
			if (spotLights.size() >= renderer->getForwardShadingMaxSpotLights())
			{
				JFF_LOG_WARNING("Reached max number of spot lights. New light won't be visible")
			}
			else
			{
				spotLights.push_back(spotLight);
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

void JFF::RenderPassShadowCast::removeLight(LightComponent* const light)
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

void JFF::RenderPassShadowCast::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot add an environment map to shadow cast render pass. Operation aborted")
}

void JFF::RenderPassShadowCast::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	JFF_LOG_WARNING("Cannot remove an environment map from shadow cast render pass. Operation aborted")
}

inline void JFF::RenderPassShadowCast::renderLights(const std::vector<LightComponent*>& lights)
{
	auto renderer = engine->renderer.lock();

	std::for_each(lights.begin(), lights.end(), [this, &renderer](LightComponent* lightComponent)
		{
			// If this light is not enabled, skip its rendering
			if (!lightComponent->isEnabled())
				return; // Technically, this is a 'continue' statement on a usual for loop

			// Enable each light's fbo to write results on depth buffer. Also clear the depth buffer and set the viewport size to match shadow map resolution
			lightComponent->enableShadowMapFramebuffer();
			unsigned int shadowMapWidth, shadowMapHeight;
			lightComponent->getShadowMapSizePixels(shadowMapWidth, shadowMapHeight);
			renderer->setViewport(0, 0, shadowMapWidth, shadowMapHeight);

			// Enable light material to cast shadows
			lightComponent->useMaterial();

			// Send light matrices
			lightComponent->sendMat4(ShaderCodeBuilder::VIEW_MATRIX.c_str(), lightComponent->getViewMatrix());
			lightComponent->sendMat4(ShaderCodeBuilder::PROJECTION_MATRIX.c_str(), lightComponent->getProjectionMatrix());

			std::for_each(renderables.begin(), renderables.end(), [this, &lightComponent](RenderComponent* renderComponent)
				{
					// If this render component is not enabled, skip its rendering
					if (!renderComponent->isEnabled())
						return; // Technically, this is a 'continue' statement on a usual for loop

					// Send Model matrix of light's material
					lightComponent->sendMat4(ShaderCodeBuilder::MODEL_MATRIX.c_str(), renderComponent->gameObject->transform.getModelMatrix());

					// Execute the draw call
					renderComponent->draw();
				});

		});
}

inline void JFF::RenderPassShadowCast::renderOmnidirectionalLights()
{
	auto renderer = engine->renderer.lock();

	std::for_each(pointLights.begin(), pointLights.end(), [this, &renderer](PointLightComponent* lightComponent)
		{
			// If this light is not enabled, skip its rendering
			if (!lightComponent->isEnabled())
				return; // Technically, this is a 'continue' statement on a usual for loop

			// Enable each light's fbo to write results on cubemap depth buffer. Also clear the depth buffer and set the viewport size to match shadow map resolution
			lightComponent->enableShadowMapFramebuffer();
			unsigned int shadowCubemapFaceWidth, shadowCubemapFaceHeight;
			lightComponent->getShadowMapSizePixels(shadowCubemapFaceWidth, shadowCubemapFaceHeight);
			renderer->setViewport(0, 0, shadowCubemapFaceWidth, shadowCubemapFaceHeight);

			// Enable light material to cast shadows
			lightComponent->useMaterial();

			// Send light matrices and other needed uniforms
			lightComponent->sendCubemapViewMatrices();
			lightComponent->sendMat4(ShaderCodeBuilder::PROJECTION_MATRIX.c_str(), lightComponent->getProjectionMatrix());
			lightComponent->sendVec3(ShaderCodeBuilder::LIGHT_POSITION.c_str(), lightComponent->gameObject->transform.getWorldPos());

			float zNear, zFar;
			lightComponent->getPointLightImportanceVolume(zNear, zFar);
			lightComponent->sendFloat(ShaderCodeBuilder::LIGHT_FAR_PLANE.c_str(), zFar);

			std::for_each(renderables.begin(), renderables.end(), [this, &lightComponent](RenderComponent* renderComponent) 
				{
					// If this render component is not enabled, skip its rendering
					if (!renderComponent->isEnabled())
						return; // Technically, this is a 'continue' statement on a usual for loop

					// Send Model matrix of light's material
					lightComponent->sendMat4(ShaderCodeBuilder::MODEL_MATRIX.c_str(), renderComponent->gameObject->transform.getModelMatrix());

					// Execute the draw call
					renderComponent->draw();
				});
		});
}