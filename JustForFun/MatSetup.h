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
#	define JFF_MAT_IMPL_DEPENDENT_TYPE(_Dim) glm::mat<_Dim, _Dim, float, glm::qualifier::defaultp>
#	define JFF_MAT_IMPL_DEPENDENT_ATTRS(_Dim) JFF_MAT_IMPL_DEPENDENT_TYPE(_Dim) _mat;
#	define JFF_MAT_IMPL_DEPENDENT_FUNC_PARAMS(_Dim) const JFF_MAT_IMPL_DEPENDENT_TYPE(_Dim) & mat
#	define JFF_MAT_IMPL_DEPENDENT_INLINES "MatGLM.inl"
#else
#	define JFF_MAT_IMPL_DEPENDENT_ATTRS
#	define JFF_MAT_IMPL_DEPENDENT_FUNC_PARAMS(_Dim)
#	define JFF_MAT_IMPL_DEPENDENT_INLINES "Mat.h" // Placeholder include header
#	error No API defined for Mat
#endif

// Library independent macros
#define _JFF_MAT_OPS(FRIEND_TEMPLATE)																\
	FRIEND_TEMPLATE MatBase<_Dim> operator+(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2);		\
	FRIEND_TEMPLATE MatBase<_Dim> operator-(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2);		\
	FRIEND_TEMPLATE MatBase<_Dim> operator*(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2);		\
	FRIEND_TEMPLATE VecBase<_Dim> operator*(const MatBase<_Dim>& m, const VecBase<_Dim>& v);		\
	FRIEND_TEMPLATE MatBase<_Dim> operator+(const MatBase<_Dim>& m);								\
	FRIEND_TEMPLATE MatBase<_Dim> operator-(const MatBase<_Dim>& m);								\
	FRIEND_TEMPLATE MatBase<_Dim>& operator*=(MatBase<_Dim>& m1, const MatBase<_Dim>& m2);			\
	FRIEND_TEMPLATE MatBase<_Dim>& operator+=(MatBase<_Dim>& m1, const MatBase<_Dim>& m2);			\
	FRIEND_TEMPLATE MatBase<_Dim>& operator-=(MatBase<_Dim>& m1, const MatBase<_Dim>& m2);			\
	FRIEND_TEMPLATE bool operator==(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2);				\
	FRIEND_TEMPLATE bool operator!=(const MatBase<_Dim>& m1, const MatBase<_Dim>& m2);				\
	FRIEND_TEMPLATE MatBase<_Dim> transpose(const MatBase<_Dim>& m);								\
	FRIEND_TEMPLATE float determinant(const MatBase<_Dim>& m);										\
	FRIEND_TEMPLATE MatBase<_Dim> inverse(const MatBase<_Dim>& m);									\
	FRIEND_TEMPLATE MatBase<_Dim> translate(const MatBase<_Dim>& m, const Vec3& v);					\
	FRIEND_TEMPLATE MatBase<_Dim> rotate(const MatBase<_Dim>& m, float angleRadians, const Vec3& axisNormalized); \
	FRIEND_TEMPLATE MatBase<_Dim> scale(const MatBase<_Dim>& m, const Vec3& v);						\
	FRIEND_TEMPLATE MatBase<_Dim> lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);		\
	FRIEND_TEMPLATE MatBase<_Dim> ortho(float left, float right, float bottom, float top, float zNear, float zFar); \
	FRIEND_TEMPLATE MatBase<_Dim> perspective(float fovyRad, float aspect, float zNear, float zFar);	

#define _JFF_MAT_OPS_MULTIORDER(FRIEND_TEMPLATE)													\
	FRIEND_TEMPLATE MatBase<_Dim> reduceOrder(const MatBase<_DimHigher>& m);

// Needed to pass a comma separated template arguments as a single macro argument
#define _JFF_MAT_OPS_COMMA ,

#define JFF_MAT_GLOBAL_FUNCTIONS _JFF_MAT_OPS(template<int _Dim>) _JFF_MAT_OPS_MULTIORDER(template<int _Dim _JFF_MAT_OPS_COMMA int _DimHigher>)
#define JFF_MAT_GLOBAL_FRIENDS _JFF_MAT_OPS(template<int _Dim> friend) _JFF_MAT_OPS_MULTIORDER(template<int _Dim _JFF_MAT_OPS_COMMA int _DimHigher> friend)
