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

// Library dependant macros
#if defined(JFF_GL) && defined(JFF_GLM)
#	include "GLM/glm.hpp"
#	include "GLM/gtc/type_ptr.hpp"
#	define JFF_VEC_IMPL_DEPENDANT_ATTRS(_Dim) glm::vec<_Dim, float, glm::qualifier::defaultp> _vec;
#	define JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(_Dim) const glm::vec<_Dim, float, glm::qualifier::defaultp>& vec
#	define JFF_VEC_IMPL_DEPENDANT_INLINES "VecGLM.inl"
#else
#	define JFF_VEC_IMPL_DEPENDANT_ATTRS
#	define JFF_VEC_IMPL_DEPENDANT_FUNC_PARAMS(_Dim)
#	define JFF_VEC_IMPL_DEPENDANT_INLINES "Vec.h" // Placeholder include header
#	error No API defined for Vec
#endif

// Library independent macros
#define _JFF_VEC_OPS(FRIEND_TEMPLATE) \
	FRIEND_TEMPLATE VecBase<_Dim> operator*(float scalar, const VecBase<_Dim>& v);					\
	FRIEND_TEMPLATE VecBase<_Dim> operator*(const VecBase<_Dim>& v, float scalar);					\
	FRIEND_TEMPLATE VecBase<_Dim> operator+(const VecBase<_Dim>& v1, const VecBase<_Dim>& v2);		\
	FRIEND_TEMPLATE VecBase<_Dim> operator-(const VecBase<_Dim>& v1, const VecBase<_Dim>& v2);		\
	FRIEND_TEMPLATE VecBase<_Dim>& operator*=(VecBase<_Dim>& v, float scalar);						\
	FRIEND_TEMPLATE VecBase<_Dim>& operator+=(VecBase<_Dim>& v1, const VecBase<_Dim>& v2);			\
	FRIEND_TEMPLATE VecBase<_Dim>& operator-=(VecBase<_Dim>& v1, const VecBase<_Dim>& v2);			\
	FRIEND_TEMPLATE VecBase<_Dim> operator+(const VecBase<_Dim>& v);								\
	FRIEND_TEMPLATE VecBase<_Dim> operator-(const VecBase<_Dim>& v);								\
	FRIEND_TEMPLATE bool operator==(const VecBase<_Dim>& v1, const VecBase<_Dim>& v2);				\
	FRIEND_TEMPLATE bool operator!=(const VecBase<_Dim>& v1, const VecBase<_Dim>& v2);				\
	FRIEND_TEMPLATE float length(const VecBase<_Dim>& v);											\
	FRIEND_TEMPLATE float sqrtLength(const VecBase<_Dim>& v);										\
	FRIEND_TEMPLATE float distance(const VecBase<_Dim>& v1, const VecBase<_Dim>& v2);				\
	FRIEND_TEMPLATE float dot(const VecBase<_Dim>& v1, const VecBase<_Dim>& v2);					\
	FRIEND_TEMPLATE VecBase<_Dim> cross(const VecBase<_Dim>& v1, const VecBase<_Dim>& v2);			\
	FRIEND_TEMPLATE VecBase<_Dim> normalize(const VecBase<_Dim>& v);								\
	FRIEND_TEMPLATE VecBase<_Dim> faceForward(const VecBase<_Dim>& n, const VecBase<_Dim>& i, const VecBase<_Dim>& nRef); \
	FRIEND_TEMPLATE VecBase<_Dim> reflect(const VecBase<_Dim>& i, const VecBase<_Dim>& n);			\
	FRIEND_TEMPLATE VecBase<_Dim> refract(const VecBase<_Dim>& i, const VecBase<_Dim>& n, float refrIdx);

#define JFF_VEC_GLOBAL_FUNCTIONS _JFF_VEC_OPS(template<int _Dim>)
#define JFF_VEC_GLOBAL_FRIENDS _JFF_VEC_OPS(template<int _Dim> friend)

// Make matrix-vector relationship friend functions of vector
#define JFF_VEC_MAT_FRIEND_FUNCTIONS \
	template<int _Dim> friend VecBase<_Dim> operator*(const MatBase<_Dim>& m, const VecBase<_Dim>& v);								\
	template<int _Dim> friend MatBase<_Dim> translate(const MatBase<_Dim>& m, const VecBase<3>& v);									\
	template<int _Dim> friend MatBase<_Dim> rotate(const MatBase<_Dim>& m, float angleRadians, const VecBase<3>& axisNormalized);	\
	template<int _Dim> friend MatBase<_Dim> scale(const MatBase<_Dim>& m, const VecBase<3>& v);										\
	template<int _Dim> friend MatBase<_Dim> lookAt(const VecBase<3>& eye, const VecBase<3>& center, const VecBase<3>& up);			\
	template<int _Dim> friend MatBase<_Dim> ortho(float left, float right, float bottom, float top, float zNear, float zFar);		\
	template<int _Dim> friend MatBase<_Dim> perspective(float fovyRad, float aspect, float near, float far);						