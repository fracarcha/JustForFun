/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "MathGLM.h"

#include "Log.h"

#include "GLM/trigonometric.hpp"

JFF::MathGLM::MathGLM()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: MathGLM")
}

JFF::MathGLM::~MathGLM()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: MathGLM")
}

void JFF::MathGLM::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: MathGLM")
}

void JFF::MathGLM::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: MathGLM")
}

JFF::Subsystem::UnloadOrder JFF::MathGLM::getUnloadOrder() const
{
	return UnloadOrder::MATH;
}

float JFF::MathGLM::abs(float n) const
{
	return glm::abs(n);
}

int JFF::MathGLM::abs(int n) const
{
	return glm::abs(n);
}

float JFF::MathGLM::pow(float base, float exp) const
{
	return std::pow(base, exp);
}

float JFF::MathGLM::clamp(float value, float min, float max) const
{
	return glm::clamp(value, min, max);
}

int JFF::MathGLM::clamp(int value, int min, int max) const
{
	return glm::clamp(value, min, max);
}

float JFF::MathGLM::lerp(float x, float y, float a) const
{
	return x + a * (y - x);
}

float JFF::MathGLM::radians(float degrees) const
{
	return glm::radians(degrees);
}

float JFF::MathGLM::degrees(float radians) const
{
	return glm::degrees(radians);
}

float JFF::MathGLM::sin(float angleRad) const
{
	return glm::sin(angleRad);
}

float JFF::MathGLM::cos(float angleRad) const
{
	return glm::cos(angleRad);
}

float JFF::MathGLM::tan(float angleRad) const
{
	return glm::tan(angleRad);
}

float JFF::MathGLM::asin(float value) const
{
	return glm::asin(value);
}

float JFF::MathGLM::acos(float value) const
{
	return glm::acos(value);
}

float JFF::MathGLM::atan(float value) const
{
	return glm::atan(value);
}

JFF::Vec2 JFF::MathGLM::vec2(float scalar) const
{
	return Vec2(scalar);
}

JFF::Vec2 JFF::MathGLM::vec2(float x, float y) const
{
	return Vec2(x, y);
}

float JFF::MathGLM::length(const Vec2& v) const
{
	return JFF::length(v);
}

float JFF::MathGLM::sqrtLength(const Vec2& v) const
{
	return JFF::sqrtLength(v);
}

float JFF::MathGLM::distance(const Vec2& v1, const Vec2& v2) const
{
	return JFF::distance(v1, v2);
}

float JFF::MathGLM::dot(const Vec2& v1, const Vec2& v2) const
{
	return JFF::dot(v1, v2);
}

JFF::Vec2 JFF::MathGLM::normalize(const Vec2& v) const
{
	return v == Vec2::ZERO ? v : JFF::normalize(v);
}

JFF::Vec2 JFF::MathGLM::faceForward(const Vec2& n, const Vec2& i, const Vec2& nRef) const
{
	return JFF::faceForward(n, i, nRef);
}

JFF::Vec2 JFF::MathGLM::reflect(const Vec2& i, const Vec2& n) const
{
	return JFF::reflect(i, n);
}

JFF::Vec2 JFF::MathGLM::refract(const Vec2& i, const Vec2& n, float refrIdx) const
{
	return JFF::refract(i, n, refrIdx);
}

JFF::Vec3 JFF::MathGLM::vec3(float scalar) const
{
	return Vec3(scalar);
}

JFF::Vec3 JFF::MathGLM::vec3(float x, float y, float z) const
{
	return Vec3(x, y, z);
}

float JFF::MathGLM::length(const Vec3& v) const
{
	return JFF::length(v);
}

float JFF::MathGLM::sqrtLength(const Vec3& v) const
{
	return JFF::sqrtLength(v);
}

float JFF::MathGLM::distance(const Vec3& v1, const Vec3& v2) const
{
	return JFF::distance(v1, v2);
}

float JFF::MathGLM::dot(const Vec3& v1, const Vec3& v2) const
{
	return JFF::dot(v1, v2);
}

JFF::Vec3 JFF::MathGLM::normalize(const Vec3& v) const
{
	return v == Vec3::ZERO ? v : JFF::normalize(v);
}

JFF::Vec3 JFF::MathGLM::faceForward(const Vec3& n, const Vec3& i, const Vec3& nRef) const
{
	return JFF::faceForward(n, i, nRef);
}

JFF::Vec3 JFF::MathGLM::reflect(const Vec3& i, const Vec3& n) const
{
	return JFF::reflect(i, n);
}

JFF::Vec3 JFF::MathGLM::refract(const Vec3& i, const Vec3& n, float refrIdx) const
{
	return JFF::refract(i, n, refrIdx);
}

JFF::Vec3 JFF::MathGLM::cross(const Vec3& v1, const Vec3& v2) const
{
	return JFF::cross(v1, v2);
}

JFF::Vec4 JFF::MathGLM::vec4(float scalar) const
{
	return Vec4(scalar);
}

JFF::Vec4 JFF::MathGLM::vec4(float x, float y, float z, float w) const
{
	return Vec4(x,y,z,w);
}

float JFF::MathGLM::length(const Vec4& v) const
{
	return JFF::length(v);
}

float JFF::MathGLM::sqrtLength(const Vec4& v) const
{
	return JFF::sqrtLength(v);
}

float JFF::MathGLM::distance(const Vec4& v1, const Vec4& v2) const
{
	return JFF::distance(v1, v2);
}

float JFF::MathGLM::dot(const Vec4& v1, const Vec4& v2) const
{
	return JFF::dot(v1, v2);
}

JFF::Vec4 JFF::MathGLM::normalize(const Vec4& v) const
{
	return v == Vec4::ZERO ? v : JFF::normalize(v);
}

JFF::Vec4 JFF::MathGLM::faceForward(const Vec4& n, const Vec4& i, const Vec4& nRef) const
{
	return JFF::faceForward(n, i, nRef);
}

JFF::Vec4 JFF::MathGLM::reflect(const Vec4& i, const Vec4& n) const
{
	return JFF::reflect(i, n);
}

JFF::Vec4 JFF::MathGLM::refract(const Vec4& i, const Vec4& n, float refrIdx) const
{
	return JFF::refract(i, n, refrIdx);
}

JFF::Mat2 JFF::MathGLM::mat2(float diagonalValue) const
{
	return Mat2(diagonalValue);
}

JFF::Mat2 JFF::MathGLM::transpose(const Mat2& m) const
{
	return JFF::transpose(m);
}

float JFF::MathGLM::determinant(const Mat2& m) const
{
	return JFF::determinant(m);
}

JFF::Mat2 JFF::MathGLM::inverse(const Mat2& m) const
{
	return JFF::inverse(m);
}

JFF::Mat3 JFF::MathGLM::mat3(float diagonalValue) const
{
	return Mat3(diagonalValue);
}

JFF::Mat3 JFF::MathGLM::transpose(const Mat3& m) const
{
	return JFF::transpose(m);
}

float JFF::MathGLM::determinant(const Mat3& m) const
{
	return JFF::determinant(m);
}

JFF::Mat3 JFF::MathGLM::inverse(const Mat3& m) const
{
	return JFF::inverse(m);
}

JFF::Mat4 JFF::MathGLM::mat4(float diagonalValue) const
{
	return Mat4(diagonalValue);
}

JFF::Mat4 JFF::MathGLM::transpose(const Mat4& m) const
{
	return JFF::transpose(m);
}

float JFF::MathGLM::determinant(const Mat4& m) const
{
	return JFF::determinant(m);
}

JFF::Mat4 JFF::MathGLM::inverse(const Mat4& m) const
{
	return JFF::inverse(m);
}

JFF::Mat4 JFF::MathGLM::translate(const Mat4& m, const Vec3& v) const
{
	return JFF::translate(m, v);
}

JFF::Mat4 JFF::MathGLM::rotate(const Mat4& m, float angleRadians, const Vec3& axisNormalized) const
{
	return JFF::rotate(m, angleRadians, axisNormalized);
}

JFF::Mat4 JFF::MathGLM::scale(const Mat4& m, const Vec3& v) const
{
	return JFF::scale(m, v);
}

JFF::Mat4 JFF::MathGLM::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) const
{
	return JFF::lookAt<4>(eye, center, up);
}

JFF::Mat4 JFF::MathGLM::ortho(float left, float right, float bottom, float top, float zNear, float zFar) const
{
	return JFF::ortho<4>(left, right, bottom, top, zNear, zFar);
}

JFF::Mat4 JFF::MathGLM::perspective(float fovyRad, float aspect, float zNear, float zFar) const
{
	return JFF::perspective<4>(fovyRad, aspect, zNear, zFar);
}

JFF::Mat3 JFF::MathGLM::reduceOrder(const Mat4& m) const
{
	return JFF::reduceOrder<3>(m);
}

JFF::Mat2 JFF::MathGLM::reduceOrder(const Mat3& m) const
{
	return JFF::reduceOrder<2>(m);
}
