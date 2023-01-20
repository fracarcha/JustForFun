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
	class InputProcessorInverter : public InputProcessor<_Ret>
	{
	public:
		// Ctor & Dtor
		explicit InputProcessorInverter(Engine* const engine, bool invertX = false, bool invertY = false);
		virtual ~InputProcessorInverter();

		// Copy ctor and copy assignment
		InputProcessorInverter(const InputProcessorInverter& other) = delete;
		InputProcessorInverter& operator=(const InputProcessorInverter& other) = delete;

		// Move ctor and assignment
		InputProcessorInverter(InputProcessorInverter&& other) = delete;
		InputProcessorInverter operator=(InputProcessorInverter&& other) = delete;

		// -------------------------------- INPUT PROCESSOR OVERRIDES -------------------------------- //

		virtual _Ret process(const _Ret& inputValue) const override;

	protected:
		Engine* engine;
		bool invertX, invertY;
	};

	// Template typical uses
	using InputProcessorInverterAxes = InputProcessorInverter<Vec2>;
	using InputProcessorInverterButton = InputProcessorInverter<bool>;
	using InputProcessorInverterTrigger = InputProcessorInverter<float>;
}

// Template definitions
#include "InputProcessorInverter.inl"