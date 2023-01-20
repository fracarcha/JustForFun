/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Node.h"

//#define JFF_SUPRESS_INFO_LOGS_HERE
#include "Log.h"

#include <algorithm>

JFF::Node::Node()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor Node")
}

JFF::Node::~Node()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor Node")
}

bool JFF::Node::isConnectedTo(const std::weak_ptr<NodeBase>& other) const
{
	auto otherHandler = other.lock();

	// Check if the other node is valid
	if (!otherHandler.get())
		return false;

	// This node is not connected to itself
	if (*this == other)
		return false;

	// Build a predicate to check if there is an edge that connects both nodes
	auto predicate = [&otherHandler](const std::weak_ptr<EdgeBase<Node>>& e)
	{
		auto eHandler = e.lock();
		return (*otherHandler == eHandler->getDstNode()) || (*otherHandler == eHandler->getSrcNode());
	};

	// Execute find
	return std::find_if(edges.begin(), edges.end(), predicate) != edges.end();
}
