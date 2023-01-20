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

#include "InputBehavior.h"
#include "Vec.h"

namespace JFF
{
	template<typename _Ret>
	class InputBehaviorPress : public InputBehavior<_Ret>
	{
	public:
		enum class Type
		{
			PRESS,
			RELEASE,
			PRESS_AND_RELEASE, // checkBehavior() will return true always, except if newBehavior == lastBehavior
		};

		// Ctor & Dtor
		// NOTE: Press point is the value that an axis or trigger has to reach in order to consider it pressed
		explicit InputBehaviorPress(Engine* const engine, Type pressType = Type::PRESS_AND_RELEASE, float pressPoint = 0.5f);
		virtual ~InputBehaviorPress();

		// Copy ctor and copy assignment
		InputBehaviorPress(const InputBehaviorPress& other) = delete;
		InputBehaviorPress& operator=(const InputBehaviorPress& other) = delete;

		// Move ctor and assignment
		InputBehaviorPress(InputBehaviorPress&& other) = delete;
		InputBehaviorPress operator=(InputBehaviorPress&& other) = delete;

		// -------------------------------- INPUT BEHAVIOR OVERRIDES -------------------------------- //

		virtual bool checkBehavior(const _Ret& newBehavior, bool ignoreLastValueChecking = false) override;

	private:
		inline bool checkBehaviorBool(bool behavior, bool ignoreLastValueChecking);

	protected:
		Engine* engine;

		Type pressType;
		float pressPoint;

		bool lastBehavior; // True: press False: release

		bool lastBehaviorUp;
		bool lastBehaviorDown;
		bool lastBehaviorLeft;
		bool lastBehaviorRight;
	};

	// Template typical uses
	using InputBehaviorPressAxes = InputBehaviorPress<Vec2>;
	using InputBehaviorPressButton = InputBehaviorPress<bool>;
	using InputBehaviorPressTrigger = InputBehaviorPress<float>;
}

// Template definitions
#include "InputBehaviorPress.inl"