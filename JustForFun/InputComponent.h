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

namespace JFF
{
	class InputComponent : public Component
	{
	public:
		// Ctor & Dtor
		InputComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
			Component(gameObject, name, initiallyEnabled)
		{}
		virtual ~InputComponent() {}

		// Copy ctor and copy assignment
		InputComponent(const InputComponent& other) = delete;
		InputComponent& operator=(const InputComponent& other) = delete;

		// Move ctor and assignment
		InputComponent(InputComponent&& other) = delete;
		InputComponent operator=(InputComponent&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() = 0;
	};
}