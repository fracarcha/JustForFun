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

#include "Framebuffer.h"
#include <memory>

namespace JFF
{
	// Class used to launch a single draw call only one time per engine execution and store result textures on disk
	class Preprocess
	{
	public:
		// Ctor & Dtor
		Preprocess() {}
		virtual ~Preprocess() {}

		// Copy ctor and copy assignment
		Preprocess(const Preprocess& other) = delete;
		Preprocess& operator=(const Preprocess& other) = delete;

		// Move ctor and assignment
		Preprocess(Preprocess&& other) = delete;
		Preprocess operator=(Preprocess&& other) = delete;

		// ---------------------- POST PROCESS FX INTERFACE ---------------------- //

		// Execute the preprocessing effect
		virtual void execute() = 0;
	};
}