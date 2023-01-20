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

#include "InputProcessor.h"

namespace JFF
{
	template<typename _Ret>
	class InputProcessorDeadZone : public InputProcessor<_Ret>
	{
	public:
		// Ctor & Dtor
		explicit InputProcessorDeadZone(Engine* const engine, float minValue = 0.2f, float maxValue = 0.9f);
		virtual ~InputProcessorDeadZone();

		// Copy ctor and copy assignment
		InputProcessorDeadZone(const InputProcessorDeadZone& other) = delete;
		InputProcessorDeadZone& operator=(const InputProcessorDeadZone& other) = delete;

		// Move ctor and assignment
		InputProcessorDeadZone(InputProcessorDeadZone&& other) = delete;
		InputProcessorDeadZone operator=(InputProcessorDeadZone&& other) = delete;

		// -------------------------------- INPUT PROCESSOR OVERRIDES -------------------------------- //

		virtual _Ret process(const _Ret& inputValue) const override;

	protected:
		Engine* engine;
		float minValue, maxValue;
		float lineSlope, lineOffset;
	};

	// Template typical uses
	using InputProcessorDeadZoneAxes = InputProcessorDeadZone<Vec2>;
	using InputProcessorDeadZoneButton = InputProcessorDeadZone<bool>;
	using InputProcessorDeadZoneTrigger = InputProcessorDeadZone<float>;
}

// Template definitions
#include "InputProcessorDeadZone.inl"