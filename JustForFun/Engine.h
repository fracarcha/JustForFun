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

// Core subsystem functionality
#include "ExecutableSubsystem.h"

// Basic subsystems
#include "Time.h"
#include "Physics.h"
#include "Input.h"
#include "Logic.h"
#include "Renderer.h"
#include "Context.h"
#include "IO.h"
#include "Math.h"
#include "Camera.h"
#include "Cache.h"

// STL
#include <string>
#include <memory>
#include <map>

namespace JFF
{
	class Engine final
	{
	public:
		// Ctors & dtors
		Engine();
		~Engine();

		// Copy ctor and copy assignment
		Engine(const Engine& other) = delete;
		Engine& operator=(const Engine& other) = delete;

		// Move ctor and assignment
		Engine(Engine&& other) = delete;
		Engine operator=(Engine&& other) = delete;

		/*	
		*	Adds a new subsystem module to add functionality to the engine.
		*	This must be called before the main loop is executing.
		*	Y should inherit from T 
		*	T should inherit from Subsystem abstract class
		*/
		template<typename T, typename Y>
		void attachSubsystem(std::shared_ptr<Y>&& subsystem);

		template<typename T>
		std::weak_ptr<T> getSubsystem();

		// Inits basic subsystems that haven't been initialized yet by attachSubsystem()
		void initBasicSubsystems();

		// Call postLoad on all subsystems
		void postLoadSubsystems();

		// Execute main loop
		void mainLoop();

	protected:
		inline void storeExecutableSubsystem(std::shared_ptr<ExecutableSubsystem>& ess);
		inline void storeDestructibleSubsystem(const std::shared_ptr<Subsystem>& ss);
		inline void logWarning(const std::string& msg); // Walkaround to log in Engine.inl

	public: // Public attributes
		// Direct access to basic subsystems
		std::weak_ptr<Cache> cache;
		std::weak_ptr<Math> math;
		std::weak_ptr<IO> io;
		std::weak_ptr<Camera> camera;
		std::weak_ptr<Time> time;
		std::weak_ptr<Physics> physics;
		std::weak_ptr<Input> input;
		std::weak_ptr<Logic> logic;
		std::weak_ptr<Renderer> renderer;
		std::weak_ptr<Context> context;

	protected: // Private attributes
		// Maps all subsystems with their names
		std::map<std::string, std::shared_ptr<Subsystem>> subsystems;

		// Maps all subsystems sorted by destruction order
		std::map<Subsystem::UnloadOrder, std::shared_ptr<Subsystem>> destructionSortedSubsystems;

		// Sorted list of executable subsystems
		std::map<ExecutableSubsystem::ExecutionOrder, std::shared_ptr<ExecutableSubsystem>> executables;

		// Main loop execution phase
		enum class EngineState : char
		{
			LOADING,
			POST_LOADING,
			RUNNING,
			EXITING,
		} state;
	};
}

// Include template definitions
#include "Engine.inl" 