#pragma once

#include "DirectedNodeBase.h"
#include "EdgeBase.h"

namespace JFF
{
	class DirectedNode : public DirectedNodeBase<EdgeBase<DirectedNode>>
	{
	public:
		// Ctor & Dtor
		DirectedNode();
		virtual ~DirectedNode();

		// Copy ctor and copy assignment
		DirectedNode(const DirectedNode& other) = delete;
		DirectedNode& operator=(const DirectedNode& other) = delete;

		// Move ctor and assignment
		DirectedNode(DirectedNode&& other) = delete;
		DirectedNode operator=(DirectedNode&& other) = delete;
	};
}