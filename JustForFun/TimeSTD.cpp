/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "TimeSTD.h"

#include "Log.h"

#include <chrono>

JFF::TimeSTD::TimeSTD() :
	delta(0.0)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: TimeSTD")
}

JFF::TimeSTD::~TimeSTD()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: TimeSTD")
}

void JFF::TimeSTD::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: TimeSTD")
}

void JFF::TimeSTD::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: TimeSTD")
}

JFF::Subsystem::UnloadOrder JFF::TimeSTD::getUnloadOrder() const
{
	return UnloadOrder::TIME;
}

JFF::ExecutableSubsystem::ExecutionOrder JFF::TimeSTD::getExecutionOrder() const
{
	return ExecutableSubsystem::ExecutionOrder::TIME;
}

bool JFF::TimeSTD::execute()
{
	static auto lastFrameTime = std::chrono::steady_clock::now(); // Static initialization is called only once
	auto currentFrameTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	delta = deltaTime.count();

	return true; // Signal main loop to continue
}

double JFF::TimeSTD::deltaTime() const
{
	return delta;
}