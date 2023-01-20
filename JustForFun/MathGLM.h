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

#include "Math.h"

namespace JFF
{
	// Math implementation using GLM math library
	class MathGLM : public Math
	{
	public:
		// Ctor & Dtor
		MathGLM();
		virtual ~MathGLM();

		// Copy ctor and copy assignment
		MathGLM(const MathGLM& other) = delete;
		MathGLM& operator=(const MathGLM& other) = delete;

		// Move ctor and assignment
		MathGLM(MathGLM&& other) = delete;
		MathGLM operator=(MathGLM&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ----------------------------------------- Math interface ----------------------------------------- //

		// Common ops

		virtual float abs(float n) const override;
		virtual int abs(int n) const override;
		virtual float pow(float base, float exp) const override;
		virtual float clamp(float value, float min, float max) const override;
		virtual int clamp(int value, int min, int max) const override;
		virtual float lerp(float x, float y, float a) const override;

		// Trigonometry

		virtual float radians(float degrees) const override;
		virtual float degrees(float radians) const override;
		virtual float sin(float angleRad) const override;
		virtual float cos(float angleRad) const override;
		virtual float tan(float angleRad) const override;
		virtual float asin(float value) const override;
		virtual float acos(float value) const override;
		virtual float atan(float value) const override;

		// Vectors

		virtual Vec2 vec2(float scalar = 0.0f) const override;
		virtual Vec2 vec2(float x, float y) const override;
		virtual float length(const Vec2& v) const override;
		virtual float sqrtLength(const Vec2& v) const override;
		virtual float distance(const Vec2& v1, const Vec2& v2) const override;
		virtual float dot(const Vec2& v1, const Vec2& v2) const override;
		virtual Vec2 normalize(const Vec2& v) const override;
		virtual Vec2 faceForward(const Vec2& n, const Vec2& i, const Vec2& nRef) const override;
		virtual Vec2 reflect(const Vec2& i, const Vec2& n) const override;
		virtual Vec2 refract(const Vec2& i, const Vec2& n, float refrIdx) const override;

		virtual Vec3 vec3(float scalar = 0.0f) const override;
		virtual Vec3 vec3(float x, float y, float z) const override;
		virtual float length(const Vec3& v) const override;
		virtual float sqrtLength(const Vec3& v) const override;
		virtual float distance(const Vec3& v1, const Vec3& v2) const override;
		virtual float dot(const Vec3& v1, const Vec3& v2) const override;
		virtual Vec3 normalize(const Vec3& v) const override;
		virtual Vec3 faceForward(const Vec3& n, const Vec3& i, const Vec3& nRef) const override;
		virtual Vec3 reflect(const Vec3& i, const Vec3& n) const override;
		virtual Vec3 refract(const Vec3& i, const Vec3& n, float refrIdx) const override;

		virtual Vec3 cross(const Vec3& v1, const Vec3& v2) const override;

		virtual Vec4 vec4(float scalar = 0.0f) const override;
		virtual Vec4 vec4(float x, float y, float z, float w = 1.0f) const override;
		virtual float length(const Vec4& v) const override;
		virtual float sqrtLength(const Vec4& v) const override;
		virtual float distance(const Vec4& v1, const Vec4& v2) const override;
		virtual float dot(const Vec4& v1, const Vec4& v2) const override;
		virtual Vec4 normalize(const Vec4& v) const override;
		virtual Vec4 faceForward(const Vec4& n, const Vec4& i, const Vec4& nRef) const override;
		virtual Vec4 reflect(const Vec4& i, const Vec4& n) const override;
		virtual Vec4 refract(const Vec4& i, const Vec4& n, float refrIdx) const override;

		// Matrices

		virtual Mat2 mat2(float diagonalValue = 1.0f) const override;
		virtual Mat2 transpose(const Mat2& m) const override;
		virtual float determinant(const Mat2& m) const override;
		virtual Mat2 inverse(const Mat2& m) const override;

		virtual Mat3 mat3(float diagonalValue = 1.0f) const override;
		virtual Mat3 transpose(const Mat3& m) const override;
		virtual float determinant(const Mat3& m) const override;
		virtual Mat3 inverse(const Mat3& m) const override;

		virtual Mat4 mat4(float diagonalValue = 1.0f) const override;
		virtual Mat4 transpose(const Mat4& m) const override;
		virtual float determinant(const Mat4& m) const override;
		virtual Mat4 inverse(const Mat4& m) const override;

		virtual Mat4 translate(const Mat4& m, const Vec3& v) const override;
		virtual Mat4 rotate(const Mat4& m, float angleRadians, const Vec3& axisNormalized) const override;
		virtual Mat4 scale(const Mat4& m, const Vec3& v) const override;
		virtual Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) const override;
		virtual Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar) const override;
		virtual Mat4 perspective(float fovyRad, float aspect, float zNear, float zFar) const override;

		virtual Mat3 reduceOrder(const Mat4& m) const override;
		virtual Mat2 reduceOrder(const Mat3& m) const override;
	};
}