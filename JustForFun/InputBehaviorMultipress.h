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
	class InputBehaviorMultipress : public InputBehavior<_Ret>
	{
	public:
		// Ctor & Dtor
		// NOTE: Press point is the value that an axis or trigger has to reach in order to consider it pressed
		explicit InputBehaviorMultipress(Engine* const engine, unsigned int pressCount = 2, double maxTime = 0.5f, float pressPoint = 0.5f);
		virtual ~InputBehaviorMultipress();

		// Copy ctor and copy assignment
		InputBehaviorMultipress(const InputBehaviorMultipress& other) = delete;
		InputBehaviorMultipress& operator=(const InputBehaviorMultipress& other) = delete;

		// Move ctor and assignment
		InputBehaviorMultipress(InputBehaviorMultipress&& other) = delete;
		InputBehaviorMultipress operator=(InputBehaviorMultipress&& other) = delete;

		// -------------------------------- INPUT BEHAVIOR OVERRIDES -------------------------------- //

		virtual bool checkBehavior(const _Ret& newBehavior, bool ignoreLastValueChecking = false) override;

	private:
		inline bool checkBehaviorBool(bool behavior, bool ignoreLastValueChecking);

	protected:
		Engine* engine;

		double maxTime;
		int minPressesNeeded;
		float pressPoint;

		bool lastBehavior; // True: press False: release
		Vec2 lastBehaviorDirection;

		std::chrono::steady_clock::time_point beginTimePoint;
		int currentNumPresses;
	};

	// Template typical uses
	using InputBehaviorMultipressAxes = InputBehaviorMultipress<Vec2>;
	using InputBehaviorMultipressButton = InputBehaviorMultipress<bool>;
	using InputBehaviorMultipressTrigger = InputBehaviorMultipress<float>;
}

// Template definitions
#include "InputBehaviorMultipress.inl"