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

#include "Scene.h"
#include "DirectedNode.h"

#include <vector>
#include <functional>

namespace JFF
{
	using DirectedNodeEdge = EdgeBase<DirectedNode>;
	using GameObjectEdge = EdgeBase<GameObject>;
	using DirectedTreeGraph = TreeGraph<DirectedNode, DirectedNodeEdge>;

	// ----------------------------------------- GENERIC DEPTH FIRST SEARCH ALGORITHM ----------------------------------------- //

	template<typename _G, typename _N, typename _E>
	class DFSAlgorithm : public GraphAlgorithm<_G, _N, _E, void>
	{
	public:
		// Ctor & Dtor
		explicit DFSAlgorithm(const std::function<void(const std::weak_ptr<_N>&)>& func); // The search stops if func returns true
		virtual ~DFSAlgorithm();

		// Copy ctor and copy assignment
		DFSAlgorithm(const DFSAlgorithm& other) = delete;
		DFSAlgorithm& operator=(const DFSAlgorithm& other) = delete;

		// Move ctor and assignment
		DFSAlgorithm(DFSAlgorithm&& other) = delete;
		DFSAlgorithm operator=(DFSAlgorithm&& other) = delete;

		// Functor
		virtual void operator()(const std::weak_ptr<_N>& itObj) override;

		// Resets internal data to make the algorithm reusable
		virtual void reset() override;

	protected:
		std::vector<std::weak_ptr<_N>> visited;
		std::function<void(const std::weak_ptr<_N>&)> _func;
	};

	// ----------------------------------------- TEMPLATE SPECIALIZATION FOR DIRECTED GRAPHS ----------------------------------------- //

	template<typename _G>
	class DFSAlgorithm<_G, DirectedNode, DirectedNodeEdge> : public GraphAlgorithm<_G, DirectedNode, DirectedNodeEdge, void>
	{
	public:
		// Ctor & Dtor
		explicit DFSAlgorithm(const std::function<void(const std::weak_ptr<DirectedNode>&)>& func);
		virtual ~DFSAlgorithm();

		// Copy ctor and copy assignment
		DFSAlgorithm(const DFSAlgorithm& other) = delete;
		DFSAlgorithm& operator=(const DFSAlgorithm& other) = delete;

		// Move ctor and assignment
		DFSAlgorithm(DFSAlgorithm&& other) = delete;
		DFSAlgorithm operator=(DFSAlgorithm&& other) = delete;

		// Functor
		virtual void operator()(const std::weak_ptr<DirectedNode>& itObj) override;

		// Resets internal data to make the algorithm reusable
		virtual void reset() override;

	protected:
		std::vector<std::weak_ptr<DirectedNode>> visited;
		std::function<void(const std::weak_ptr<DirectedNode>&)> _func;
	};

	// ----------------------------------------- TEMPLATE SPECIALIZATION FOR DIRECTED TREE GRAPHS ----------------------------------------- //

	template<>
	class DFSAlgorithm<DirectedTreeGraph, DirectedNode, DirectedNodeEdge> : public GraphAlgorithm<DirectedTreeGraph, DirectedNode, DirectedNodeEdge, void>
	{
	public:
		// Ctor & Dtor
		explicit DFSAlgorithm(const std::function<void(const std::weak_ptr<DirectedNode>&)>& func);
		virtual ~DFSAlgorithm();

		// Copy ctor and copy assignment
		DFSAlgorithm(const DFSAlgorithm& other) = delete;
		DFSAlgorithm& operator=(const DFSAlgorithm& other) = delete;

		// Move ctor and assignment
		DFSAlgorithm(DFSAlgorithm&& other) = delete;
		DFSAlgorithm operator=(DFSAlgorithm&& other) = delete;

		// Functor
		virtual void operator()(const std::weak_ptr<DirectedNode>& itObj) override;

		// Resets internal data to make the algorithm reusable
		virtual void reset() override;

	protected:
		std::function<void(const std::weak_ptr<DirectedNode>&)> _func;
	};

	// ----------------------------------------- TEMPLATE SPECIALIZATION FOR SCENES ----------------------------------------- //

	template<>
	class DFSAlgorithm<Scene, GameObject, GameObjectEdge> : public GraphAlgorithm<Scene, GameObject, GameObjectEdge, void>
	{
	public:
		// Ctor & Dtor
		explicit DFSAlgorithm(const std::function<void(const std::weak_ptr<GameObject>&)>& func, bool ignoreDisabledGameObjects = true);
		virtual ~DFSAlgorithm();

		// Copy ctor and copy assignment
		DFSAlgorithm(const DFSAlgorithm& other) = delete;
		DFSAlgorithm& operator=(const DFSAlgorithm& other) = delete;

		// Move ctor and assignment
		DFSAlgorithm(DFSAlgorithm&& other) = delete;
		DFSAlgorithm operator=(DFSAlgorithm&& other) = delete;

		// Functor
		virtual void operator()(const std::weak_ptr<GameObject>& itObj) override;

		// Resets internal data to make the algorithm reusable
		virtual void reset() override;

	protected:
		bool _ignoreDisabledGameObjects;
		std::function<void(const std::weak_ptr<GameObject>&)> _func;
	};
}

// Include inline definitions
#include "DFSAlgorithm.inl"