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

#include "ExecutableSubsystem.h"

#include "RenderComponent.h"
#include "LightComponent.h"
#include "EnvironmentMapComponent.h"

#include "Framebuffer.h"
#include <memory>

namespace JFF
{
	class Renderer : public ExecutableSubsystem
	{
	public:
		enum class RenderPath : char
		{
			FORWARD,
			DEFERRED,
		};

		enum class DepthOp : char
		{
			NEVER_PASS,
			ALWAYS_PASS,

			PASS_IF_DEPTH_IS_LESS,
			PASS_IF_DEPTH_IS_LESS_OR_EQUAL,
			PASS_IF_DEPTH_IS_GREATER,
			PASS_IF_DEPTH_IS_GREATER_OR_EQUAL,

			PASS_IF_DEPTH_IS_EQUAL,
			PASS_IF_DEPTH_IS_NOT_EQUAL,
		};

		enum class BlendOp : char
		{
			ALPHA_BLEND,	// Dst = Src * Src.a + Dst * (1.0 - Src.a)
			ADDITIVE,		// Dst = Src + Dst
			MULTIPLY,		// Dst = Src * Dst
		};

		enum class FaceCullOp : char
		{
			DISABLE,
			CULL_FRONT_FACES,
			CULL_BACK_FACES,
		};

		// Ctor & Dtor
		Renderer() {}
		virtual ~Renderer() {}

		// Copy ctor and copy assignment
		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

		// Move ctor and assignment
		Renderer(Renderer&& other) = delete;
		Renderer operator=(Renderer&& other) = delete;

		// ------------------------------------ RENDERER INTERFACE ------------------------------------ //

		// Adds a new renderable. The meshes it represent will be drawn on screen
		virtual void addRenderable(RenderComponent* const renderable) = 0;

		// Removes the renderable. The meshes it represent won't be drawn on screen anymore
		virtual void removeRenderable(RenderComponent* const renderable) = 0;

		// Adds a new light. Lights will affect the look and feel of RenderComponents
		virtual void addLight(LightComponent* const light) = 0;

		// Removes a light. Lights won't affect the look and feel of RenderComponents anymore
		virtual void removeLight(LightComponent* const light) = 0;

		// Adds a new environment map. Environment maps will affect the reflections of RenderComponents
		virtual void addEnvironmentMap(EnvironmentMapComponent* const envMap) = 0;

		// removes an environment map. This envirnoment won't affect reflections anymore
		virtual void removeEnvironmentMap(EnvironmentMapComponent* const envMap) = 0;

		// ------------- Light limitations ------------- //

		// Gets the maximum number of per RenderComponent point lights in Forward Shading render path
		virtual int getForwardShadingMaxPointLights() const = 0;

		// Gets the maximum number of per RenderComponent directional lights in Forward Shading render path
		virtual int getForwardShadingMaxDirectionalLights() const = 0;

		// Gets the maximum number of per RenderComponent spot lights in Forward Shading render path
		virtual int getForwardShadingMaxSpotLights() const = 0;

		// ------------ Environment map limitations ------------- //

		// Gets the maximum number of per RenderComponent environment maps in current render path
		virtual int getForwardShadingMaxEnvironmentMaps() const = 0;

		// ------------- Render path ------------- //

		// Gets current render path
		virtual RenderPath getRenderPath() const = 0;

		// ------------ Framebuffer functions -------------- //

		// Get the framebuffer used to do pre-processing
		virtual std::weak_ptr<Framebuffer> getFramebuffer() const = 0;
		// Get the geometry framebuffer in deferred shading. In forward shading, this pre-process FBO
		virtual std::weak_ptr<Framebuffer> getGeometryFramebuffer() const = 0;

		// Sets the viewport size, commonly used to do custom render passes that targets to framebuffers of different sizes
		virtual void setViewport(int x, int y, int width, int height) = 0;
		// Restore the viewport size of the default framebuffer
		virtual void restoreViewport() = 0;

		// Enables depth test. Render passes also writes to depth buffer by default
		virtual void enableDepthTest() = 0;
		// Enables depth test giving the option to enable/disable writing on depth buffer
		virtual void enableDepthTest(bool writeToDepthBuffer) = 0;
		/* 
		* Enables depth test giving the option to enable/disable writing on depth buffer.
		* Sets the function that this renderer use to pass/fail depth test
		*/
		virtual void enableDepthTest(bool writeToDepthBuffer, DepthOp depthOp) = 0;
		// Disable depth test
		virtual void disableDepthTest() = 0;
		// Restore depth test to Renderer defaults (Depth test on and writing to depth buffer enabled)
		virtual void restoreDepthTest() = 0;

		// Enables framebuffer blending operations. Default operation is ALPHA_BLEND
		virtual void enableBlending() = 0;
		// Enables blending and sets the blend operation
		virtual void enableBlending(BlendOp op) = 0;
		// Disables framebuffer blending operations and restores the default blend operation, which is ALPHA_BLEND
		virtual void disableBlending() = 0;

		// Sets which faces will be discarded
		virtual void faceCulling(FaceCullOp op) = 0;
		// Resets face culling to Renderer defaults
		virtual void restoreFaceCulling() = 0;

		// Enable rendering objects using lines only
		virtual void enableWireframeMode() = 0;
		// Disable rendering objects using lines only (Default behavior)
		virtual void disableWireframeMode() = 0;
	};
}