/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "EdgeBase.h"

#include "Log.h"

template<typename _N>
JFF::EdgeBase<_N>::EdgeBase(const std::weak_ptr<_N>& nodeSrc, const std::weak_ptr<_N>& nodeDst)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor EdgeBase")

	auto nodeSrcHandler = nodeSrc.lock();
	auto nodeDstHandler = nodeDst.lock();

	if (!(nodeSrcHandler.get() && nodeDstHandler.get()))
	{
		JFF_LOG_ERROR("Ill-formed Edge. Node components (one or both) are invalid")
		return;
	}

	// Don't allow loops
	if (*nodeSrcHandler == nodeDst)
	{
		JFF_LOG_ERROR("Ill-formed Edge. Class Edge doesn't allow loops")
		return;
	}

	// Set the nodes for this edge
	this->nodeSrc = nodeSrc;
	this->nodeDst = nodeDst;
}

template<typename _N>
JFF::EdgeBase<_N>::~EdgeBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor EdgeBase")
}

template<typename _N>
std::weak_ptr<_N> JFF::EdgeBase<_N>::getTheOtherEnd(const std::weak_ptr<_N>& from) const
{
	const std::shared_ptr<_N> n = from.lock();

	// Check if provided node is valid
	if (!n.get())
		return {};

	// Get the opposite of given node
	if (*n == nodeSrc)
	{
		return nodeDst;
	}
	else if (*n == nodeDst)
	{
		return nodeSrc;
	}
	
	return {};
}

template<typename _N>
bool JFF::EdgeBase<_N>::operator==(const std::weak_ptr<EdgeBase>& other) const
{
	return this == other.lock().get(); // TODO: is this a good idea ?
}
