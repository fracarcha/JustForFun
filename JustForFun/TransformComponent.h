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

#include "Component.h"
#include "Mat.h" // Includes Vec.h inside

namespace JFF
{
	class TransformComponent : public Component
	{
	public:
		// Ctor & Dtor
		explicit TransformComponent(
			GameObject* const gameObject, 
			const char* name,
			bool initiallyEnabled,
			const Vec3& localPosition = Vec3::ZERO, 
			const Vec3& localRotation = Vec3::ZERO,
			const Vec3& localScale = Vec3::ONE);
		virtual ~TransformComponent();

		// Copy ctor and copy assignment
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;

		// Move ctor and assignment
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent operator=(TransformComponent&& other) = delete;

		// ----------------------------- COMPONENT OVERRIDES ----------------------------- //

		virtual void onStart() override {} // Not used (Never called)

		// ----------------------------- TRANSFOM COMPONENT FUNCTIONS ----------------------------- //

		// Local setters
		virtual void setLocalPos(Vec3 localPos);
		virtual void setLocalPos(float x, float y, float z);
		virtual void setLocalX(float x);
		virtual void setLocalY(float y);
		virtual void setLocalZ(float z);

		virtual void setLocalRotation(Vec3 localRot);
		virtual void setLocalRotation(float pitch, float yaw, float roll);
		virtual void setLocalPitch(float pitch);
		virtual void setLocalYaw(float yaw);
		virtual void setLocalRoll(float roll);

		virtual void setLocalScale(Vec3 localScale);
		virtual void setLocalScale(float x, float y, float z);
		virtual void setLocalScaleX(float x);
		virtual void setLocalScaleY(float y);
		virtual void setLocalScaleZ(float z);

		// Local adders
		virtual void addToLocalPos(Vec3 addedLocalPos);
		virtual void addToLocalPos(float x, float y, float z);
		virtual void addToLocalX(float x);
		virtual void addToLocalY(float y);
		virtual void addToLocalZ(float z);

		virtual void addToLocalRotation(Vec3 addedLocalRot);
		virtual void addToLocalRotation(float pitch, float yaw, float roll);
		virtual void addToLocalPitch(float pitch);
		virtual void addToLocalYaw(float yaw);
		virtual void addToLocalRoll(float roll);

		virtual void addToLocalScale(Vec3 addedLocalScale);
		virtual void addToLocalScale(float x, float y, float z);
		virtual void addToLocalScaleX(float x);
		virtual void addToLocalScaleY(float y);
		virtual void addToLocalScaleZ(float z);

		// Local getters
		virtual Vec3 getLocalPos() const;
		virtual float getLocalX() const;
		virtual float getLocalY() const;
		virtual float getLocalZ() const;

		virtual Vec3 getLocalRot() const;
		virtual float getLocalPitch() const;
		virtual float getLocalYaw() const;
		virtual float getLocalRoll() const;

		virtual Vec3 getLocalScale() const;
		virtual float getLocalScaleX() const;
		virtual float getLocalScaleY() const;
		virtual float getLocalScalaZ() const;

		// World gettters
		virtual Vec3 getWorldPos();

		// Matrices
		virtual Mat4 getRotationMatrix(); // TODO: Think about a more efficient way to minimize matrix constructions
		virtual Mat4 getModelMatrix();
		virtual Mat3 getNormalMatrix();

	private:
		inline void rebuildMatrices();
		inline Mat3 getNormalMatrixRecursive();

	protected:
		Vec3 localPos;
		Vec3 localRot; // The order of rotation is 1º-roll, 2º-pitch, 3º-yaw
		Vec3 localScale;

		bool dirtyMatrices;
		Mat4 rotationMatrix;
		Mat4 modelMatrix;
		Mat3 modelMatrixNoTranslations;
	};
}