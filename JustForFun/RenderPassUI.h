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

namespace JFF
{
	class Engine;

	class RenderPassUI : public RenderPass
	{
	public:
		// Ctor & Dtor
		explicit RenderPassUI(Engine* const engine);
		virtual ~RenderPassUI();

		// Copy ctor and copy assignment
		RenderPassUI(const RenderPassUI& other) = delete;
		RenderPassUI& operator=(const RenderPassUI& other) = delete;

		// Move ctor and assignment
		RenderPassUI(RenderPassUI&& other) = delete;
		RenderPassUI operator=(RenderPassUI&& other) = delete;

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
	};
}