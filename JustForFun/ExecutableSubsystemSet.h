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

#include <vector>
#include <memory>
#include <algorithm>

namespace JFF
{
	class ExecutableSubsystemSet final : public ExecutableSubsystem
	{
	public:
		// Ctor & Dtor
		ExecutableSubsystemSet() {}
		virtual ~ExecutableSubsystemSet() {}

		// Copy ctor and copy assignment
		ExecutableSubsystemSet(const ExecutableSubsystemSet& other) = delete;
		ExecutableSubsystemSet& operator=(const ExecutableSubsystemSet& other) = delete;

		// Move ctor and assignment
		ExecutableSubsystemSet(ExecutableSubsystemSet&& other) = delete;
		ExecutableSubsystemSet operator=(ExecutableSubsystemSet&& other) = delete;

		// Subsystem implementation
		virtual void load() override {} // Never used
		virtual void postLoad(Engine* engine) override {} // Never used
		virtual UnloadOrder getUnloadOrder() const override { return UnloadOrder::UNESPECIFIED; } // Never used

		// ExecutableSubsystem implementation
		virtual ExecutionOrder getExecutionOrder() const override { return ExecutableSubsystem::ExecutionOrder::SUBSYSTEM_SET; }
		virtual bool execute()
		{
			bool mainLoopExecution = true;
			std::for_each(subsystemSet.begin(), subsystemSet.end(), [&mainLoopExecution](auto& ss)
				{
					mainLoopExecution = mainLoopExecution && ss->execute();
				});
			return mainLoopExecution;
		}

		// Add ExecutableSubsystem to this subset
		void addExecutableSubsystem(std::shared_ptr<ExecutableSubsystem>& ess) { subsystemSet.push_back(ess); }

	private:
		std::vector<std::shared_ptr<ExecutableSubsystem>> subsystemSet;
	};
}