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

#include "Engine.h"

namespace JFF
{
	template<typename _Ret>
	class InputBehavior
	{
	public:
		// Ctor & Dtor
		InputBehavior(Engine* const engine) {}
		virtual ~InputBehavior() {}

		// Copy ctor and copy assignment
		InputBehavior(const InputBehavior& other) = delete;
		InputBehavior& operator=(const InputBehavior& other) = delete;

		// Move ctor and assignment
		InputBehavior(InputBehavior&& other) = delete;
		InputBehavior operator=(InputBehavior&& other) = delete;

		// -------------------------------- INPUT BEHAVIOR INTERFACE -------------------------------- //

		// Checks if user input behavior matches with this InputBehavior. Returns true if behavior matches
		virtual bool checkBehavior(const _Ret& newValue, bool ignoreLastValueChecking = false) = 0;
	};

	// Template typical uses
	using InputBehaviorAxes = InputBehavior<Vec2>;
	using InputBehaviorButton = InputBehavior<bool>;
	using InputBehaviorTrigger = InputBehavior<float>;
}