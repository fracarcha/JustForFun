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

#include "VecSetup.h"

namespace JFF
{
	template<int _Dim> class MatBase;

	template<int _Dim>
	class VecBase final
	{
		static_assert(_Dim >= 2 && _Dim <= 4, "Only 2, 3 or 4 dimension vector are allowed");
	};

	JFF_VEC_GLOBAL_FUNCTIONS // Global functions to operate with vectors

	template<>
	class VecBase<2> final
	{
	public:
		// Ctor & Dtor
		explicit VecBase(float scalar = 0.0f);
		VecBase(JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(2)); // Implicit ctor
		VecBase(float x, float y);
		~VecBase();

		// Copy ctor and copy assignment
		VecBase(const VecBase& other);
		VecBase& operator=(const VecBase& other);

		// Move ctor and assignment
		VecBase(VecBase&& other) noexcept;
		VecBase& operator=(VecBase&& other) noexcept;

		// Vector raw data
		const float* operator*() const;

	public:
		float& x;
		float& r;
		float& s;

		float& y;
		float& g;
		float& t;

	public: // Predefined static Vec2
		static const VecBase<2> ZERO;
		static const VecBase<2> ONE;

	protected: // Implementation dependant attributes
		JFF_VEC_IMPL_DEPENDANT_ATTRS(2)
		JFF_VEC_GLOBAL_FRIENDS
		JFF_VEC_MAT_FRIEND_FUNCTIONS
	};

	template<>
	class VecBase<3> final
	{
	public:
		// Ctor & Dtor
		explicit VecBase(float scalar = 0.0f);
		VecBase(JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(3)); // Implicit ctor
		VecBase(float x, float y, float z);
		~VecBase();

		// Copy ctor and copy assignment
		VecBase(const VecBase& other);
		VecBase& operator=(const VecBase& other);

		// Move ctor and assignment
		VecBase(VecBase&& other) noexcept;
		VecBase& operator=(VecBase&& other) noexcept;

		// Vector raw data
		const float* operator*() const;

	public:
		float& x;
		float& r;
		float& s;
		float& pitch;
		float& red;

		float& y;
		float& g;
		float& t;
		float& yaw;
		float& green;

		float& z;
		float& b;
		float& p;
		float& roll;
		float& blue;

	public: // Predefined static Vec2
		static const VecBase<3> ZERO;
		static const VecBase<3> ONE;

		static const VecBase<3> UP;
		static const VecBase<3> DOWN;
		static const VecBase<3> RIGHT;
		static const VecBase<3> LEFT;
		static const VecBase<3> FORWARD;
		static const VecBase<3> BACKWARD;

		static const VecBase<3> WHITE;
		static const VecBase<3> BLACK;
		static const VecBase<3> RED;
		static const VecBase<3> GREEN;
		static const VecBase<3> BLUE;

	protected: // Implementation dependant attributes
		JFF_VEC_IMPL_DEPENDANT_ATTRS(3)
		JFF_VEC_GLOBAL_FRIENDS
		JFF_VEC_MAT_FRIEND_FUNCTIONS
	};

	template<>
	class VecBase<4> final
	{
	public:
		// Ctor & Dtor
		explicit VecBase(float scalar = 0.0f);
		VecBase(JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(4)); // Implicit ctor
		VecBase(float x, float y, float z, float w = 1.0f);
		~VecBase();

		// Copy ctor and copy assignment
		VecBase(const VecBase& other);
		VecBase& operator=(const VecBase& other);

		// Move ctor and assignment
		VecBase(VecBase&& other) noexcept;
		VecBase& operator=(VecBase&& other) noexcept;

		// Vector raw data
		const float* operator*() const;

	public:
		float& x;
		float& r;
		float& s;
		float& red;
		float& pitch;

		float& y;
		float& g;
		float& t;
		float& yaw;
		float& green;

		float& z;
		float& b;
		float& p;
		float& roll;
		float& blue;

		float& w;
		float& a;
		float& q;
		float& alpha;

	public: // Predefined static Vec2
		static const VecBase<4> ZERO;
		static const VecBase<4> ONE;

		static const VecBase<4> UP;
		static const VecBase<4> DOWN;
		static const VecBase<4> RIGHT;
		static const VecBase<4> LEFT;
		static const VecBase<4> FORWARD;
		static const VecBase<4> BACKWARD;

		static const VecBase<4> WHITE;
		static const VecBase<4> BLACK;
		static const VecBase<4> RED;
		static const VecBase<4> GREEN;
		static const VecBase<4> BLUE;

	protected: // Implementation dependant attributes
		JFF_VEC_IMPL_DEPENDANT_ATTRS(4)
		JFF_VEC_GLOBAL_FRIENDS
		JFF_VEC_MAT_FRIEND_FUNCTIONS
	};

	// Name decorators
	using Vec2 = VecBase<2>;
	using Vec3 = VecBase<3>;
	using Vec4 = VecBase<4>;
}

// Implementation dependant inline definitions
#include JFF_VEC_IMPL_DEPENDANT_INLINES