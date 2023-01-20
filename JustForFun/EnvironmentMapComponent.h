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

namespace JFF
{
	class EnvironmentMapComponent : public Component
	{
	public:
		// Ctor & Dtor
		EnvironmentMapComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
			Component(gameObject, name, initiallyEnabled)
		{}
		virtual ~EnvironmentMapComponent() {}

		// Copy ctor and copy assignment
		EnvironmentMapComponent(const EnvironmentMapComponent& other) = delete;
		EnvironmentMapComponent& operator=(const EnvironmentMapComponent& other) = delete;

		// Move ctor and assignment
		EnvironmentMapComponent(EnvironmentMapComponent&& other) = delete;
		EnvironmentMapComponent operator=(EnvironmentMapComponent&& other) = delete;

		// ------------------------------- ENVIRONMENT MAP COMPONENT INTERFACE ------------------------------- //

		// Send environment map (cubemap) to active shader
		virtual void sendEnvironmentMap(RenderComponent* const renderComponent) = 0;
	};
}