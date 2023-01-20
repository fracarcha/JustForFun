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
	template<typename _G, typename _N, typename _E, typename _RetVal>
	class GraphAlgorithm
	{
	public:
		// Ctor & Dtor
		GraphAlgorithm() {}
		virtual ~GraphAlgorithm() {}

		// Copy ctor and copy assignment
		GraphAlgorithm(const GraphAlgorithm& other) = delete;
		GraphAlgorithm& operator=(const GraphAlgorithm& other) = delete;

		// Move ctor and assignment
		GraphAlgorithm(GraphAlgorithm&& other) = delete;
		GraphAlgorithm operator=(GraphAlgorithm&& other) = delete;

		// Functor
		virtual _RetVal operator()(const std::weak_ptr<_N>& itObj) = 0;

		// Resets internal data to make the algorithm reusable
		virtual void reset() = 0;
	};
}