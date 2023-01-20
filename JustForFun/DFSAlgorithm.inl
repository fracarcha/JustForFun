/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "DFSAlgorithm.h"

#include "Log.h"
#include <algorithm>

#pragma region GENERIC DEPTH FIRST SEARCH ALGORITHM

template<typename _G, typename _N, typename _E>
inline JFF::DFSAlgorithm<_G, _N, _E>::DFSAlgorithm(const std::function<void(const std::weak_ptr<_N>&)>& func) :
	_func(func)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor DFSAlgorithm")
}

template<typename _G, typename _N, typename _E>
inline JFF::DFSAlgorithm<_G, _N, _E>::~DFSAlgorithm()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor DFSAlgorithm")
}

template<typename _G, typename _N, typename _E>
inline void JFF::DFSAlgorithm<_G, _N, _E>::operator()(const std::weak_ptr<_N>& itObj)
{
	// Visit the object
	visited.push_back(itObj);
	_func(itObj);

	// Select the next node using Depth First Search algorithm
	itObj.lock()->visitEdges([this, &itObj](const std::weak_ptr<_E>& edge)
		{
			// Get the node at the other end of each edge
			std::shared_ptr<_N> neighbour = edge.lock()->getTheOtherEnd(itObj).lock();

			// Return if neigbour was already visited
			auto predicate = [&neighbour](const std::weak_ptr<_N>& v) {return *neighbour == v; };
			if (std::find_if(visited.begin(), visited.end(), predicate) != visited.end())
				return;

			// Visit each child
			this->operator()(neighbour);
		});
}

template<typename _G, typename _N, typename _E>
inline void JFF::DFSAlgorithm<_G, _N, _E>::reset()
{
	visited.clear();
}

#pragma endregion

#pragma region TEMPLATE SPECIALIZATION FOR DIRECTED GRAPHS

template<typename _G> using DirectedGraphAlgorithm = JFF::DFSAlgorithm<_G, JFF::DirectedNode, JFF::DirectedNodeEdge>;

template<typename _G>
inline DirectedGraphAlgorithm<_G>::DFSAlgorithm(const std::function<void(const std::weak_ptr<DirectedNode>&)>& func) :
	_func(func)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor DFSAlgorithm")
}

template<typename _G>
inline DirectedGraphAlgorithm<_G>::~DFSAlgorithm()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor DFSAlgorithm")
}

template<typename _G>
inline void DirectedGraphAlgorithm<_G>::operator()(const std::weak_ptr<DirectedNode>& itObj)
{
	// Visit the object
	visited.push_back(itObj);
	_func(itObj);

	// Select the next node using Depth First Search algorithm
	itObj.lock()->visitOutcomingEdges([this](const std::weak_ptr<DirectedNodeEdge>& edge)
		{
			// Get the node at the other end of each edge
			std::shared_ptr<DirectedNode> neighbour = edge.lock()->getDstNode().lock();

			// Return if neigbour was already visited
			auto predicate = [&neighbour](const std::weak_ptr<DirectedNode>& v) {return *neighbour == v; };
			if (std::find_if(visited.begin(), visited.end(), predicate) != visited.end())
				return;

			// Visit each child
			this->operator()(neighbour);
		});
}

template<typename _G>
inline void DirectedGraphAlgorithm<_G>::reset()
{
	visited.clear();
}

#pragma endregion

#pragma region TEMPLATE SPECIALIZATION FOR DIRECTED TREE GRAPHS 

using DirectedTreeGraphAlgorithm = JFF::DFSAlgorithm<JFF::DirectedTreeGraph, JFF::DirectedNode, JFF::DirectedNodeEdge>;

inline DirectedTreeGraphAlgorithm::DFSAlgorithm(const std::function<void(const std::weak_ptr<DirectedNode>&)>& func) :
	_func(func)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor DFSAlgorithm")
}

inline DirectedTreeGraphAlgorithm::~DFSAlgorithm()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor DFSAlgorithm")
}

inline void DirectedTreeGraphAlgorithm::operator()(const std::weak_ptr<DirectedNode>& itObj)
{
	// Visit the object
	_func(itObj);

	// Select the next node using Depth First Search algorithm
	itObj.lock()->visitOutcomingEdges([this](const std::weak_ptr<DirectedNodeEdge>& edge)
		{
			// NOTE: There's no need to check if neightbout is visited because directed trees can't have loops and parents aren't visited

			// Get the node at the other end of each edge
			std::shared_ptr<DirectedNode> neighbour = edge.lock()->getDstNode().lock();

			// Visit each child
			this->operator()(neighbour);
		});
}

inline void DirectedTreeGraphAlgorithm::reset()
{
	// Not implemented
}

#pragma endregion

#pragma region TEMPLATE SPECIALIZATION FOR SCENES

using SceneAlgorithm = JFF::DFSAlgorithm<JFF::Scene, JFF::GameObject, JFF::GameObjectEdge>;

inline SceneAlgorithm::DFSAlgorithm(const std::function<void(const std::weak_ptr<GameObject>&)>& func, bool ignoreDisabledGameObjects) :
	_ignoreDisabledGameObjects(ignoreDisabledGameObjects),
	_func(func)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor DFSAlgorithm")
}

inline SceneAlgorithm::~DFSAlgorithm()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor DFSAlgorithm")
}

inline void SceneAlgorithm::operator()(const std::weak_ptr<GameObject>& itObj)
{
	std::shared_ptr<GameObject> itObjHandler = itObj.lock();

	// Don't exectute the function and loop on children if this object's state isn't enabled
	if (_ignoreDisabledGameObjects && !itObjHandler->isEnabled())
		return;

	// Visit the object
	_func(itObj);

	// Select the next node using Depth First Search algorithm
	itObjHandler->visitOutcomingEdges([this](const std::weak_ptr<GameObjectEdge>& edge)
		{
			// NOTE: There's no need to check if neightbout is visited because directed trees can't have loops and parents aren't visited

			// Get the node at the other end of each edge
			std::shared_ptr<GameObject> neighbour = edge.lock()->getDstNode().lock();

			// Visit each child
			this->operator()(neighbour);
		});
}

inline void SceneAlgorithm::reset()
{
	// Not implemented
}

#pragma endregion