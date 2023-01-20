/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "DirectedNodeBase.h"

#include "Log.h"
#include <algorithm>
#include <stdexcept>

template<typename _E>
JFF::DirectedNodeBase<_E>::DirectedNodeBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor DirectedNodeBase")
}

template<typename _E>
JFF::DirectedNodeBase<_E>::~DirectedNodeBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor DirectedNodeBase")
}

template<typename _E>
void JFF::DirectedNodeBase<_E>::operator<<(const std::weak_ptr<_E>& edge)
{
	if (this->addUniqueEdge(edge))
		incomingEdges.push_back(edge);
}

template<typename _E>
void JFF::DirectedNodeBase<_E>::operator>>(const std::weak_ptr<_E>& edge)
{
	if (this->addUniqueEdge(edge))
		outcomingEdges.push_back(edge);
}

template<typename _E>
bool JFF::DirectedNodeBase<_E>::isConnectedTo(const std::weak_ptr<JFF::NodeBase<_E>>& dstNode) const
{
	auto dstNodeHandler = dstNode.lock();

	// Check if the other node is valid
	if (!dstNodeHandler.get())
		return false;

	// This node is not connected to itself
	if (*this == dstNode)
		return false;

	// Build a predicate to check if there is an edge that goes from this to dstNode
	auto predicate = [&dstNodeHandler](const std::weak_ptr<_E>& edge)
	{
		auto edgeDstEnd = edge.lock()->getDstNode();
		return *dstNodeHandler == edgeDstEnd;
	};

	// Execute find
	return std::find_if(outcomingEdges.begin(), outcomingEdges.end(), predicate) != outcomingEdges.end();
}

template<typename _E>
void JFF::DirectedNodeBase<_E>::visitIncomingEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor)
{
	std::for_each(incomingEdges.begin(), incomingEdges.end(), visitor);
}

template<typename _E>
void JFF::DirectedNodeBase<_E>::visitOutcomingEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor)
{
	std::for_each(outcomingEdges.begin(), outcomingEdges.end(), visitor);
}

template<typename _E>
inline std::weak_ptr<_E>& JFF::DirectedNodeBase<_E>::getIncomingEdge(unsigned int index)
{
	if (index >= incomingEdges.size())
	{
		JFF_LOG_ERROR("Attempting to access an incoming edge at invalid index. Aborting")
		throw std::out_of_range("Attempting to access an incoming edge at invalid index.");
	}

	return incomingEdges[index];
}

template<typename _E>
inline std::weak_ptr<_E>& JFF::DirectedNodeBase<_E>::getOutcomingEdge(unsigned int index)
{
	if (index >= outcomingEdges.size())
	{
		JFF_LOG_ERROR("Attempting to access an outcoming edge at invalid index. Aborting")
		throw std::out_of_range("Attempting to access an outcoming edge at invalid index.");
	}

	return outcomingEdges[index];
}
