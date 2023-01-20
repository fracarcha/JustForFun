/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputBehaviorHold.h"

template<typename _Ret>
inline JFF::InputBehaviorHold<_Ret>::InputBehaviorHold(Engine* const engine, double minHoldTime, float pressPoint) :
	InputBehavior<_Ret>(engine),
	engine(engine),
	minHoldTime(minHoldTime),
	pressPoint(pressPoint),
	lastBehavior(false),
	lastBehaviorUp(false),
	lastBehaviorDown(false),
	lastBehaviorLeft(false),
	lastBehaviorRight(false),
	holdBeginTimePoint()
{
	JFF_LOG_INFO("Ctor InputBehaviorHold")
}

template<typename _Ret>
inline JFF::InputBehaviorHold<_Ret>::~InputBehaviorHold()
{
	JFF_LOG_INFO("Dtor InputBehaviorHold")
}

template<typename _Ret>
inline bool JFF::InputBehaviorHold<_Ret>::checkBehavior(const _Ret& newBehavior, bool ignoreLastValueChecking)
{
	JFF_LOG_ERROR("Undefined input behavior")
	return false;
}

template<>
inline bool JFF::InputBehaviorHold<bool>::checkBehavior(const bool& newBehavior, bool ignoreLastValueChecking)
{
	return checkBehaviorBool(newBehavior, ignoreLastValueChecking);
}

template<>
inline bool JFF::InputBehaviorHold<float>::checkBehavior(const float& newBehavior, bool ignoreLastValueChecking)
{
	bool behaviorBool = newBehavior >= pressPoint; // True: pressed False: released
	return this->checkBehaviorBool(behaviorBool, ignoreLastValueChecking);
}

template<>
inline bool JFF::InputBehaviorHold<JFF::Vec2>::checkBehavior(const Vec2& newBehavior, bool ignoreLastValueChecking)
{
	bool behaviorLeft = newBehavior.x <= -pressPoint;
	bool behaviorRight = newBehavior.x >= pressPoint;
	bool behaviorUp = newBehavior.y >= pressPoint;
	bool behaviorDown = newBehavior.y <= -pressPoint;

	bool somethingChanged = false;
	if (behaviorLeft != lastBehaviorLeft || ignoreLastValueChecking)
	{
		lastBehaviorLeft = behaviorLeft;
		somethingChanged = somethingChanged || true;
	}
	if (behaviorRight != lastBehaviorRight || ignoreLastValueChecking)
	{
		lastBehaviorRight = behaviorRight;
		somethingChanged = somethingChanged || true;
	}
	if (behaviorUp != lastBehaviorUp || ignoreLastValueChecking)
	{
		lastBehaviorUp = behaviorUp;
		somethingChanged = somethingChanged || true;
	}
	if (behaviorDown != lastBehaviorDown || ignoreLastValueChecking)
	{
		lastBehaviorDown = behaviorDown;
		somethingChanged = somethingChanged || true;
	}

	if (!somethingChanged)
		return false;

	bool anyPressed = behaviorLeft || behaviorRight || behaviorUp || behaviorDown;

	// Start holding
	bool result = false;
	if (anyPressed)
	{
		holdBeginTimePoint = std::chrono::steady_clock::now();
	}

	// Releasing hold
	else
	{
		std::chrono::duration<double> deltaTimeSeconds = std::chrono::steady_clock::now() - holdBeginTimePoint;
		double elapsedTime = deltaTimeSeconds.count();
		result = elapsedTime > minHoldTime;
	}

	return result;
}

template<typename _Ret>
inline bool JFF::InputBehaviorHold<_Ret>::checkBehaviorBool(bool behavior, bool ignoreLastValueChecking)
{
	if (behavior == lastBehavior && !ignoreLastValueChecking)
		return false;

	bool result = false;
	lastBehavior = behavior;

	// Start holding
	if (behavior)
	{
		holdBeginTimePoint = std::chrono::steady_clock::now();
	}

	// Releasing hold
	else
	{
		std::chrono::duration<double> deltaTimeSeconds = std::chrono::steady_clock::now() - holdBeginTimePoint;
		double elapsedTime = deltaTimeSeconds.count();
		result = elapsedTime > minHoldTime;
	}

	return result;
}