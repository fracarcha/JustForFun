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

#include <string>

namespace JFF
{
	class InputBindingBase
	{
	public:
		// Ctor & Dtor
		InputBindingBase() {}
		virtual ~InputBindingBase() {}

		// Copy ctor and copy assignment
		InputBindingBase(const InputBindingBase& other) = delete;
		InputBindingBase& operator=(const InputBindingBase& other) = delete;

		// Move ctor and assignment
		InputBindingBase(InputBindingBase&& other) = delete;
		InputBindingBase operator=(InputBindingBase&& other) = delete;

		// -------------------------------- INPUT BINDING BASE INTERFACE -------------------------------- //

		virtual std::string getName() const = 0;

		/*
		* Some input events may be generated many times in a single frame (e.g. mouse delta), so they need to
		* be accumulated until the frame ends. This function will reset all these accumulators
		*/
		virtual void resetAccumulators() = 0;
	};
}