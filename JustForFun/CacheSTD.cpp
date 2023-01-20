/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "CacheSTD.h"

#include "Log.h"

JFF::CacheSTD::CacheSTD() :
	cachedItems()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: CacheSTD")
}

JFF::CacheSTD::~CacheSTD()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: CacheSTD")
}

void JFF::CacheSTD::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: CacheSTD")
}

void JFF::CacheSTD::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: CacheSTD")
}

JFF::Subsystem::UnloadOrder JFF::CacheSTD::getUnloadOrder() const
{
	return UnloadOrder::CACHE;
}

void JFF::CacheSTD::addCacheItem(const std::shared_ptr<Cacheable>& cacheItem)
{
	// Find if item is already cached using cacheable name
	// NOTE: Careful! Unique naming in cacheables is important to keep a valid cache
	std::string cacheItemName = cacheItem->getCacheName();
	auto iter = cachedItems.find(cacheItemName);
	if (iter != cachedItems.end())
	{
		JFF_LOG_WARNING("A cached item with name " << cacheItemName << " already exist. Caching aborted")
		return;
	}

	cachedItems[cacheItemName] = cacheItem;
}

void JFF::CacheSTD::removeCacheItem(const std::string& cacheItemName)
{
	// Find if item is already cached using cacheable name
	// NOTE: Careful! Unique naming in cacheables is important to keep a valid cache
	auto iter = cachedItems.find(cacheItemName);
	if (iter == cachedItems.end())
	{
		JFF_LOG_WARNING("There is no cached items with name " << cacheItemName << ". Un-caching aborted")
		return;
	}

	cachedItems.erase(cacheItemName);
}

void JFF::CacheSTD::clearCache()
{
	cachedItems.clear();
}

std::shared_ptr<JFF::Cacheable> JFF::CacheSTD::getCachedItem(const std::string& cachedItemName)
{
	// Find if item is already cached using cacheable name
	auto iter = cachedItems.find(cachedItemName);
	if (iter != cachedItems.end())
	{
		return cachedItems[cachedItemName];
	}
	else
	{
		return nullptr;
	}
}
