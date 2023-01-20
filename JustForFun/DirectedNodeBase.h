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

namespace JFF
{
	template<typename _E>
	class DirectedNodeBase : public NodeBase<_E>
	{
	public:
		// Ctor & Dtor
		DirectedNodeBase();
		virtual ~DirectedNodeBase();

		// Copy ctor and copy assignment
		DirectedNodeBase(const DirectedNodeBase& other) = delete;
		DirectedNodeBase& operator=(const DirectedNodeBase& other) = delete;

		// Move ctor and assignment
		DirectedNodeBase(DirectedNodeBase&& other) = delete;
		DirectedNodeBase operator=(DirectedNodeBase&& other) = delete;

		// ------------------------------------ Overrides ------------------------------------ //

		// Connect with incoming edge. This node doesn't allow edge repetition. Expensive operation
		virtual void operator<<(const std::weak_ptr<_E>& edge) override;

		// Connect with outcoming edge. This node doesn't allow edge repetition. Expensive operation
		virtual void operator>>(const std::weak_ptr<_E>& edge) override;

		/*
		* Checks if this node is connected to another node through an edge.
		* Directionality is important here. Even if this function returns false, it could exist a edge that connects
		* from dstNode to this.
		*/ 
		virtual bool isConnectedTo(const std::weak_ptr<NodeBase<_E>>& dstNode) const override;

		// ------------------------------------ Directed node functions ------------------------------------ //

		// Visits all incoming edges in this node in no particular order and executes the visitor.
		void visitIncomingEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor);

		// Visits all outcoming edges in this node in no particular order and executes the visitor.
		void visitOutcomingEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor);

		// Get the incoming edge at specified index. This function does bounds checking
		std::weak_ptr<_E>& getIncomingEdge(unsigned int index);

		// Get the outcoming edge at specified index. This function does bounds checking
		std::weak_ptr<_E>& getOutcomingEdge(unsigned int index);

		// Gets the number of incoming edges connected to this node
		size_t numIncomingEdges() const { return incomingEdges.size(); }

		// Gets the number of outcoming edges connected to this node
		size_t numOutcomingEdges() const { return outcomingEdges.size(); }

	protected:
		std::vector<std::weak_ptr<_E>> incomingEdges;
		std::vector<std::weak_ptr<_E>> outcomingEdges;
	};
}

// Include inline definitions
#include "DirectedNodeBase.inl"