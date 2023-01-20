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
#include "GLM/gtc/matrix_transform.hpp"

// NOTE: Template specializations need to be out of MatGLM.inl to avoid function redefinitions

template<> 
JFF::MatBase<4> JFF::translate(const MatBase<4>& m, const Vec3& v)
{
	return glm::translate(m._mat, v._vec);
}

template<> 
JFF::MatBase<4> JFF::rotate(const MatBase<4>& m, float angleRadians, const Vec3& axisNormalized)
{
	return glm::rotate(m._mat, angleRadians, axisNormalized._vec);
}

template<> 
JFF::MatBase<4> JFF::scale(const MatBase<4>& m, const Vec3& v)
{
	return glm::scale(m._mat, v._vec);
}

template<> 
JFF::MatBase<4> JFF::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
{
	return glm::lookAt(eye._vec, center._vec, up._vec);
}

template<> 
JFF::MatBase<4> JFF::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	return glm::ortho(left, right, bottom, top, zNear, zFar);
}

template<>
JFF::MatBase<4> JFF::perspective(float fovyRad, float aspect, float zNear, float zFar)
{
	return glm::perspective(fovyRad, aspect, zNear, zFar);
}