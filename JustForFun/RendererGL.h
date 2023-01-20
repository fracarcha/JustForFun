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

#include "Renderer.h"
#include "RenderPass.h"

#include <vector>
#include <map>

namespace JFF
{
	// Renderer version implemented using OpenGL
	class RendererGL : public Renderer
	{
	public:
		// Ctor & Dtor
		RendererGL();
		virtual ~RendererGL();

		// Copy ctor and copy assignment
		RendererGL(const RendererGL& other) = delete;
		RendererGL& operator=(const RendererGL& other) = delete;

		// Move ctor and assignment
		RendererGL(RendererGL&& other) = delete;
		RendererGL operator=(RendererGL&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ExecutableSubsystem impl
		virtual ExecutableSubsystem::ExecutionOrder getExecutionOrder() const override;
		virtual bool execute() override;

		// ------------------------------------ RENDERER INTERFACE ------------------------------------ //

		/*
		* Adds a new renderable.The meshes it represent will be drawn on screen.
		* The RenderPass for the RenderComponent will be selected depending on the material domain
		*/
		virtual void addRenderable(RenderComponent* const renderable) override;

		// Removes the renderable. The meshes it represent won't be drawn on screen anymore
		virtual void removeRenderable(RenderComponent* const renderable) override;

		// Adds a new light. Lights will affect the look and feel of RenderComponents
		virtual void addLight(LightComponent* const light) override;

		// Removes a new light. Lights won't affect the look and feel of RenderComponents anymore
		virtual void removeLight(LightComponent* const light) override;

		// Adds a new environment map. Environment maps will affect the reflections of RenderComponents
		virtual void addEnvironmentMap(EnvironmentMapComponent* const envMap) override;

		// removes an environment map. This envirnoment won't affect reflections anymore
		virtual void removeEnvironmentMap(EnvironmentMapComponent* const envMap) override;

		// ------------- Light limitations ------------- //

		// Gets the maximum number of per RenderComponent point lights in Forward Shading render path
		virtual int getForwardShadingMaxPointLights() const override;

		// Gets the maximum number of per RenderComponent directional lights in Forward Shading render path
		virtual int getForwardShadingMaxDirectionalLights() const override;

		// Gets the maximum number of per RenderComponent spot lights in Forward Shading render path
		virtual int getForwardShadingMaxSpotLights() const override;

		// ------------ Environment map limitations ------------- //

		// Gets the maximum number of per RenderComponent environment maps in current render path
		virtual int getForwardShadingMaxEnvironmentMaps() const override;

		// ------------- Render path ------------- //

		// Gets current render path
		virtual RenderPath getRenderPath() const override;

		// ------------ Framebuffer functions -------------- //

		// Get the framebuffer used to do pre-processing
		virtual std::weak_ptr<Framebuffer> getFramebuffer() const override;
		// Get the geometry framebuffer in deferred shading. In forward shading, this pre-process FBO
		virtual std::weak_ptr<Framebuffer> getGeometryFramebuffer() const override;

		// Sets the viewport size, commonly used to do custom render passes that targets to framebuffers of different sizes
		virtual void setViewport(int x, int y, int width, int height) override;
		// Restore the viewport size of the default framebuffer
		virtual void restoreViewport() override;

		// Enables depth test. Render passes also writes to depth buffer by default
		virtual void enableDepthTest() override;
		// Enables depth test giving the option to enable/disable writing on depth buffer
		virtual void enableDepthTest(bool writeToDepthBuffer) override;
		/*
		* Enables depth test giving the option to enable/disable writing on depth buffer.
		* Sets the function that this renderer use to pass/fail depth test
		*/
		virtual void enableDepthTest(bool writeToDepthBuffer, DepthOp depthOp) override;
		// Disable depth test
		virtual void disableDepthTest() override;
		// Restore depth test to Renderer defaults (Depth test on and writing to depth buffer enabled)
		virtual void restoreDepthTest() override;

		// Enables framebuffer blending operations
		virtual void enableBlending() override;
		// Enables blending and sets the blend operation
		virtual void enableBlending(BlendOp op) override;
		// Disables framebuffer blending operations and restores the default blend operation, which is ALPHA_BLEND
		virtual void disableBlending() override;

		// Sets which faces will be discarded
		virtual void faceCulling(FaceCullOp op) override;
		// Resets face culling to Renderer defaults
		virtual void restoreFaceCulling() override;

		// Enable rendering objects using lines only
		virtual void enableWireframeMode() override;
		// Disable rendering objects using lines only (Default behavior)
		virtual void disableWireframeMode() override;

	private:
		struct Params
		{
			RenderPath renderPath;

			int maxPointLightsForwardShading;
			int maxDirectionalLightsForwardShading;
			int maxSpotLightsForwardShading;
		};
		inline Params loadConfigFile() const;
		inline void executeForward();
		inline void executeDeferred();

	protected:
		Engine* engine;
		RenderPath activeRenderPath;
		std::map<Material::MaterialDomain, std::shared_ptr<RenderPass>> renderables;

		// Forward shading:		0 -> FBO_PRE_PROCESS_FORWARD
		// Deferred shading:	0 -> FBO_GEOMETRY_DEFERRED | 1 -> FBO_LIGHTING_DEFERRED
		std::vector<std::shared_ptr<Framebuffer>> FBOs;
		int fbWidth, fbHeight;
		int samplesPerPixel;

		unsigned long long int framebufferCallbackHandler;

		int maxPointLightsForwardShading;
		int maxDirectionalLightsForwardShading;
		int maxSpotLightsForwardShading;
		
		const int maxEnvironmentMapsForwardShading;
	};
}