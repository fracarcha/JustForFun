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

#include "Cache.h"
#include <map>
#include <string>

namespace JFF
{
	class CacheSTD : public Cache
	{
	public:
		// Ctor & Dtor
		CacheSTD();
		virtual ~CacheSTD();

		// Copy ctor and copy assignment
		CacheSTD(const CacheSTD& other) = delete;
		CacheSTD& operator=(const CacheSTD& other) = delete;

		// Move ctor and assignment
		CacheSTD(CacheSTD&& other) = delete;
		CacheSTD operator=(CacheSTD&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ------------------- CACHE INTERFACE ------------------- //

		virtual void addCacheItem(const std::shared_ptr<Cacheable>& cacheItem) override;
		virtual void removeCacheItem(const std::string& cacheItemName) override;
		virtual void clearCache() override;
		virtual std::shared_ptr<Cacheable> getCachedItem(const std::string& cachedItemName) override;

	protected:
		// Key: cachedItemName | Value: cacheable object
		std::map<std::string, std::shared_ptr<Cacheable>> cachedItems;
	};
}