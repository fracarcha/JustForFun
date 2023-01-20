/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputBehaviorPress.h"

#include "Log.h"

template<typename _Ret>
inline JFF::InputBehaviorPress<_Ret>::InputBehaviorPress(Engine* const engine, Type pressType, float pressPoint) :
	InputBehavior<_Ret>(engine),
	engine(engine),
	pressType(pressType),
	pressPoint(pressPoint),
	lastBehavior(false),
	lastBehaviorUp(false),
	lastBehaviorDown(false),
	lastBehaviorLeft(false),
	lastBehaviorRight(false)
{
	JFF_LOG_INFO("Ctor InputBehaviorPress")
}

template<typename _Ret>
inline JFF::InputBehaviorPress<_Ret>::~InputBehaviorPress()
{
	JFF_LOG_INFO("Dtor InputBehaviorPress")
}

template<typename _Ret>
inline bool JFF::InputBehaviorPress<_Ret>::checkBehavior(const _Ret& newBehavior, bool ignoreLastValueChecking)
{
	JFF_LOG_ERROR("Undefined input behavior")
	return false;
}

template<>
inline bool JFF::InputBehaviorPress<bool>::checkBehavior(const bool& newBehavior, bool ignoreLastValueChecking)
{
	return checkBehaviorBool(newBehavior, ignoreLastValueChecking);
}

template<>
inline bool JFF::InputBehaviorPress<float>::checkBehavior(const float& newBehavior, bool ignoreLastValueChecking)
{
	bool behaviorBool = newBehavior >= pressPoint; // True: pressed False: released
	return this->checkBehaviorBool(behaviorBool, ignoreLastValueChecking);
}

template<>
inline bool JFF::InputBehaviorPress<JFF::Vec2>::checkBehavior(const Vec2& newBehavior, bool ignoreLastValueChecking)
{
	bool behaviorLeft = newBehavior.x <= -pressPoint;
	bool behaviorRight = newBehavior.x >= pressPoint;
	bool behaviorUp = newBehavior.y >= pressPoint;
	bool behaviorDown = newBehavior.y <= -pressPoint;

	bool result = false;
	if (behaviorLeft != lastBehaviorLeft || ignoreLastValueChecking)
	{
		switch (pressType)
		{
		case Type::PRESS:
			result = result || behaviorLeft;
			break;
		case Type::RELEASE:
			result = result || !behaviorLeft;
			break;
		case Type::PRESS_AND_RELEASE:
			result = result || true;
			break;
		default:
			break;
		}

		lastBehaviorLeft = behaviorLeft;
	}

	if (behaviorRight != lastBehaviorRight || ignoreLastValueChecking)
	{
		switch (pressType)
		{
		case Type::PRESS:
			result = result || behaviorRight;
			break;
		case Type::RELEASE:
			result = result || !behaviorRight;
			break;
		case Type::PRESS_AND_RELEASE:
			result = result || true;
			break;
		default:
			break;
		}

		lastBehaviorRight = behaviorRight;
	}

	if (behaviorUp != lastBehaviorUp || ignoreLastValueChecking)
	{
		switch (pressType)
		{
		case Type::PRESS:
			result = result || behaviorUp;
			break;
		case Type::RELEASE:
			result = result || !behaviorUp;
			break;
		case Type::PRESS_AND_RELEASE:
			result = result || true;
			break;
		default:
			break;
		}

		lastBehaviorUp = behaviorUp;
	}

	if (behaviorDown != lastBehaviorDown || ignoreLastValueChecking)
	{
		switch (pressType)
		{
		case Type::PRESS:
			result = result || behaviorDown;
			break;
		case Type::RELEASE:
			result = result || !behaviorDown;
			break;
		case Type::PRESS_AND_RELEASE:
			result = result || true;
			break;
		default:
			break;
		}

		lastBehaviorDown = behaviorDown;
	}

	return result;
}

template<typename _Ret>
inline bool JFF::InputBehaviorPress<_Ret>::checkBehaviorBool(bool behavior, bool ignoreLastValueChecking)
{
	if (behavior == lastBehavior && !ignoreLastValueChecking)
		return false;

	bool result = false;
	switch (pressType)
	{
	case Type::PRESS:
		result = behavior;
		break;
	case Type::RELEASE:
		result = !behavior;
		break;
	case Type::PRESS_AND_RELEASE:
		result = true;
		break;
	default:
		break;
	}

	lastBehavior = behavior;
	return result;
}