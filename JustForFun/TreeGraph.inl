/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "TreeGraph.h"

#include "Log.h"

template<typename _N, typename _E>
inline JFF::TreeGraph<_N, _E>::TreeGraph()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor TreeGraph")
}

template<typename _N, typename _E>
inline JFF::TreeGraph<_N, _E>::~TreeGraph()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor TreeGraph")
}

template<typename _N, typename _E>
inline bool JFF::TreeGraph<_N, _E>::addNode(const std::shared_ptr<_N>& n)
{
	if (!this->assertValidNode(n, "Try adding an invalid node to the graph. Aborted."))
		return false;
	if (this->nodes.size() > 0)
	{
		JFF_LOG_WARNING("Try adding a disconected node on tree. Aborted.")
		return false;
	}

	this->nodes.push_back(n);
	this->setNodeAsRoot(n);

	return true;
}