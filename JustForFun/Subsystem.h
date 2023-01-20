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

// STL
#include <string>

namespace JFF
{
	class Engine;

	class Subsystem
	{
	public:
		enum class UnloadOrder : char
		{
			UNESPECIFIED = -1,

			LOGIC = 0,
			CUSTOM_SUBSYSTEM = 1,

			PHYSICS,
			RENDERER,
			CAMERA,
			INPUT,
			IO,
			MATH,
			TIME,
			CACHE,
			CONTEXT,
		};

		// Ctor & Dtor
		Subsystem() {}
		virtual ~Subsystem() {}

		// Copy ctor and copy assignment
		Subsystem(const Subsystem& other) = delete;
		Subsystem& operator=(const Subsystem& other) = delete;

		// Move ctor and assignment
		Subsystem(Subsystem&& other) = delete;
		Subsystem operator=(Subsystem&& other) = delete;

		// This is called before main loop. Don't assume that other subsystems are loaded yet
		virtual void load() = 0;

		// This is called before main loop and after load(). You can use engine and its subsystems in this function
		virtual void postLoad(Engine* engine) = 0;

		// Gets the unload order of this subsystems
		virtual UnloadOrder getUnloadOrder() const = 0;
	};
}