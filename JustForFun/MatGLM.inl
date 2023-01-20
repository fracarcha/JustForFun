/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Mat.h"

#include "Log.h"

#pragma region Mat

template<int _Dim>
inline JFF::MatBase<_Dim>::MatBase(float diagonalValue) : 
	_mat(diagonalValue)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor Mat")
}

template<int _Dim>
inline JFF::MatBase<_Dim>::MatBase(JFF_MAT_IMPL_DEPENDENT_FUNC_PARAMS(_Dim)) :
	_mat(mat)
{
	JFF_LOG_INFO_LOW_PRIORITY("Platform dependant Ctor Mat")
}

template<int _Dim>
inline JFF::MatBase<_Dim>::~MatBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor Mat")
}

template<int _Dim>
inline JFF::MatBase<_Dim>::MatBase(const MatBase& other) :
	_mat(other._mat)
{
	JFF_LOG_INFO_LOW_PRIORITY("Copy Ctor Mat")
}

template<int _Dim>
inline JFF::MatBase<_Dim>& JFF::MatBase<_Dim>::operator=(const MatBase& other)
{
	_mat = other._mat;
	return *this;
}

template<int _Dim>
inline JFF::MatBase<_Dim>::MatBase(MatBase&& other) noexcept :
	_mat(other._mat)
{
	other._mat = glm::mat<_Dim, _Dim, float, glm::qualifier::defaultp>(0.0f);
	JFF_LOG_INFO_LOW_PRIORITY("Move Ctor Mat")
}

template<int _Dim>
inline JFF::MatBase<_Dim>& JFF::MatBase<_Dim>::operator=(MatBase&& other) noexcept
{
	_mat = other._mat;
	other._mat = glm::mat<_Dim, _Dim, float, glm::qualifier::defaultp>(0.0f);
	return *this;
}

template<int _Dim>
inline const float* JFF::MatBase<_Dim>::operator*() const
{
	return glm::value_ptr(_mat);
}

#pragma endregion

#pragma region Global operations

template<int _Dim>
JFF::MatBase<_Dim> JFF::operator+(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	return m1._mat + m2._mat;
}

template<int _Dim>
JFF::MatBase<_Dim> JFF::operator-(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	return m1._mat - m2._mat;
}

template<int _Dim>
JFF::MatBase<_Dim> JFF::operator*(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	return m1._mat * m2._mat;
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::operator*(const MatBase<_Dim>& m, const VecBase<_Dim>& v)
{
	return m._mat * v._vec;
}

template<int _Dim>
JFF::MatBase<_Dim> JFF::operator+(const MatBase<_Dim>& m)
{
	return m;
}

template<int _Dim>
JFF::MatBase<_Dim> JFF::operator-(const MatBase<_Dim>& m)
{
	return -m._mat;
}

template<int _Dim>
JFF::MatBase<_Dim>& JFF::operator*=(MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	m1._mat *= m2._mat;
	return m1;
}

template<int _Dim>
JFF::MatBase<_Dim>& JFF::operator+=(MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	m1._mat += m2._mat;
	return m1;
}

template<int _Dim>
JFF::MatBase<_Dim>& JFF::operator-=(MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	m1._mat -= m2._mat;
	return m1;
}

template<int _Dim>
bool JFF::operator==(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	return m1._mat == m2._mat;
}

template<int _Dim>
bool JFF::operator!=(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2)
{
	return m1._mat != m2._mat;
}

template<int _Dim>
JFF::MatBase<_Dim> JFF::transpose(const MatBase<_Dim>& m)
{
	return glm::transpose(m._mat);
}

template<int _Dim>
float JFF::determinant(const MatBase<_Dim>& m)
{
	return glm::determinant(m._mat);
}

template<int _Dim>
JFF::MatBase<_Dim> JFF::inverse(const MatBase<_Dim>& m)
{
	return glm::inverse(m._mat);
}

template<int _Dim, int _DimHigher>
JFF::MatBase<_Dim> JFF::reduceOrder(const JFF::MatBase<_DimHigher>& m)
{
	// Ensure _DimHigher is higher than _Dim
	static_assert(_DimHigher > _Dim, "Reduce order can only transform a higher order matrix into a smaller one");

	JFF_MAT_IMPL_DEPENDENT_TYPE(_Dim) mReduced = m._mat;
	return JFF::MatBase<_Dim>(mReduced);
}

#pragma endregion