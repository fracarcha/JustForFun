/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "GraphBase.h"

#include "Log.h"
#include <algorithm>

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::addNodeConnected(const std::shared_ptr<_N>& inGraphNode, const std::shared_ptr<_N>& newNode)
{
	if (!assertInGraphNode(inGraphNode, "Cannot connect a new node to existing one. The existing one is not part of the graph"))
		return false;
	if (!assertValidNode(newNode, "Try adding an invalid node to the graph. Aborted."))
		return false;
	if (!assertUniqueNode(newNode, "Try adding a node that is already in the graph. Aborted."))
		return false;

	// Add the new node into the graph
	nodes.push_back(newNode);

	// Create a new edge between the two nodes
	std::shared_ptr<_E> newEdge = std::make_shared<_E>(inGraphNode, newNode);
	edges.push_back(newEdge);

	// Register the edges in nodes
	*inGraphNode >> newEdge;
	*newNode << newEdge;

	return true;
}

template<typename _N, typename _E>
inline void JFF::GraphBase<_N, _E>::visitNodes(const std::function<void(const std::weak_ptr<_N>&)>& visitor)
{
	std::for_each(nodes.begin(), nodes.end(), visitor);
}

template<typename _N, typename _E>
inline void JFF::GraphBase<_N, _E>::visitEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor)
{
	std::for_each(edges.begin(), edges.end(), visitor);
}

template<typename _N, typename _E>
template<typename _G, typename _RetVal>
inline _RetVal JFF::GraphBase<_N, _E>::visitFromRoot(const std::weak_ptr<GraphAlgorithm<_G, _N, _E, _RetVal>>& visitor)
{
	if (!rootNode.lock().get())
	{
		JFF_LOG_WARNING("Cannot visit root node. There isn't a root node selected.")
		return;
	}
	return visitor.lock()->operator()(rootNode);
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::isEmpty() const
{
	return nodes.empty();
}

// -------------------------------------- Helper functions ---------------------------------------------- //

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::assertValidNode(const std::shared_ptr<_N>& n, const char* errorMsg) const
{
	if (!n.get())
	{
		JFF_LOG_WARNING(errorMsg)
			return false;
	}
	return true;
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::assertValidEdge(const std::shared_ptr<_E>& e, const char* errorMsg) const
{
	// Ensure the edge pointer is valid
	if (!(e.get() && e->getSrcNode().lock().get() && e->getDstNode().lock().get()))
	{
		JFF_LOG_WARNING(errorMsg)
		return false;
	}
	return true;
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::assertUniqueNode(const std::shared_ptr<_N>& n, const char* errorMsg) const
{
	auto predicate = [&n](const std::weak_ptr<_N>& node) { return *n == node; };
	if (std::find_if(nodes.begin(), nodes.end(), predicate) != nodes.end())
	{
		JFF_LOG_WARNING(errorMsg)
		return false;
	}
	return true;
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::assertUniqueEdge(const std::shared_ptr<_E>& e, const char* errorMsg) const
{
	auto predicate = [&e](const std::weak_ptr<_E>& edge) { return *e == edge; };
	if (std::find_if(edges.begin(), edges.end(), predicate) != edges.end())
	{
		JFF_LOG_WARNING(errorMsg)
		return false;
	}
	return true;
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::assertInGraphNode(const std::shared_ptr<_N>& n, const char* errorMsg) const
{
	auto predicate = [&n](const std::weak_ptr<_N>& node) { return *n == node; };
	if (std::find_if(nodes.begin(), nodes.end(), predicate) == nodes.end())
	{
		JFF_LOG_WARNING(errorMsg)
		return false;
	}
	return true;
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::assertEqualNodes(const std::shared_ptr<_N>& nA, const std::shared_ptr<_N>& nB, const char* errorMsg) const
{
	if (*nA == nB)
	{
		JFF_LOG_WARNING(errorMsg)
		return true;
	}
	return false;
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::assertConnectedNodes(const std::shared_ptr<_N>& nA, const std::shared_ptr<_N>& nB, const char* errorMsg) const
{
	if (nA->isConnectedTo(nB))
	{
		JFF_LOG_WARNING(errorMsg)
		return true;
	}
	return false;
}

template<typename _N, typename _E>
inline bool JFF::GraphBase<_N, _E>::setNodeAsRoot(const std::weak_ptr<_N>& n)
{
	if (!assertInGraphNode(n.lock(), "Cannot set a root node. Provided node is not part of the graph"))
		return false;

	rootNode = n;
	return true;
}
