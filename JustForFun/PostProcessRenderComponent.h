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
#include "PostProcessFX.h"

namespace JFF
{
	class PostProcessRenderComponent : public RenderComponent
	{
	public:
		enum class ExecutionMode : char
		{
			POST_PROCESS,
			POST_PROCESS_PRE_LIGHTING,
		};

		// Ctor & Dtor
		PostProcessRenderComponent(
			GameObject* const gameObject,
			const char* name,
			bool initiallyEnabled,
			const char* materialAssetFilepath);
		PostProcessRenderComponent(
			GameObject* const gameObject,
			const char* name,
			bool initiallyEnabled,
			const std::shared_ptr<Material>& material);
		virtual ~PostProcessRenderComponent();

		// Copy ctor and copy assignment
		PostProcessRenderComponent(const PostProcessRenderComponent& other) = delete;
		PostProcessRenderComponent& operator=(const PostProcessRenderComponent& other) = delete;

		// Move ctor and assignment
		PostProcessRenderComponent(PostProcessRenderComponent&& other) = delete;
		PostProcessRenderComponent operator=(PostProcessRenderComponent&& other) = delete;

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

		// ------------------------------- POST-PROCESS RENDER COMPONENT FUNCTIONS ------------------------------- //

		// Changes the way executeCustomRenderPass() works
		virtual void setExecutionMode(ExecutionMode mode);

		// Execute some additional render passes to the main render pass. It's used to do special drawings (e.g. Gaussian blur, High pass filter...)
		virtual void executeCustomRenderPass(const std::weak_ptr<Framebuffer>& ppFBO, const std::weak_ptr<Framebuffer>& ppFBO2 = std::weak_ptr<Framebuffer>());

		// Enable the framebuffer attached to this render component to be the current render target
		virtual void enablePostProcessFramebuffer();

		// Disable the framebuffer attached to this render component
		virtual void disablePostProcessFramebuffer();

		// Gets the framebuffer object associated to this render component
		virtual std::weak_ptr<Framebuffer> getFramebuffer() const;

	private:
		inline void buildCustomFX(int bufferWidth, int bufferHeight);

	protected:
		const std::string materialAssetFilepath;
		unsigned long long FBOSizeCallbackHandler;
		
		std::weak_ptr<MeshComponent> mesh;

		std::shared_ptr<Material> material;
		std::shared_ptr<Framebuffer> fbo;

		// Custom post process FX
		ExecutionMode executionMode;
		std::vector<std::shared_ptr<PostProcessFX>> fx;
		std::vector<std::shared_ptr<PostProcessFX>> fxPreLighting;
	};
}