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

#include "ExecutableSubsystem.h"

namespace JFF
{
	class Physics : public ExecutableSubsystem
	{
	public:
		// Ctor & dtor
		Physics() {}
		virtual ~Physics() {}

		// Copy ctor and copy assignment
		Physics(const Physics& other) = delete;
		Physics& operator=(const Physics& other) = delete;

		// Move ctor and assignment
		Physics(Physics&& other) = delete;
		Physics operator=(Physics&& other) = delete;

		// Physics functions
		// TODO: Define Physics functions
	};
}