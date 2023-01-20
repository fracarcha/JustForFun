/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "NodeBase.h"

#include "Log.h"
#include <algorithm>

template<typename _E>
JFF::NodeBase<_E>::NodeBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor NodeBase")
}

template<typename _E>
JFF::NodeBase<_E>::~NodeBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor NodeBase")
}

template<typename _E>
void JFF::NodeBase<_E>::operator<<(const std::weak_ptr<_E>& edge)
{
	addUniqueEdge(edge);
}

template<typename _E>
void JFF::NodeBase<_E>::operator>>(const std::weak_ptr<_E>& edge)
{
	addUniqueEdge(edge);
}

template<typename _E>
bool JFF::NodeBase<_E>::operator==(const std::weak_ptr<NodeBase>& other) const
{
	return this == other.lock().get(); // TODO: is this a good idea ?
}

template<typename _E>
void JFF::NodeBase<_E>::visitEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor)
{
	std::for_each(edges.begin(), edges.end(), visitor);
}

template<typename _E>
inline std::weak_ptr<_E>& JFF::NodeBase<_E>::operator[](unsigned int index)
{
	if (index >= edges.size())
	{
		JFF_LOG_WARNING("Attempting to access an edge at invalid index. Aborting")
		return {};
	}

	return edges[index];
}

template<typename _E>
inline bool JFF::NodeBase<_E>::addUniqueEdge(const std::weak_ptr<_E>& edge)
{
	// NOTE: This function was separated from operator<< to make it useful for children of this class

	auto predicate = [&edge](const std::weak_ptr<_E>& e) { return *(edge.lock()) == e; };
	if (std::find_if(edges.begin(), edges.end(), predicate) != edges.end())
	{
		JFF_LOG_WARNING("Edge is already connected to node. Operation aborted.")
		return false;
	}
	else
	{
		edges.push_back(edge);
		return true;
	}
}
