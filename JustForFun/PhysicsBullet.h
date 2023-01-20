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

#include "Physics.h"

namespace JFF
{
	// Physics implementation using Bullet
	class PhysicsBullet : public Physics
	{
	public:
		// Ctor & Dtor
		PhysicsBullet();
		virtual ~PhysicsBullet();

		// Copy ctor and copy assignment
		PhysicsBullet(const PhysicsBullet& other) = delete;
		PhysicsBullet& operator=(const PhysicsBullet& other) = delete;

		// Move ctor and assignment
		PhysicsBullet(PhysicsBullet&& other) = delete;
		PhysicsBullet operator=(PhysicsBullet&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ExecutableSubsystem impl
		virtual ExecutableSubsystem::ExecutionOrder getExecutionOrder() const override;
		virtual bool execute() override;

		// Physics impl
		// TODO: Physics impl
	};
}