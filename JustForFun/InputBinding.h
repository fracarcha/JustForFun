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

#include "InputBindingBase.h"
#include "InputAction.h"
#include "InputBehavior.h"
#include "InputProcessor.h"
#include "InputMappingSetup.h"

#include "Vec.h"
#include <string>

namespace JFF
{
	template<typename _Ret>
	class InputBinding : public InputBindingBase
	{
	public:
		// Ctor & Dtor
		explicit InputBinding(
			const std::string& name,
			Engine* const engine,
			InputAction<_Ret>* const parentAction,
			Mapping inputMapping) {}
		virtual ~InputBinding() {}

		// Copy ctor and copy assignment
		InputBinding(const InputBinding& other) = delete;
		InputBinding& operator=(const InputBinding& other) = delete;

		// Move ctor and assignment
		InputBinding(InputBinding&& other) = delete;
		InputBinding operator=(InputBinding&& other) = delete;

		// -------------------------------- INPUT BINDING BASE INTERFACE -------------------------------- //

		virtual std::string getName() const = 0;

		/*
		* Some input events may be generated many times in a single frame (e.g. mouse delta), so they need to
		* be accumulated until the frame ends. This function will reset all these accumulators
		*/
		virtual void resetAccumulators() = 0;

		// -------------------------------- INPUT BINDING INTERFACE -------------------------------- //

		virtual void setBehavior(const std::shared_ptr<InputBehavior<_Ret>>& behavior) = 0;
		virtual void addProcessor(const std::shared_ptr<InputProcessor<_Ret>>& processor) = 0;

		// -------------------------------- HELPER FUNCTIONS -------------------------------- //

	public:
		static Mapping stringToMapping(const std::string& mappingName);

	protected:
		virtual void initSubscriptions() = 0;
		virtual void cancelSubscriptions() = 0;
	};

	// Template typical uses
	using InputBindingAxes = InputBinding<Vec2>;
	using InputBindingButton = InputBinding<bool>;
	using InputBindingTrigger = InputBinding<float>;
}

// Template definitions
#include "InputBinding.inl"