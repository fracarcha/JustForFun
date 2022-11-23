#pragma once

#include <memory>

namespace JFF
{
	template<typename _G, typename _N, typename _E>
	class Algorithm
	{
	public:
		// Ctor & Dtor
		Algorithm() {}
		virtual ~Algorithm() {}

		// Copy ctor and copy assignment
		Algorithm(const Algorithm& other) = delete;
		Algorithm& operator=(const Algorithm& other) = delete;

		// Move ctor and assignment
		Algorithm(Algorithm&& other) = delete;
		Algorithm operator=(Algorithm&& other) = delete;

		// Functor
		virtual void operator()(const std::weak_ptr<_N>& itObj) = 0;

		// Resets internal data to make the algorithm reusable
		virtual void reset() = 0;
	};
}