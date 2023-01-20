/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "PhysicsBullet.h"

#include "Log.h"

JFF::PhysicsBullet::PhysicsBullet()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: Dummy PhysicsBullet")
}

JFF::PhysicsBullet::~PhysicsBullet()
{
	JFF_LOG_WARNING("Dtor subsystem: Dummy PhysicsBullet")
}

void JFF::PhysicsBullet::load()
{
	JFF_LOG_WARNING("Loading subsystem: Dummy PhysicsBullet")
}

void JFF::PhysicsBullet::postLoad(Engine* engine)
{
	JFF_LOG_WARNING("Post-loading subsystem: Dummy PhysicsBullet")
}

JFF::Subsystem::UnloadOrder JFF::PhysicsBullet::getUnloadOrder() const
{
	return UnloadOrder::PHYSICS;
}

JFF::ExecutableSubsystem::ExecutionOrder JFF::PhysicsBullet::getExecutionOrder() const
{
	return ExecutableSubsystem::ExecutionOrder::PHYSICS;
}

bool JFF::PhysicsBullet::execute()
{
	return true;
}
