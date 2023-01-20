/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputBehaviorMultipress.h"

template<typename _Ret>
inline JFF::InputBehaviorMultipress<_Ret>::InputBehaviorMultipress(
	Engine* const engine, 
	unsigned int pressCount, 
	double maxTime, 
	float pressPoint) :
	InputBehavior<_Ret>(engine),

	engine(engine),
	maxTime(maxTime),
	minPressesNeeded(pressCount),
	pressPoint(pressPoint),

	lastBehavior(false),
	lastBehaviorDirection(),

	beginTimePoint(),
	currentNumPresses(0)
{
	JFF_LOG_INFO("Ctor InputBehaviorMultipress")
}

template<typename _Ret>
inline JFF::InputBehaviorMultipress<_Ret>::~InputBehaviorMultipress()
{
	JFF_LOG_INFO("Dtor InputBehaviorMultipress")
}

template<typename _Ret>
inline bool JFF::InputBehaviorMultipress<_Ret>::checkBehavior(const _Ret& newBehavior, bool ignoreLastValueChecking)
{
	JFF_LOG_ERROR("Undefined input behavior")
	return false;
}

template<>
inline bool JFF::InputBehaviorMultipress<bool>::checkBehavior(const bool& newBehavior, bool ignoreLastValueChecking)
{
	return checkBehaviorBool(newBehavior, ignoreLastValueChecking);
}

template<>
inline bool JFF::InputBehaviorMultipress<float>::checkBehavior(const float& newBehavior, bool ignoreLastValueChecking)
{
	bool behaviorBool = newBehavior >= pressPoint; // True: pressed False: released
	return this->checkBehaviorBool(behaviorBool, ignoreLastValueChecking);
}

template<>
inline bool JFF::InputBehaviorMultipress<JFF::Vec2>::checkBehavior(const Vec2& newBehavior, bool ignoreLastValueChecking)
{
	bool behaviorLeft = newBehavior.x <= -pressPoint;
	bool behaviorRight = newBehavior.x >= pressPoint;
	bool behaviorUp = newBehavior.y >= pressPoint;
	bool behaviorDown = newBehavior.y <= -pressPoint;

	bool anyPressed = behaviorLeft || behaviorRight || behaviorUp || behaviorDown;
	if (anyPressed && newBehavior != lastBehaviorDirection)
	{
		lastBehavior = anyPressed;
		lastBehaviorDirection = newBehavior;
		currentNumPresses = 0;
		beginTimePoint = std::chrono::steady_clock::now();
		
		return false;
	}

	return checkBehaviorBool(anyPressed, ignoreLastValueChecking);
}

template<typename _Ret>
inline bool JFF::InputBehaviorMultipress<_Ret>::checkBehaviorBool(bool behavior, bool ignoreLastValueChecking)
{
	if (behavior == lastBehavior && !ignoreLastValueChecking)
		return false;

	bool result = false;
	lastBehavior = behavior;

	// Calculate time since last different behavior
	std::chrono::duration<double> deltaTimeSeconds = std::chrono::steady_clock::now() - beginTimePoint;
	double elapsedTime = deltaTimeSeconds.count();

	// On press
	if (behavior)
	{
		if (elapsedTime > maxTime)
		{
			beginTimePoint = std::chrono::steady_clock::now();
			currentNumPresses = 0;
		}
		else if (currentNumPresses <= 0)
		{
			beginTimePoint = std::chrono::steady_clock::now();
		}
	}

	// On release
	else
	{
		if (elapsedTime > maxTime)
			return false;

		currentNumPresses++;
		if (currentNumPresses >= minPressesNeeded)
		{
			result = true;
			currentNumPresses = 0;
		}
	}

	return result;
}
