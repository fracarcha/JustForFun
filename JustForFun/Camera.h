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
#include "CameraComponent.h"

#include <memory>

namespace JFF
{
	class Camera : public Subsystem
	{
	public:
		// Ctor & Dtor
		Camera() {}
		virtual ~Camera() {}

		// Copy ctor and copy assignment
		Camera(const Camera& other) = delete;
		Camera& operator=(const Camera& other) = delete;

		// Move ctor and assignment
		Camera(Camera&& other) = delete;
		Camera operator=(Camera&& other) = delete;

		// -------------------------------- CAMERA INTERFACE -------------------------------- //

		// Sets the active camera. This will remove any previous active camera
		virtual void setActiveCamera(CameraComponent* const camera) = 0;
		
		// Reset the active camera, if there's any
		virtual void resetActiveCamera() = 0;

		// Returns true if there's an active camera. Returns false if there's an active camera, but the component is disabled
		virtual bool hasAnyActiveCamera() const = 0;

		// Returns true if given camera is the active camera
		virtual bool isCameraActive(const CameraComponent* const camera) const = 0;

		// Gets the active camera, if there's any, or an empty weak pointer otherwise
		virtual std::weak_ptr<CameraComponent> getActiveCamera() const = 0;

		/*
		* Gets the view matrix associated to the active camera.
		* If there isn't any active camera, this function returns the identity matrix
		*/
		virtual Mat4 getActiveCameraViewMatrix() const = 0;

		/*
		* Gets the projection matrix associated to the active camera.
		* If there isn't any active camera, this function returns the identity matrix
		*/
		virtual Mat4 getActiveCameraProjectionMatrix() const = 0;

		/*
		* Gets the camera's world position.
		* If there isn't any active camera, this function returns Vec3(0,0,0)
		*/
		virtual Vec3 getActiveCameraWorldPos() const = 0;
	};
}