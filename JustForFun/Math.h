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

#include "Subsystem.h"
#include "Mat.h" // Includes Vec.h inside

namespace JFF
{
	class Math : public Subsystem
	{
	public:
		// Ctor & Dtor
		Math() {}
		virtual ~Math() {}

		// Copy ctor and copy assignment
		Math(const Math& other) = delete;
		Math& operator=(const Math& other) = delete;

		// Move ctor and assignment
		Math(Math&& other) = delete;
		Math operator=(Math&& other) = delete;

		// ----------------------------------------- Math interface ----------------------------------------- //

		// Common ops

		virtual float abs(float n) const = 0;
		virtual int abs(int n) const = 0;
		virtual float pow(float base, float exp) const = 0;
		virtual float clamp(float value, float min, float max) const = 0;
		virtual int clamp(int value, int min, int max) const = 0;
		virtual float lerp(float x, float y, float a) const = 0;

		// Trigonometry

		virtual float radians(float degrees) const = 0;
		virtual float degrees(float radians) const = 0;
		virtual float sin(float angleRad) const = 0;
		virtual float cos(float angleRad) const = 0;
		virtual float tan(float angleRad) const = 0;
		virtual float asin(float value) const = 0;
		virtual float acos(float value) const = 0;
		virtual float atan(float value) const = 0;
	
		// Vectors

		virtual Vec2 vec2(float scalar = 0.0f) const = 0;
		virtual Vec2 vec2(float x, float y) const = 0;
		virtual float length(const Vec2& v) const = 0;
		virtual float sqrtLength(const Vec2& v) const = 0;
		virtual float distance(const Vec2& v1, const Vec2& v2) const = 0;
		virtual float dot(const Vec2& v1, const Vec2& v2) const = 0;
		virtual Vec2 normalize(const Vec2& v) const = 0;
		virtual Vec2 faceForward(const Vec2& n, const Vec2& i, const Vec2& nRef) const = 0;
		virtual Vec2 reflect(const Vec2& i, const Vec2& n) const = 0;
		virtual Vec2 refract(const Vec2& i, const Vec2& n, float refrIdx) const = 0;

		virtual Vec3 vec3(float scalar = 0.0f) const = 0;
		virtual Vec3 vec3(float x, float y, float z) const = 0;
		virtual float length(const Vec3& v) const = 0;
		virtual float sqrtLength(const Vec3& v) const = 0;
		virtual float distance(const Vec3& v1, const Vec3& v2) const = 0;
		virtual float dot(const Vec3& v1, const Vec3& v2) const = 0;
		virtual Vec3 normalize(const Vec3& v) const = 0;
		virtual Vec3 faceForward(const Vec3& n, const Vec3& i, const Vec3& nRef) const = 0;
		virtual Vec3 reflect(const Vec3& i, const Vec3& n) const = 0;
		virtual Vec3 refract(const Vec3& i, const Vec3& n, float refrIdx) const = 0;

		virtual Vec3 cross(const Vec3& v1, const Vec3& v2) const = 0;

		virtual Vec4 vec4(float scalar = 0.0f) const = 0;
		virtual Vec4 vec4(float x, float y, float z, float w = 1.0f) const = 0;
		virtual float length(const Vec4& v) const = 0;
		virtual float sqrtLength(const Vec4& v) const = 0;
		virtual float distance(const Vec4& v1, const Vec4& v2) const = 0;
		virtual float dot(const Vec4& v1, const Vec4& v2) const = 0;
		virtual Vec4 normalize(const Vec4& v) const = 0;
		virtual Vec4 faceForward(const Vec4& n, const Vec4& i, const Vec4& nRef) const = 0;
		virtual Vec4 reflect(const Vec4& i, const Vec4& n) const = 0;
		virtual Vec4 refract(const Vec4& i, const Vec4& n, float refrIdx) const = 0;

		// Matrices
		
		virtual Mat2 mat2(float diagonalValue = 1.0f) const = 0;
		virtual Mat2 transpose(const Mat2& m) const = 0;
		virtual float determinant(const Mat2& m) const = 0;
		virtual Mat2 inverse(const Mat2& m) const = 0;

		virtual Mat3 mat3(float diagonalValue = 1.0f) const = 0;
		virtual Mat3 transpose(const Mat3& m) const = 0;
		virtual float determinant(const Mat3& m) const = 0;
		virtual Mat3 inverse(const Mat3& m) const = 0;

		virtual Mat4 mat4(float diagonalValue = 1.0f) const = 0;
		virtual Mat4 transpose(const Mat4& m) const = 0;
		virtual float determinant(const Mat4& m) const = 0;
		virtual Mat4 inverse(const Mat4& m) const = 0;

		virtual Mat4 translate(const Mat4& m, const Vec3& v) const = 0;
		virtual Mat4 rotate(const Mat4& m, float angleRadians, const Vec3& axisNormalized) const = 0;
		virtual Mat4 scale(const Mat4& m, const Vec3& v) const = 0;
		virtual Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) const = 0;
		virtual Mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar) const = 0;
		virtual Mat4 perspective(float fovyRad, float aspect, float zNear, float zFar) const = 0;

		virtual Mat3 reduceOrder(const Mat4& m) const = 0;
		virtual Mat2 reduceOrder(const Mat3& m) const = 0;
	};
}