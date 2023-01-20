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
#include "Subsystem.h"

namespace JFF
{
	class ExecutableSubsystem : public Subsystem
	{
	public:
		enum class ExecutionOrder : char
		{
			UNESPECIFIED = -1,

			SUBSYSTEM_SET = 0,

			TIME = 1,
			AFTER_TIME,
			BEFORE_PHISICS = AFTER_TIME,
			PHYSICS,
			AFTER_PHYSICS,
			BEFORE_INPUT = AFTER_PHYSICS,
			INPUT,
			AFTER_INPUT,
			BEFORE_LOGIC = AFTER_INPUT,
			LOGIC,
			AFTER_LOGIC,
			BEFORE_RENDERER = AFTER_LOGIC,
			RENDERER,
			AFTER_RENDERER,
			BEFORE_CONTEXT = AFTER_RENDERER,
			CONTEXT,
		};

		// Ctor & Dtor
		ExecutableSubsystem() {}
		virtual ~ExecutableSubsystem() {}

		// Copy ctor and copy assignment
		ExecutableSubsystem(const ExecutableSubsystem& other) = delete;
		ExecutableSubsystem& operator=(const ExecutableSubsystem& other) = delete;

		// Move ctor and assignment
		ExecutableSubsystem(ExecutableSubsystem&& other) = delete;
		ExecutableSubsystem operator=(ExecutableSubsystem&& other) = delete;

		/** Determines in which order execute() is called among other subsystems
		*	@param order:	The execution order with respect fixed subsystems. If fixed subsystem is 
		*					selected, this will replace the previous subsystem in that position
		*/
		virtual ExecutionOrder getExecutionOrder() const = 0;

		/*	
		*	This executes every frame in order given by setExecutionOrder()
		*	Returns false to signal the main loop to stop
		*/
		virtual bool execute() = 0;
	};
}