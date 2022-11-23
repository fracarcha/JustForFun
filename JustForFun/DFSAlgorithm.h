#pragma once

#include "Algorithm.h"

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
	class DFSAlgorithm : public Algorithm<_G, _N, _E>
	{
	public:
		// Ctor & Dtor
		DFSAlgorithm(const std::function<void(const std::weak_ptr<_N>&)>& func);
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
	class DFSAlgorithm<_G, DirectedNode, DirectedNodeEdge> : public Algorithm<_G, DirectedNode, DirectedNodeEdge>
	{
	public:
		// Ctor & Dtor
		DFSAlgorithm(const std::function<void(const std::weak_ptr<DirectedNode>&)>& func);
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
	class DFSAlgorithm<DirectedTreeGraph, DirectedNode, DirectedNodeEdge> : public Algorithm<DirectedTreeGraph, DirectedNode, DirectedNodeEdge>
	{
	public:
		// Ctor & Dtor
		DFSAlgorithm(const std::function<void(const std::weak_ptr<DirectedNode>&)>& func);
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
	class DFSAlgorithm<Scene, GameObject, GameObjectEdge> : public Algorithm<Scene, GameObject, GameObjectEdge>
	{
	public:
		// Ctor & Dtor
		DFSAlgorithm(const std::function<void(const std::weak_ptr<GameObject>&)>& func);
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
		std::function<void(const std::weak_ptr<GameObject>&)> _func;
	};
}

// Include inline definitions
#include "DFSAlgorithm.inl"