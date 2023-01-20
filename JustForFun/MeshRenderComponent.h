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

#include "RenderComponent.h"
#include "MeshComponent.h"

#include <memory>

namespace JFF
{
	class MeshRenderComponent : public RenderComponent
	{
	public:
		// Ctor & Dtor
		MeshRenderComponent(
			GameObject* const gameObject,
			const char* name,
			bool initiallyEnabled,
			const char* materialAssetFilepath);
		MeshRenderComponent(
			GameObject* const gameObject,
			const char* name,
			bool initiallyEnabled,
			const std::shared_ptr<Material>& material);
		virtual ~MeshRenderComponent();

		// Copy ctor and copy assignment
		MeshRenderComponent(const MeshRenderComponent& other) = delete;
		MeshRenderComponent& operator=(const MeshRenderComponent& other) = delete;

		// Move ctor and assignment
		MeshRenderComponent(MeshRenderComponent&& other) = delete;
		MeshRenderComponent operator=(MeshRenderComponent&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() override;
		virtual void onDestroy() noexcept override;

		// ------------------------------- RENDER COMPONENT OVERRIDES ------------------------------- //

		// Gets the material domain, which translates to the render pass where this RenderComponent renders
		virtual Material::MaterialDomain getMaterialDomain() const override;

		// Gets the material light model
		virtual Material::LightModel getLightModel() const override;

		// Gets the drawable side of the model
		virtual Material::Side getMaterialSide() const override;

		// Gets debug display option if applicable
		virtual Material::DebugDisplay getDebugDisplay() const override;

		// Enables the internal shader and its associated textures
		virtual void useMaterial() override;

		/*
		* Send a 4x4 matrix to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shared code
		*/
		virtual void sendMat4(const char* variableName, const Mat4& matrix) override;

		/*
		* Send a 3x3 matrix to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shared code
		*/
		virtual void sendMat3(const char* variableName, const Mat3& matrix) override;

		/*
		* Send a vec3 to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec3(const char* variableName, const Vec3& vec) override;

		/*
		* Send a vec4 to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec4(const char* variableName, const Vec4& vec) override;

		/*
		* Send a float to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendFloat(const char* variableName, float f) override;

		/*
		* Send an environment map to active material.
		* If envMap is nullptr, sends an empty environment map to material. This is important if no environment map is present,
		* because the cubemap sampler in shader must be specified.
		* There are other maps that can be sent as part of environment map, like irradiance map, used for PBR rendering.
		*/
		virtual void sendEnvironmentMap(const std::shared_ptr<Cubemap>& envMap = nullptr,
			const std::shared_ptr<Cubemap>& irradianceMap = nullptr,
			const std::shared_ptr<Cubemap>& preFilteredMap = nullptr,
			const std::shared_ptr<Texture>& BRDFIntegrationMap = nullptr) override;

		/*
		* Send a directional light's shadowmap texture to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxDirectionalLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important
		* because all texture samplers in shader must be specified
		*/
		virtual void sendDirLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO = std::weak_ptr<Framebuffer>()) override;

		/*
		* Send a point light's shadowmap cubemap to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxPointLights())
		* If @shadowCubemapFBO is invalid, this function will send the default bind cubemap. This is important
		* because all cubemap samplers in shader must be specified
		*/
		virtual void sendPointLightShadowCubemap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowCubemapFBO = std::weak_ptr<Framebuffer>()) override;

		/*
		* Send a spot light's shadowmap texture to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxSpotLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important
		* because all texture samplers in shader must be specified
		*/
		virtual void sendSpotLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO = std::weak_ptr<Framebuffer>()) override;

		/*
		* Send all post-processing textures to this render component material
		*/
		virtual void sendPostProcessingTextures(const std::weak_ptr<Framebuffer>& ppFBO, const std::weak_ptr<Framebuffer>& ppFBO2 = std::weak_ptr<Framebuffer>()) override;

		// Enables the GPU buffer where the vertex data of associated mesh is stored and extecute a draw call
		virtual void draw() override;

	protected:
		const std::string materialAssetFilepath;
		std::shared_ptr<Material> material;
		std::weak_ptr<MeshComponent> mesh;
	};
}