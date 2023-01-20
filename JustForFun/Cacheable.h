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

#include <string>

namespace JFF
{
	class Cacheable
	{
	public:
		// Ctor & Dtor
		Cacheable() {}
		virtual ~Cacheable() {}

		// Copy ctor and copy assignment
		Cacheable(const Cacheable& other) = delete;
		Cacheable& operator=(const Cacheable& other) = delete;

		// Move ctor and assignment
		Cacheable(Cacheable&& other) = delete;
		Cacheable operator=(Cacheable&& other) = delete;

		// Cacheable interface
		virtual std::string getCacheName() const = 0;
	};
}