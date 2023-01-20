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

#include "RenderPass.h"

#include "PostProcessRenderComponent.h"

namespace JFF
{
	class Engine;
	class Camera;

	class RenderPassPostProcess : public RenderPass
	{
	public:
		// Ctor & Dtor
		explicit RenderPassPostProcess(Engine* const engine);
		virtual ~RenderPassPostProcess();

		// Copy ctor and copy assignment
		RenderPassPostProcess(const RenderPassPostProcess& other) = delete;
		RenderPassPostProcess& operator=(const RenderPassPostProcess& other) = delete;

		// Move ctor and assignment
		RenderPassPostProcess(RenderPassPostProcess&& other) = delete;
		RenderPassPostProcess operator=(RenderPassPostProcess&& other) = delete;

		// ---------------------------------- RENDER PASS INTERFACE ---------------------------------- //

		// Render
		virtual void execute() override;

		// Adds a new renderable. The meshes it represent will be drawn on screen
		virtual void addRenderable(RenderComponent* renderable) override;

		// Removes the renderable. The meshes it represent won't be drawn on screen anymore
		virtual void removeRenderable(RenderComponent* renderable) override;

		// Adds a new light. Lights will affect the look and feel of RenderComponents
		virtual void addLight(LightComponent* const light) override;

		// Removes a new light. Lights won't affect the look and feel of RenderComponents anymore
		virtual void removeLight(LightComponent* const light) override;

		// Adds a new environment map. Environment maps will affect the reflections of RenderComponents
		virtual void addEnvironmentMap(EnvironmentMapComponent* const envMap) override;

		// removes an environment map. This envirnoment won't affect reflections anymore
		virtual void removeEnvironmentMap(EnvironmentMapComponent* const envMap) override;

	protected:
		Engine* engine;
		PostProcessRenderComponent* renderable;
	};
}