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
	template<typename _N, typename _E>
	class TreeGraph : public GraphBase<_N, _E>
	{
	public:
		// Ctor & Dtor
		TreeGraph();
		virtual ~TreeGraph();

		// Copy ctor and copy assignment
		TreeGraph(const TreeGraph& other) = delete;
		TreeGraph& operator=(const TreeGraph& other) = delete;

		// Move ctor and assignment
		TreeGraph(TreeGraph&& other) = delete;
		TreeGraph operator=(TreeGraph&& other) = delete;

		// ---------------------------------- Inherited from Graph ---------------------------------- //

		/*
		* Adds a new node to the graph. It only works if graph is empty, in which case sets this as root node. The node won't be added if:
		*	* The graph isn't empty
		*	* n is empty
		*	* @return true if node was successfully added. False otherwise
		*/
		virtual bool addNode(const std::shared_ptr<_N>& n) override;
	};
}

// Include inline definitions
#include "TreeGraph.inl"