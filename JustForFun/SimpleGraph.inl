/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "SimpleGraph.h"

#include "Log.h"

template<typename _N, typename _E>
inline JFF::SimpleGraph<_N, _E>::SimpleGraph()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor SimpleGraph")
}

template<typename _N, typename _E>
inline JFF::SimpleGraph<_N, _E>::~SimpleGraph()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor SimpleGraph")
}

template<typename _N, typename _E>
bool JFF::SimpleGraph<_N, _E>::addNode(const std::shared_ptr<_N>& n)
{
	if (!this->assertValidNode(n, "Try adding an invalid node to the graph. Aborted."))
		return false;
	if (!this->assertUniqueNode(n, "Try adding a node that is already in the graph. Aborted."))
		return false;
	
	this->nodes.push_back(n);
	
	if (this->nodes.size() == 1)
		setRootNode(n);
	
	return true;
}

template<typename _N, typename _E>
bool JFF::SimpleGraph<_N, _E>::setRootNode(const std::weak_ptr<_N>& n)
{
	return this->setNodeAsRoot(n);
}

template<typename _N, typename _E>
bool JFF::SimpleGraph<_N, _E>::addEdge(const std::shared_ptr<_E>& e)
{
	if (!this->assertValidEdge(e, "Try adding an invalid edge to the graph. Aborted."))
		return false;
	if (!this->assertUniqueEdge(e, "Try adding an edge that is already in the graph. Aborted."))
		return false;
	
	std::shared_ptr<_N> srcNode = std::dynamic_pointer_cast<_N>(e->getSrcNode().lock()); // TODO: unnecesary dynamic cast
	if (!this->assertInGraphNode(srcNode, "Cannot add edge if one of its declared nodes is not part of the graph before this call"))
		return false;
	
	std::shared_ptr<_N> dstNode = std::dynamic_pointer_cast<_N>(e->getDstNode().lock()); // TODO: unnecesary dynamic cast
	if (!this->assertInGraphNode(dstNode, "Cannot add edge if one of its declared nodes is not part of the graph before this call"))
		return false;
	
	if (this->assertConnectedNodes(srcNode, dstNode, "An edge exist between provided nodes before this function call. Aborted."))
		return false;
	
	// Add edge to the graph
	this->edges.push_back(e);
	
	// Register the edges in nodes
	*srcNode >> e;
	*dstNode << e;
	
	return true;
}

template<typename _N, typename _E>
bool JFF::SimpleGraph<_N, _E>::addEdge(const std::shared_ptr<_N>& srcNode, const std::shared_ptr<_N>& dstNode)
{
	if (!this->assertValidNode(srcNode, "Try creating an edge to the graph, but provided node is invalid. Aborted."))
		return false;
	if (!this->assertValidNode(dstNode, "Try creating an edge to the graph, but provided node is invalid. Aborted."))
		return false;
	if (this->assertEqualNodes(srcNode, dstNode, "Try creating an edge to the graph in a loop. Aborted."))
		return false;
	if (!this->assertInGraphNode(srcNode, "Cannot add edge if one of its declared nodes is not part of the graph before this call"))
		return false;
	if (!this->assertInGraphNode(dstNode, "Cannot add edge if one of its declared nodes is not part of the graph before this call"))
		return false;
	if (this->assertConnectedNodes(srcNode, dstNode, "An edge exist between provided nodes before this function call. Aborted."))
		return false;
	
	// Create a new edge between the two nodes
	std::shared_ptr<_E> newEdge = std::make_shared<_E>(srcNode, dstNode);
	this->edges.push_back(newEdge);
	
	// Register the edges in nodes
	*srcNode >> newEdge;
	*dstNode << newEdge;
	
	return true;
}