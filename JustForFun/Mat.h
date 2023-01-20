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

#include "MatSetup.h"
#include "Vec.h"

namespace JFF
{
	template<int _Dim>
	class MatBase final
	{
		static_assert(_Dim >= 2 && _Dim <= 4, "Only 2x2, 3x3 or 4x4 dimension matrix are allowed");

	public:
		explicit MatBase(float diagonalValue = 1.0f);
		MatBase(JFF_MAT_IMPL_DEPENDENT_FUNC_PARAMS(_Dim));
		~MatBase();

		// Copy ctor and copy assignment
		MatBase(const MatBase& other);
		MatBase& operator=(const MatBase& other);

		// Move ctor and assignment
		MatBase(MatBase&& other) noexcept;
		MatBase& operator=(MatBase&& other) noexcept;

		// Matrix raw data
		const float* operator*() const;

	protected:
		JFF_MAT_IMPL_DEPENDENT_ATTRS(_Dim)
		JFF_MAT_GLOBAL_FRIENDS
	};

	// Name decorators
	using Mat2 = MatBase<2>;
	using Mat3 = MatBase<3>;
	using Mat4 = MatBase<4>;

	JFF_MAT_GLOBAL_FUNCTIONS
}

// Implementation dependant inline definitions
#include JFF_MAT_IMPL_DEPENDENT_INLINES