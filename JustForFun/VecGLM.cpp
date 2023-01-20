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

// NOTE: Template specializations need to be out of VecGLM.inl to avoid function redefinitions

const JFF::VecBase<2> JFF::VecBase<2>::ZERO(0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::ZERO(0.0f);
const JFF::VecBase<4> JFF::VecBase<4>::ZERO(0.0f);

const JFF::VecBase<2> JFF::VecBase<2>::ONE(1.0f);
const JFF::VecBase<3> JFF::VecBase<3>::ONE(1.0f);
const JFF::VecBase<4> JFF::VecBase<4>::ONE(1.0f);

const JFF::VecBase<3> JFF::VecBase<3>::UP(0.0f, 1.0f, 0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::DOWN(0.0f, -1.0f, 0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::RIGHT(1.0f, 0.0f, 0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::LEFT(-1.0f, 0.0f, 0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::FORWARD(0.0f, 0.0f, -1.0f);
const JFF::VecBase<3> JFF::VecBase<3>::BACKWARD(0.0f, 0.0f, 1.0f);

const JFF::VecBase<4> JFF::VecBase<4>::UP(0.0f, 1.0f, 0.0f, 0.0f);
const JFF::VecBase<4> JFF::VecBase<4>::DOWN(0.0f, -1.0f, 0.0f, 0.0f);
const JFF::VecBase<4> JFF::VecBase<4>::RIGHT(1.0f, 0.0f, 0.0f, 0.0f);
const JFF::VecBase<4> JFF::VecBase<4>::LEFT(-1.0f, 0.0f, 0.0f, 0.0f);
const JFF::VecBase<4> JFF::VecBase<4>::FORWARD(0.0f, 0.0f, -1.0f, 0.0f);
const JFF::VecBase<4> JFF::VecBase<4>::BACKWARD(0.0f, 0.0f, 1.0f, 0.0f);

const JFF::VecBase<3> JFF::VecBase<3>::WHITE(1.0f, 1.0f, 1.0f);
const JFF::VecBase<3> JFF::VecBase<3>::BLACK(0.0f, 0.0f, 0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::RED(1.0f, 0.0f, 0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::GREEN(0.0f, 1.0f, 0.0f);
const JFF::VecBase<3> JFF::VecBase<3>::BLUE(0.0f, 0.0f, 1.0f);

const JFF::VecBase<4> JFF::VecBase<4>::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const JFF::VecBase<4> JFF::VecBase<4>::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const JFF::VecBase<4> JFF::VecBase<4>::RED(1.0f, 0.0f, 0.0f, 1.0f);
const JFF::VecBase<4> JFF::VecBase<4>::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
const JFF::VecBase<4> JFF::VecBase<4>::BLUE(0.0f, 0.0f, 1.0f, 1.0f);

template<>
float JFF::sqrtLength(const JFF::VecBase<2>& v)
{
	return v.x * v.x + v.y * v.y;
}

template<>
float JFF::sqrtLength(const JFF::VecBase<3>& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

template<>
float JFF::sqrtLength(const JFF::VecBase<4>& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

template<>
JFF::VecBase<3> JFF::cross(const JFF::VecBase<3>& v1, const JFF::VecBase<3>& v2)
{
	return glm::cross(v1._vec, v2._vec);
}