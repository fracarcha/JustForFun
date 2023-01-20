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
	class InputProcessorNormalizer : public InputProcessor<_Ret>
	{
	public:
		// Ctor & Dtor
		explicit InputProcessorNormalizer(Engine* const engine);
		virtual ~InputProcessorNormalizer();

		// Copy ctor and copy assignment
		InputProcessorNormalizer(const InputProcessorNormalizer& other) = delete;
		InputProcessorNormalizer& operator=(const InputProcessorNormalizer& other) = delete;

		// Move ctor and assignment
		InputProcessorNormalizer(InputProcessorNormalizer&& other) = delete;
		InputProcessorNormalizer operator=(InputProcessorNormalizer&& other) = delete;

		// -------------------------------- INPUT PROCESSOR OVERRIDES -------------------------------- //

		virtual _Ret process(const _Ret& inputValue) const override;

	protected:
		Engine* engine;
	};

	// Template typical uses
	using InputProcessorNormalizerAxes = InputProcessorNormalizer<Vec2>;
	using InputProcessorNormalizerButton = InputProcessorNormalizer<bool>;
	using InputProcessorNormalizerTrigger = InputProcessorNormalizer<float>;
}

// Template definitions
#include "InputProcessorNormalizer.inl"