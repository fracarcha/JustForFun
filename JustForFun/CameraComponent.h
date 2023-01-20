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
#include "Mat.h"

namespace JFF
{
	class CameraComponent : public Component
	{
	public:
		// Ctor & Dtor
		CameraComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
			Component(gameObject, name, initiallyEnabled)
		{}
		virtual ~CameraComponent() {}

		// Copy ctor and copy assignment
		CameraComponent(const CameraComponent& other) = delete;
		CameraComponent& operator=(const CameraComponent& other) = delete;

		// Move ctor and assignment
		CameraComponent(CameraComponent&& other) = delete;
		CameraComponent operator=(CameraComponent&& other) = delete;

		// ------------------------------- CAMERA COMPONENT INTERFACE ------------------------------- /

		// Set this camera as active for rendering. Do not confuse with Component::setEnabled()
		virtual void setActiveCamera() = 0;

		// Changes projection matrix to orthographic mode. CAREFUL! zNear tends to be negative and zFar positive
		virtual void setOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar) = 0;

		// Changes projection matrix to perspective mode
		virtual void setPerspectiveProjection(float FOVDeg, float aspectRatio, float zNear, float zFar) = 0;

		// Gets the view matrix of this camera.
		virtual Mat4 getViewMatrix() const = 0;

		// Gets the projection matrix of this camera.
		virtual Mat4 getProjectionMatrix() const = 0;
	};
}