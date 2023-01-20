/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Engine.h"

#include "Log.h" // Log before Setup.h to calm down Windows.h redefinition warnings
#include "Setup.h" // Platform target, library selection, debug log and other configuration macros

#include "ExecutableSubsystemSet.h"
#include "DestructibleSubsystemSet.h"

#include <algorithm>

JFF::Engine::Engine() : 
	cache(),
	math(),
	io(),
	camera(),
	time(),
	physics(),
	input(),
	logic(),
	renderer(),
	context(),

	subsystems(),
	destructionSortedSubsystems(),
	executables(),

	state(EngineState::LOADING)
{
	JFF_LOG_SUPER_IMPORTANT("Initializing Engine...")

	// Add intermediate executable sets
	using Order = ExecutableSubsystem::ExecutionOrder;
	executables[Order::AFTER_TIME] = std::make_shared<ExecutableSubsystemSet>();
	executables[Order::AFTER_PHYSICS] = std::make_shared<ExecutableSubsystemSet>();
	executables[Order::AFTER_INPUT] = std::make_shared<ExecutableSubsystemSet>();
	executables[Order::AFTER_LOGIC] = std::make_shared<ExecutableSubsystemSet>();
	executables[Order::AFTER_RENDERER] = std::make_shared<ExecutableSubsystemSet>();

	// Add destructible subsystem set to hold custom subsystems
	using UnloadOrder = Subsystem::UnloadOrder;
	destructionSortedSubsystems[UnloadOrder::CUSTOM_SUBSYSTEM] = std::make_shared<DestructibleSubsystemSet>();
}

JFF::Engine::~Engine()
{
	JFF_LOG_SUPER_IMPORTANT("Exiting Engine...")

	// Clear subsystem and executables list (Does not delete subsystems yet)
	subsystems.clear();
	executables.clear();

	// Clear and effectively delete al subsystems in a concrete destruction order
	std::for_each(destructionSortedSubsystems.begin(), destructionSortedSubsystems.end(), [](auto& pair)
		{
			pair.second.reset();
		});
	destructionSortedSubsystems.clear();
}

void JFF::Engine::mainLoop()
{
	if (state != EngineState::RUNNING)
	{
		JFF_LOG_WARNING("Cannot run main loop: Incorrect engine state")
		return;
	}

	JFF_LOG_SUPER_IMPORTANT("Executing main loop...")
	bool keepExecutingMainLoop = true;
	while (keepExecutingMainLoop)
	{
		std::for_each(executables.begin(), executables.end(), [&keepExecutingMainLoop](auto& exec)
			{
				keepExecutingMainLoop = keepExecutingMainLoop && exec.second->execute();
			});
	}

	// Change engine state
	state = EngineState::EXITING;
}

inline void JFF::Engine::storeExecutableSubsystem(std::shared_ptr<ExecutableSubsystem>& ess)
{
	// Extract execution order and ensure it's a valid one
	ExecutableSubsystem::ExecutionOrder essOrder = ess->getExecutionOrder();
	if (essOrder == ExecutableSubsystem::ExecutionOrder::UNESPECIFIED || essOrder == ExecutableSubsystem::ExecutionOrder::SUBSYSTEM_SET)
	{
		JFF_LOG_ERROR("Invalid execution order for a ExecutableSubsystem")
		return;
	}

	auto iter = executables.find(essOrder);
	if (iter != executables.end())
	{
		// Found executable on the same slot. Need to check if it's a ExecutableSubsystemSet or not
		auto essSet = std::dynamic_pointer_cast<ExecutableSubsystemSet>(iter->second);
		if (essSet.get())
		{
			// Add the new executable subsystem to ExecutableSubsystemSet
			essSet->addExecutableSubsystem(ess);
		}
		else
		{
			JFF_LOG_WARNING("At least two subsystems share the same execution order. The new insertion will erase the previous one")
			executables[essOrder] = ess;
		}
	}
	else
	{
		executables[essOrder] = ess;
	}
}

inline void JFF::Engine::storeDestructibleSubsystem(const std::shared_ptr<Subsystem>& ss)
{
	using UOrder = Subsystem::UnloadOrder;

	// Extract subsystem's unload order and ensure it's a valid one
	 UOrder ssOrder = ss->getUnloadOrder();
	if (ssOrder == UOrder::UNESPECIFIED)
	{
		JFF_LOG_ERROR("Invalid unload order for a Subsystem")
		return;
	}

	auto iter = destructionSortedSubsystems.find(ssOrder);
	if (iter != destructionSortedSubsystems.end())
	{
		// Found destructible subsystem on the same slot. Need to check if it's a destructible subsystem set or not
		if (ssOrder == UOrder::CUSTOM_SUBSYSTEM)
		{
			auto ssSet = std::dynamic_pointer_cast<DestructibleSubsystemSet>(destructionSortedSubsystems[UOrder::CUSTOM_SUBSYSTEM]);
			ssSet->addDestructibleSubsystem(ss);
		}
		else
		{
			JFF_LOG_WARNING("At least two subsystems share the same destruction order. The new insertion will erase the previous one")
			destructionSortedSubsystems[ssOrder] = ss;
		}
	}
	else
	{
		destructionSortedSubsystems[ssOrder] = ss;
	}
}

inline void JFF::Engine::logWarning(const std::string& msg)
{
	JFF_LOG_WARNING(msg)
}

void JFF::Engine::initBasicSubsystems()
{
	if (state != EngineState::LOADING)
	{
		JFF_LOG_WARNING("Cannot init basic subsystems: Incorrect engine state")
		return;
	}

	JFF_LOG_SUPER_IMPORTANT("Loading basic subsystems...")

	// Context subsystem. Context must be #included before other subsystems in order to work properly
	if (context.expired())	attachSubsystem<Context>(createContextSubsystem());

	// Renderer subsystem
	if (renderer.expired())	attachSubsystem<Renderer>(createRendererSubsystem());

	// Cache subsystem
	if (cache.expired())	attachSubsystem<Cache>(createCacheSubsystem());

	// Time subsystem
	if (time.expired())		attachSubsystem<Time>(createTimeSubsystem());

	// Physics subsystem
	if (physics.expired())	attachSubsystem<Physics>(createPhysicsSubsystem());

	// Input subsystem
	if (input.expired())	attachSubsystem<Input>(createInputSubsystem());

	// Logic subsystem
	if (logic.expired())	attachSubsystem<Logic>(createLogicSubsystem());

	// IO subsystem
	if (io.expired())		attachSubsystem<IO>(createIOSubsystem());

	// Math subsystem
	if (math.expired())		attachSubsystem<Math>(createMathSubsystem());

	// Camera subsystem
	if (camera.expired())	attachSubsystem<Camera>(createCameraSubsystem());

	// Change engine state
	state = EngineState::POST_LOADING;
}

void JFF::Engine::postLoadSubsystems()
{
	if (state != EngineState::POST_LOADING)
	{
		JFF_LOG_WARNING("Cannot post loading subsystems: Incorrect engine state")
		return;
	}

	JFF_LOG_SUPER_IMPORTANT("Post-loading basic subsystems...")

	std::for_each(subsystems.begin(), subsystems.end(), [this](auto& pair) 
		{
			pair.second->postLoad(this);
		});

	// Change engine state
	state = EngineState::RUNNING;
}