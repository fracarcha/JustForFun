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

#include "NodeBase.h"
#include "EdgeBase.h"

namespace JFF
{
	class Node : public NodeBase<EdgeBase<Node>>
	{
	public:
		// Ctor & Dtor
		Node();
		virtual ~Node();

		// Copy ctor and copy assignment
		Node(const Node& other) = delete;
		Node& operator=(const Node& other) = delete;

		// Move ctor and assignment
		Node(Node&& other) = delete;
		Node operator=(Node&& other) = delete;

		// Checks if this node is connected to another node through an edge
		virtual bool isConnectedTo(const std::weak_ptr<NodeBase>& other) const override;
	};
}