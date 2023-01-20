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

#include "Time.h"

namespace JFF
{
	// Standard implementation of Time subsystem
	class TimeSTD : public Time
	{
	public:
		// Ctors & dtors
		TimeSTD();
		virtual ~TimeSTD();

		// Copy ctor and copy assignment
		TimeSTD(const TimeSTD& other) = delete;
		TimeSTD& operator=(const TimeSTD& other) = delete;

		// Move ctor and assignment
		TimeSTD(TimeSTD&& other) = delete;
		TimeSTD operator=(TimeSTD&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ExecutableSubsystem impl
		virtual ExecutableSubsystem::ExecutionOrder getExecutionOrder() const override;
		virtual bool execute() override;

		// Time impl
		virtual double deltaTime() const override;

	protected:
		double delta;
	};
}