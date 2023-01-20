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
	class InputProcessor
	{
	public:
		// Ctor & Dtor
		InputProcessor(Engine* const engine) {}
		virtual ~InputProcessor() {}

		// Copy ctor and copy assignment
		InputProcessor(const InputProcessor& other) = delete;
		InputProcessor& operator=(const InputProcessor& other) = delete;

		// Move ctor and assignment
		InputProcessor(InputProcessor&& other) = delete;
		InputProcessor operator=(InputProcessor&& other) = delete;

		// -------------------------------- INPUT PROCESSOR INTERFACE -------------------------------- //

		virtual _Ret process(const _Ret& inputValue) const = 0;
	};

	// Template typical uses
	using InputProcessorAxes = InputProcessor<Vec2>;
	using InputProcessorButton = InputProcessor<bool>;
	using InputProcessorTrigger = InputProcessor<float>;
}