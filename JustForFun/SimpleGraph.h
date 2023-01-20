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

#include "GraphBase.h"

namespace JFF
{
	template <typename _N, typename _E>
	class SimpleGraph : public GraphBase<_N, _E>
	{
	public:
		// Ctor & Dtor
		SimpleGraph();
		virtual ~SimpleGraph();

		// Copy ctor and copy assignment
		SimpleGraph(const SimpleGraph& other) = delete;
		SimpleGraph& operator=(const SimpleGraph& other) = delete;

		// Move ctor and assignment
		SimpleGraph(SimpleGraph&& other) = delete;
		SimpleGraph operator=(SimpleGraph&& other) = delete;


		// ---------------------------------- Inherited from Graph ---------------------------------- //

		/*
		* Adds a new node to the graph. If graph was empty, sets this as root node. The node won't be added if:
		*	* n was already included in the graph
		*	* n is empty
		*	* @return true if node was successfully added. False otherwise
		*/
		virtual bool addNode(const std::shared_ptr<_N>& n) override;


		// ---------------------------------- Simple Graph functions ---------------------------------- //

		/*
		* Sets the given node as root node. The operation will be cancelled if:
		*	* Node is not part of the graph before this function call
		*/
		virtual bool setRootNode(const std::weak_ptr<_N>& n);

		/*
		* Adds a new edge to the graph and connect it to its corresponding nodes.
		* This edge won't be included if:
		*	* Provided nodes aren't part of the graph before this function call
		*	* e was already included in the graph
		*	* e is empty
		*	* The nodes declared in 'e' are already connected
		*	* @return true if edge was successfully added. False otherwise
		*/
		virtual bool addEdge(const std::shared_ptr<_E>& e);

		/*
		* Creates an edge between the existing nodes.
		* This edge won't be included if:
		*	* Provided nodes aren't part of the graph before this function call
		*	* nA and nB points to the same node
		*	* nA or nB are empty
		* 	* The nodes are already connected
		*	* @return true if edge was successfully added. False otherwise
		*/
		virtual bool addEdge(const std::shared_ptr<_N>& srcNode, const std::shared_ptr<_N>& dstNode);

	};
}

// Include inline definitions
#include "SimpleGraph.inl"