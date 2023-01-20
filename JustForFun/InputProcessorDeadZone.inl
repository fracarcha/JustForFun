/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputProcessorDeadZone.h"

template<typename _Ret>
inline JFF::InputProcessorDeadZone<_Ret>::InputProcessorDeadZone(Engine* const engine, float minValue, float maxValue) :
	InputProcessor<_Ret>(engine),
	engine(engine),
	minValue(minValue),
	maxValue(maxValue),

	// Next params form the line equation y = ax + b
	lineSlope((1.0f - 0.0f) / (maxValue - minValue)), // a = (y - y') / (x - x')
	lineOffset(-minValue) // b
{
	JFF_LOG_INFO("Ctor InputProcessorDeadZone")
}

template<typename _Ret>
inline JFF::InputProcessorDeadZone<_Ret>::~InputProcessorDeadZone()
{
	JFF_LOG_INFO("Dtor InputProcessorDeadZone")
}

template<typename _Ret>
inline _Ret JFF::InputProcessorDeadZone<_Ret>::process(const _Ret& inputValue) const
{
	JFF_LOG_ERROR("This processor is axes compatible only")
	return inputValue;
}

template<>
inline JFF::Vec2 JFF::InputProcessorDeadZone<JFF::Vec2>::process(const JFF::Vec2& inputValue) const
{
	auto math = engine->math.lock();

	// Split sign from value
	float signX = inputValue.x >= 0.0f ? 1.0f : -1.0f;
	float signY = inputValue.y >= 0.0f ? 1.0f : -1.0f;
	float valueAbsX = math->abs(inputValue.x);
	float valueAbsY = math->abs(inputValue.y);

	// Transform X axis using transference function
	float outputX;
	if (valueAbsX <= minValue)
		outputX = 0.0f;
	else if (valueAbsX >= maxValue)
		outputX = signX;
	else
		outputX = signX * (lineSlope * (valueAbsX + lineOffset)); // Use the Line function (y = ax + b) to renormalize values between 0 and 1

	// Transform Y axis using transference function
	float outputY;
	if (valueAbsY <= minValue)
		outputY = 0.0f;
	else if (valueAbsY >= maxValue)
		outputY = signY;
	else
		outputY = signY * (lineSlope * (valueAbsY + lineOffset)); // Use the Line function (y = ax + b) to renormalize values between 0 and 1

	return Vec2(math->clamp(outputX, -1.0f, 1.0f), math->clamp(outputY, -1.0f, 1.0f));
}