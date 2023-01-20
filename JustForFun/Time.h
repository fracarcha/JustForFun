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
	class Time : public ExecutableSubsystem
	{
	public:
		// Ctor & Dtor
		Time(){}
		virtual ~Time(){}

		// Copy ctor and copy assignment
		Time(const Time& other) = delete;
		Time& operator=(const Time& other) = delete;

		// Move ctor and assignment
		Time(Time&& other) = delete;
		Time operator=(Time&& other) = delete;

		// Time interface
		virtual double deltaTime() const = 0;
	};
}