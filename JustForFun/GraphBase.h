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

#include "GraphAlgorithm.h"

#include <vector>
#include <functional>
#include <memory>

namespace JFF
{
	template<typename _N, typename _E>
	class GraphBase
	{
	public:
		// Ctor & Dtor
		GraphBase() {}
		virtual ~GraphBase() {}

		// Copy ctor and copy assignment
		GraphBase(const GraphBase& other) = delete;
		GraphBase& operator=(const GraphBase& other) = delete;

		// Move ctor and assignment
		GraphBase(GraphBase&& other) = delete;
		GraphBase operator=(GraphBase&& other) = delete;

		/*
		* Adds a new node to the graph. If graph was empty, sets this as root node. The node won't be added if:
		*	* n was already included in the graph
		*	* n is empty
		*	* @return true if node was successfully added. False otherwise
		*/
		virtual bool addNode(const std::shared_ptr<_N>& n) = 0;

		/*
		* Adds a new node to the graph and connect it with an existent node creating a new edge.
		* The entire operation will be cancelled if:
		*	* inGraphNode is not part of the graph before this function call
		*	* newNode was already included in the graph
		*	* newNode is empty
		*	* @return true if node and edge was successfully added. False otherwise
		*/
		virtual bool addNodeConnected(const std::shared_ptr<_N>& inGraphNode, const std::shared_ptr<_N>& newNode);

		/*
		* Visits all nodes in graph in no particular order and executes the visitor.
		* Graph connectivity isn't taken into account in this function.
		*/
		virtual void visitNodes(const std::function<void(const std::weak_ptr<_N>&)>& visitor);

		/*
		* Visits all edges in graph in no particular order and executes the visitor.
		* Graph connectivity isn't taken into account in this function.
		*/
		virtual void visitEdges(const std::function<void(const std::weak_ptr<_E>&)>& visitor);

		/*
		* Visits the root node. Is up to the visitor to decide how to visit other nodes using graph connectivity
		*/
		template<typename _G, typename _RetVal>
		_RetVal visitFromRoot(const std::weak_ptr<GraphAlgorithm<_G, _N, _E, _RetVal>>& visitor);

		// Return true if the graph doesn't have nodes
		virtual bool isEmpty() const;

	protected: // Helper functions
		inline bool assertValidNode(const std::shared_ptr<_N>& n, const char* errorMsg) const;
		inline bool assertValidEdge(const std::shared_ptr<_E>& e, const char* errorMsg) const;

		inline bool assertUniqueNode(const std::shared_ptr<_N>& n, const char* errorMsg) const;
		inline bool assertUniqueEdge(const std::shared_ptr<_E>& e, const char* errorMsg) const;

		inline bool assertInGraphNode(const std::shared_ptr<_N>& n, const char* errorMsg) const;
		inline bool assertEqualNodes(const std::shared_ptr<_N>& nA, const std::shared_ptr<_N>& nB, const char* errorMsg) const;

		inline bool assertConnectedNodes(const std::shared_ptr<_N>& nA, const std::shared_ptr<_N>& nB, const char* errorMsg) const;

		inline bool setNodeAsRoot(const std::weak_ptr<_N>& n);

	protected:
		std::vector<std::shared_ptr<_N>> nodes;
		std::vector<std::shared_ptr<_E>> edges;

		std::weak_ptr<_N> rootNode;
	};
}

// Include inline definitions
#include "GraphBase.inl"