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

#include "Cacheable.h"
#include <memory>

namespace JFF
{
	class Cache : public Subsystem
	{
	public:
		// Ctor & Dtor
		Cache() {}
		virtual ~Cache() {}

		// Copy ctor and copy assignment
		Cache(const Cache& other) = delete;
		Cache& operator=(const Cache& other) = delete;

		// Move ctor and assignment
		Cache(Cache&& other) = delete;
		Cache operator=(Cache&& other) = delete;

		// ------------------- CACHE INTERFACE ------------------- //

		/*
		* Adds a new item to the cache. This function will hold a shared pointer to cached item  
		* until removeCacheItem() is called.
		* This is used to save memory/VRAM and CPU/GPU computation time by avoiding repeated objects
		* to be loaded multiple times.
		*/
		virtual void addCacheItem(const std::shared_ptr<Cacheable>& cacheItem) = 0;
		
		/*
		* Removes an item from cache given its cache name.
		*/
		virtual void removeCacheItem(const std::string& cacheItemName) = 0;

		/*
		* Clear all cached items, effectively destroying the cacheable objects if they aren't referenced anymore
		*/
		virtual void clearCache() = 0;

		/*
		* Returns a pointer to the cached item given its cache name. Be careful trying to change this object
		* because it's shared with other owners
		*/
		virtual std::shared_ptr<Cacheable> getCachedItem(const std::string& cachedItemName) = 0;
	};
}