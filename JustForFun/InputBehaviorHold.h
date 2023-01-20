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

#include <chrono>

namespace JFF
{
	template<typename _Ret>
	class InputBehaviorHold : public InputBehavior<_Ret>
	{
	public:
		// Ctor & Dtor
		// NOTE: Press point is the value that an axis or trigger has to reach in order to consider it pressed
		explicit InputBehaviorHold(Engine* const engine, double minHoldTime = 0.4, float pressPoint = 0.5f);
		virtual ~InputBehaviorHold();

		// Copy ctor and copy assignment
		InputBehaviorHold(const InputBehaviorHold& other) = delete;
		InputBehaviorHold& operator=(const InputBehaviorHold& other) = delete;

		// Move ctor and assignment
		InputBehaviorHold(InputBehaviorHold&& other) = delete;
		InputBehaviorHold operator=(InputBehaviorHold&& other) = delete;

		// -------------------------------- INPUT BEHAVIOR OVERRIDES -------------------------------- //

		virtual bool checkBehavior(const _Ret& newBehavior, bool ignoreLastValueChecking = false) override;

	private:
		inline bool checkBehaviorBool(bool behavior, bool ignoreLastValueChecking);

	protected:
		Engine* engine;

		double minHoldTime;
		float pressPoint;

		bool lastBehavior; // True: press False: release

		bool lastBehaviorUp;
		bool lastBehaviorDown;
		bool lastBehaviorLeft;
		bool lastBehaviorRight;

		std::chrono::steady_clock::time_point holdBeginTimePoint;
	};

	// Template typical uses
	using InputBehaviorHoldAxes = InputBehaviorHold<Vec2>;
	using InputBehaviorHoldButton = InputBehaviorHold<bool>;
	using InputBehaviorHoldTrigger = InputBehaviorHold<float>;
}

// Template definitions
#include "InputBehaviorHold.inl"