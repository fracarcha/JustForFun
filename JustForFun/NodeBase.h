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

#include <vector>
#include <functional>
#include <memory>

namespace JFF
{
	template<typename _E>
	class NodeBase
	{
	public:
		// Ctor & Dtor
		NodeBase();
		virtual ~NodeBase();

		// Copy ctor and copy assignment
		NodeBase(const NodeBase& other) = delete;
		NodeBase& operator=(const NodeBase& other) = delete;

		// Move ctor and assignment
		NodeBase(NodeBase&& other) = delete;
		NodeBase operator=(NodeBase&& other) = delete;

		// -------------------------------------------- Virtual methods -------------------------------------------- //

		// Connect with edge. This node doesn't allow edge repetition. Expensive operation
		virtual void operator<<(const std::weak_ptr<_E>& edge);
		virtual void operator>>(const std::weak_ptr<_E>& edge);

		// Checks if this node is connected to another node through an edge
		virtual bool isConnectedTo(const std::weak_ptr<NodeBase>& other) const = 0;

		// Returns true when input Node is exactly the same (shares memory) than 'this'
		virtual bool operator==(const std::weak_ptr<NodeBase>& other) const;

		// -------------------------------------------- Non virtual methods -------------------------------------------- //

		// Visits all edges in this node in no particular order and executes the visitor.
		void visitEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor);

		// Get the edge at specified index. This function does bounds checking
		std::weak_ptr<_E>& operator[](unsigned int index);

		// Gets the number of edges connected to this node
		size_t numEdges() const { return edges.size(); }

	protected:
		inline bool addUniqueEdge(const std::weak_ptr<_E>& edge);

	protected:
		std::vector<std::weak_ptr<_E>> edges;
	};
}

// Include inline definitions
#include "NodeBase.inl"