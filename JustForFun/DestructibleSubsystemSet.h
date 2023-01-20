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
#include <memory>
#include <vector>

namespace JFF
{
	// This class serves to hold references to custom subsystems until it's destroyed
	class DestructibleSubsystemSet final : public Subsystem
	{
	public:
		// Ctor & Dtor
		DestructibleSubsystemSet() {}
		virtual ~DestructibleSubsystemSet() {}

		// Copy ctor and copy assignment
		DestructibleSubsystemSet(const DestructibleSubsystemSet& other) = delete;
		DestructibleSubsystemSet& operator=(const DestructibleSubsystemSet& other) = delete;

		// Move ctor and assignment
		DestructibleSubsystemSet(DestructibleSubsystemSet&& other) = delete;
		DestructibleSubsystemSet operator=(DestructibleSubsystemSet&& other) = delete;

		// Subsystem implementation
		virtual void load() override {} // Never used
		virtual void postLoad(Engine* engine) override {} // Never used
		virtual UnloadOrder getUnloadOrder() const override { return UnloadOrder::UNESPECIFIED; } // Never used

		// Destructible subsystem functions
		void addDestructibleSubsystem(const std::shared_ptr<Subsystem>& subsystem) { destructibleSubsystems.push_back(subsystem); }

	private:
		std::vector<std::shared_ptr<Subsystem>> destructibleSubsystems;
	};
}