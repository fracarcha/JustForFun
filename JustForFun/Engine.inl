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

#include <typeinfo>
#include <type_traits>

template<typename T, typename Y>
void JFF::Engine::attachSubsystem(std::shared_ptr<Y>&& subsystem)
{
	// Ensure T implements Subsystem
	static_assert(std::is_base_of_v<T, Y>, "Couldn't attach subsystem. The provided subsystem doesn't inherit from T");
	static_assert(std::is_base_of_v<Subsystem, T>, "Couldn't attach subsystem. The provided class doesn't implement Subsystem abstract class");

	if (state != EngineState::LOADING)
	{
		logWarning("Cannot attach new subsystems if main loop is under execution");
		return;
	}

	// Add it to subsystem map
	std::string ssName = typeid(T).name();
	auto iter = subsystems.find(ssName);
	if (iter != subsystems.end())
	{
		logWarning("At least two subsystems share the same name. The new insertion will erase the previous one");
	}
	subsystems[ssName] = subsystem;

	// Add it to destruction subsystem map
	storeDestructibleSubsystem(subsystem);

	// Add it to executable list if ss is ExecutableSubsystem
	std::shared_ptr<ExecutableSubsystem> pExeSub = std::dynamic_pointer_cast<ExecutableSubsystem>(subsystem);
	if (pExeSub.get())
	{
		storeExecutableSubsystem(pExeSub);
	}

	// Init direct access to basic subsystems
	if (std::dynamic_pointer_cast<Time>(subsystem).get())			time = getSubsystem<Time>();
	else if (std::dynamic_pointer_cast<Physics>(subsystem).get())	physics = getSubsystem<Physics>();
	else if (std::dynamic_pointer_cast<Input>(subsystem).get())		input = getSubsystem<Input>();
	else if (std::dynamic_pointer_cast<Logic>(subsystem).get())		logic = getSubsystem<Logic>();
	else if (std::dynamic_pointer_cast<Renderer>(subsystem).get())	renderer = getSubsystem<Renderer>();
	else if (std::dynamic_pointer_cast<Context>(subsystem).get())	context = getSubsystem<Context>();
	else if (std::dynamic_pointer_cast<IO>(subsystem).get())		io = getSubsystem<IO>();
	else if (std::dynamic_pointer_cast<Math>(subsystem).get())		math = getSubsystem<Math>();
	else if (std::dynamic_pointer_cast<Camera>(subsystem).get())	camera = getSubsystem<Camera>();
	else if (std::dynamic_pointer_cast<Cache>(subsystem).get())		cache = getSubsystem<Cache>();

	// Load subsystem
	subsystem->load();
}

template<typename T>
std::weak_ptr<T> JFF::Engine::getSubsystem()
{
	// Ensure T implements Subsystem
	static_assert(std::is_base_of_v<Subsystem, T>, "Couldn't get subsystem. The provided class doesn't implement Subsystem abstract class");

	std::weak_ptr<T> pSubsystem;

	std::string subsystemName = typeid(T).name();
	auto iter = subsystems.find(subsystemName);
	if (iter != subsystems.end())
	{
		std::shared_ptr<Subsystem> pSub = (*iter).second; // Creates a weak_ptr from shared_ptr
		std::shared_ptr<T> pT = std::dynamic_pointer_cast<T>(pSub);
		if (pT.get())
		{
			pSubsystem = pT;
		}
	}
	else
	{
		logWarning("Couldn't find subsystem");
	}

	return pSubsystem;
}