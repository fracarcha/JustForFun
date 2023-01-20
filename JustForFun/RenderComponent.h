/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#pragma once

#include "Component.h"
#include "Material.h"
#include "Mat.h"
#include "Cubemap.h"

namespace JFF
{
	class RenderComponent : public Component
	{
	public:
		// Ctor & Dtor
		RenderComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
			Component(gameObject, name, initiallyEnabled)
		{}
		virtual ~RenderComponent() {}

		// Copy ctor and copy assignment
		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;

		// Move ctor and assignment
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent operator=(RenderComponent&& other) = delete;

		// ----------------------------- RENDER COMPONENT FUNCTIONS ----------------------------- //

		// Gets the material domain, which translates to the render pass where this RenderComponent renders
		virtual Material::MaterialDomain getMaterialDomain() const = 0;

		// Gets the material light model
		virtual Material::LightModel getLightModel() const = 0;

		// Gets the drawable side of the model
		virtual Material::Side getMaterialSide() const = 0;

		// Gets debug display option if applicable
		virtual Material::DebugDisplay getDebugDisplay() const = 0;

		// Enables the internal shader and its associated textures
		virtual void useMaterial() = 0;

		/* 
		* Send a 4x4 matrix to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendMat4(const char* variableName, const Mat4& matrix) = 0;

		/*
		* Send a 3x3 matrix to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendMat3(const char* variableName, const Mat3& matrix) = 0;

		/*
		* Send a vec3 to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec3(const char* variableName, const Vec3& vec) = 0;

		/*
		* Send a vec4 to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec4(const char* variableName, const Vec4& vec) = 0;

		/*
		* Send a float to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendFloat(const char* variableName, float f) = 0;

		/*
		* Send an environment map to active material.
		* If envMap is nullptr, sends an empty environment map to material. This is important if no environment map is present,
		* because the cubemap sampler in shader must be specified.
		* There are other maps that can be sent as part of environment map, like irradiance map, used for PBR rendering
		*/
		virtual void sendEnvironmentMap(const std::shared_ptr<Cubemap>& envMap = nullptr,
			const std::shared_ptr<Cubemap>& irradianceMap = nullptr,
			const std::shared_ptr<Cubemap>& preFilteredMap = nullptr,
			const std::shared_ptr<Texture>& BRDFIntegrationMap = nullptr) = 0;

		/*
		* Send a directional light's shadowmap texture to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxDirectionalLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important
		* because all texture samplers in shader must be specified
		*/
		virtual void sendDirLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO = std::weak_ptr<Framebuffer>()) = 0;

		/*
		* Send a point light's shadowmap cubemap to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxPointLights())
		* If @shadowCubemapFBO is invalid, this function will send the default bind cubemap. This is important
		* because all cubemap samplers in shader must be specified
		*/
		virtual void sendPointLightShadowCubemap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowCubemapFBO = std::weak_ptr<Framebuffer>()) = 0;

		/*
		* Send a spot light's shadowmap texture to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxSpotLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important
		* because all texture samplers in shader must be specified
		*/
		virtual void sendSpotLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO = std::weak_ptr<Framebuffer>()) = 0;

		/*
		* Send all post-processing textures to this render component material
		*/
		virtual void sendPostProcessingTextures(const std::weak_ptr<Framebuffer>& ppFBO, const std::weak_ptr<Framebuffer>& ppFBO2 = std::weak_ptr<Framebuffer>()) = 0;

		// Enables the GPU buffer where the vertex data of associated mesh is stored and extecute a draw call
		virtual void draw() = 0;

	};
}