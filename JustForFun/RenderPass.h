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
#include "LightComponent.h"
#include "EnvironmentMapComponent.h"

namespace JFF
{
	class RenderPass
	{
	public:
		// Ctor & Dtor
		RenderPass() {}
		virtual ~RenderPass() {}

		// Copy ctor and copy assignment
		RenderPass(const RenderPass& other) = delete;
		RenderPass& operator=(const RenderPass& other) = delete;

		// Move ctor and assignment
		RenderPass(RenderPass&& other) = delete;
		RenderPass operator=(RenderPass&& other) = delete;

		// ---------------------------------- RENDER PASS INTERFACE ---------------------------------- //

		// Render
		virtual void execute() = 0;

		// Adds a new renderable. The meshes it represent will be drawn on screen
		virtual void addRenderable(RenderComponent* renderable) = 0;

		// Removes the renderable. The meshes it represent won't be drawn on screen anymore
		virtual void removeRenderable(RenderComponent* renderable) = 0;

		// Adds a new light. Lights will affect the look and feel of RenderComponents
		virtual void addLight(LightComponent* const light) = 0;

		// Removes a new light. Lights won't affect the look and feel of RenderComponents anymore
		virtual void removeLight(LightComponent* const light) = 0;

		// Adds a new environment map. Environment maps will affect the reflections of RenderComponents
		virtual void addEnvironmentMap(EnvironmentMapComponent* const envMap) = 0;

		// removes an environment map. This envirnoment won't affect reflections anymore
		virtual void removeEnvironmentMap(EnvironmentMapComponent* const envMap) = 0;
	};
}