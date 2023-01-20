/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Vec.h"
#include "Log.h"

#pragma region Vec2
inline JFF::VecBase<2>::VecBase(float scalar) :
	_vec(scalar),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor VecBase 2")
}

inline JFF::VecBase<2>::VecBase(JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(2)) :
	_vec(vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t)
{
	JFF_LOG_INFO_LOW_PRIORITY("Platform dependant Ctor VecBase 2")
}

inline JFF::VecBase<2>::VecBase(float x, float y) :
	_vec(x, y),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor VecBase 2")
}

inline JFF::VecBase<2>::~VecBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor VecBase 2")
}

inline JFF::VecBase<2>::VecBase(const VecBase& other) :
	_vec(other._vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t)
{
	JFF_LOG_INFO_LOW_PRIORITY("Copy Ctor VecBase 2")
}

inline JFF::VecBase<2>& JFF::VecBase<2>::operator=(const VecBase& other)
{
	_vec = other._vec;
	return *this;
}

inline JFF::VecBase<2>::VecBase(VecBase&& other) noexcept :
	_vec(other._vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t)
{
	other.x = 0.0f;
	other.y = 0.0f;
	JFF_LOG_INFO_LOW_PRIORITY("Move Ctor VecBase 2")
}

inline JFF::VecBase<2>& JFF::VecBase<2>::operator=(VecBase&& other) noexcept
{
	_vec = other._vec;
	other.x = 0.0f;
	other.y = 0.0f;
	return *this;
}

inline const float* JFF::VecBase<2>::operator*() const
{
	return glm::value_ptr(_vec);
}
#pragma endregion

#pragma region Vec3
inline JFF::VecBase<3>::VecBase(float scalar) :
	_vec(scalar),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor VecBase 3")
}

inline JFF::VecBase<3>::VecBase(JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(3)) :
	_vec(vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z)
{
	JFF_LOG_INFO_LOW_PRIORITY("Platform dependant Ctor VecBase 3")
}

inline JFF::VecBase<3>::VecBase(float x, float y, float z) :
	_vec(x, y, z),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor VecBase 3")
}

inline JFF::VecBase<3>::~VecBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor VecBase 3")
}

inline JFF::VecBase<3>::VecBase(const VecBase& other) :
	_vec(other._vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z)
{
	JFF_LOG_INFO_LOW_PRIORITY("Copy Ctor VecBase 3")
}

inline JFF::VecBase<3>& JFF::VecBase<3>::operator=(const VecBase& other)
{
	_vec = other._vec;
	return *this;
}

inline JFF::VecBase<3>::VecBase(VecBase&& other) noexcept :
	_vec(other._vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z)
{
	other.x = 0.0f;
	other.y = 0.0f;
	other.z = 0.0f;
	JFF_LOG_INFO_LOW_PRIORITY("Move Ctor VecBase 3")
}

inline JFF::VecBase<3>& JFF::VecBase<3>::operator=(VecBase&& other) noexcept
{
	_vec = other._vec;
	other.x = 0.0f;
	other.y = 0.0f;
	other.z = 0.0f;
	return *this;
}

inline const float* JFF::VecBase<3>::operator*() const
{
	return glm::value_ptr(_vec);
}
#pragma endregion

#pragma region Vec4
inline JFF::VecBase<4>::VecBase(float scalar) :
	_vec(scalar),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z),
	w(_vec.w),
	a(_vec.a),
	q(_vec.q),
	alpha(_vec.w)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor VecBase 4")
}

inline JFF::VecBase<4>::VecBase(JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(4)) :
	_vec(vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z),
	w(_vec.w),
	a(_vec.a),
	q(_vec.q),
	alpha(_vec.w)
{
	JFF_LOG_INFO_LOW_PRIORITY("Platform dependant Ctor VecBase 4")
}

inline JFF::VecBase<4>::VecBase(float x, float y, float z, float w) :
	_vec(x, y, z, w),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z),
	w(_vec.w),
	a(_vec.a),
	q(_vec.q),
	alpha(_vec.w)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor VecBase 4")
}

inline JFF::VecBase<4>::~VecBase()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor VecBase 4")
}

inline JFF::VecBase<4>::VecBase(const VecBase& other) :
	_vec(other._vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z),
	w(_vec.w),
	a(_vec.a),
	q(_vec.q),
	alpha(_vec.w)
{
	JFF_LOG_INFO_LOW_PRIORITY("Copy Ctor VecBase 4")
}

inline JFF::VecBase<4>& JFF::VecBase<4>::operator=(const VecBase& other)
{
	_vec = other._vec;
	return *this;
}

inline JFF::VecBase<4>::VecBase(VecBase&& other) noexcept :
	_vec(other._vec),
	x(_vec.x),
	r(_vec.r),
	s(_vec.s),
	pitch(_vec.x),
	red(_vec.x),
	y(_vec.y),
	g(_vec.g),
	t(_vec.t),
	yaw(_vec.y),
	green(_vec.y),
	z(_vec.z),
	b(_vec.b),
	p(_vec.p),
	roll(_vec.z),
	blue(_vec.z),
	w(_vec.w),
	a(_vec.a),
	q(_vec.q),
	alpha(_vec.w)
{
	other.x = 0.0f;
	other.y = 0.0f;
	other.z = 0.0f;
	other.w = 0.0f;
	JFF_LOG_INFO_LOW_PRIORITY("Move Ctor VecBase 4")
}

inline JFF::VecBase<4>& JFF::VecBase<4>::operator=(VecBase&& other) noexcept
{
	_vec = other._vec;
	other.x = 0.0f;
	other.y = 0.0f;
	other.z = 0.0f;
	other.w = 0.0f;
	return *this;
}

inline const float* JFF::VecBase<4>::operator*() const
{
	return glm::value_ptr(_vec);
}

#pragma endregion

#pragma region Global operations
template<int _Dim>
JFF::VecBase<_Dim> JFF::operator*(float scalar, const JFF::VecBase<_Dim>& v)
{
	return v._vec * scalar;
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::operator*(const JFF::VecBase<_Dim>& v, float scalar)
{
	return v._vec * scalar;
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::operator+(const JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	return v1._vec + v2._vec;
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::operator-(const JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	return v1._vec - v2._vec;
}

template<int _Dim>
JFF::VecBase<_Dim>& JFF::operator*=(JFF::VecBase<_Dim>& v, float scalar)
{
	v._vec *= scalar;
	return v;
}

template<int _Dim>
JFF::VecBase<_Dim>& JFF::operator+=(JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	v1._vec += v2._vec;
	return v1;
}

template<int _Dim>
JFF::VecBase<_Dim>& JFF::operator-=(JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	v1._vec -= v2._vec;
	return v1;
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::operator+(const JFF::VecBase<_Dim>& v)
{
	return v;
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::operator-(const JFF::VecBase<_Dim>& v)
{
	return -v._vec;
}

template<int _Dim>
bool JFF::operator==(const JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	return v1._vec == v2._vec;
}

template<int _Dim>
bool JFF::operator!=(const JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	return v1._vec != v2._vec;
}

template<int _Dim> 
float JFF::length(const JFF::VecBase<_Dim>& v)
{
	return glm::length(v._vec);
}

template<int _Dim> 
float JFF::distance(const JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	return glm::distance(v1._vec, v2._vec);
}

template<int _Dim> 
float JFF::dot(const JFF::VecBase<_Dim>& v1, const JFF::VecBase<_Dim>& v2)
{
	return glm::dot(v1._vec, v2._vec);
}

template<int _Dim> 
JFF::VecBase<_Dim> JFF::normalize(const VecBase<_Dim>& v)
{
	return glm::normalize(v._vec);
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::faceForward(const VecBase<_Dim>& n, const VecBase<_Dim>& i, const VecBase<_Dim>& nRef)
{
	return glm::faceforward(n._vec, i._vec, nRef._vec);
}

template<int _Dim> 
JFF::VecBase<_Dim> JFF::reflect(const VecBase<_Dim>& i, const VecBase<_Dim>& n)
{
	return glm::reflect(i._vec, n._vec);
}

template<int _Dim>
JFF::VecBase<_Dim> JFF::refract(const VecBase<_Dim>& i, const VecBase<_Dim>& n, float refrIdx)
{
	return glm::refract(i._vec, n._vec, refrIdx);
}

#pragma endregion





