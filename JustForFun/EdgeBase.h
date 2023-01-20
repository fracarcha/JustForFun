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

#include <memory>

namespace JFF
{
	template<typename _N>
	class EdgeBase
	{
	public:
		// Ctor & Dtor
		explicit EdgeBase(const std::weak_ptr<_N>& nodeSrc, const std::weak_ptr<_N>& nodeDst);
		virtual ~EdgeBase();

		// Copy ctor and copy assignment
		EdgeBase(const EdgeBase& other) = delete;
		EdgeBase& operator=(const EdgeBase& other) = delete;

		// Move ctor and assignment
		EdgeBase(EdgeBase&& other) = delete;
		EdgeBase operator=(EdgeBase&& other) = delete;

		// ----------------------------------------------- Getters ----------------------------------------------- //

		std::weak_ptr<_N> getSrcNode() const { return nodeSrc; }
		std::weak_ptr<_N> getDstNode() const { return nodeDst; }

		/* 
		*	Gets the other end of the edge, provided one end.
		*	The return value will be invalid if provided node isn't any end of this edge
		*/
		virtual std::weak_ptr<_N> getTheOtherEnd(const std::weak_ptr<_N>& from) const;

		// Returns true when input Edge is exactly the same (shares memory) than 'this'
		virtual bool operator==(const std::weak_ptr<EdgeBase>& other) const;

	protected:
		std::weak_ptr<_N> nodeSrc, nodeDst;
	};

}

// Include inline definitions
#include "EdgeBase.inl"