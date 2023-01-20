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

#include "Camera.h"

namespace JFF
{
	class CameraSTD : public Camera
	{
	public:
		// Ctor & Dtor
		CameraSTD();
		virtual ~CameraSTD();

		// Copy ctor and copy assignment
		CameraSTD(const CameraSTD& other) = delete;
		CameraSTD& operator=(const CameraSTD& other) = delete;

		// Move ctor and assignment
		CameraSTD(CameraSTD&& other) = delete;
		CameraSTD operator=(CameraSTD&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ---------------------------------------- CAMERA IMPL ----------------------------------------//
		// 
		// Sets the active camera. This will remove any previous active camera
		virtual void setActiveCamera(CameraComponent* const camera) override;

		// Reset the active camera, if there's any
		virtual void resetActiveCamera() override;

		// Returns true if there's an active camera. Returns false if there's an active camera, but the component is disabled
		virtual bool hasAnyActiveCamera() const override;

		// Returns true if given camera is the active camera
		virtual bool isCameraActive(const CameraComponent* const camera) const override;

		// Gets the active camera, if there's any, or an empty weak pointer otherwise
		virtual std::weak_ptr<CameraComponent> getActiveCamera() const override;

		/*
		* Gets the view matrix associated to the active camera.
		* If there isn't any active camera, this function returns the identity matrix
		*/
		virtual Mat4 getActiveCameraViewMatrix() const override;

		/*
		* Gets the projection matrix associated to the active camera.
		* If there isn't any active camera, this function returns the identity matrix
		*/
		virtual Mat4 getActiveCameraProjectionMatrix() const override;

		/*
		* Gets the camera's world position.
		* If there isn't any active camera, this function returns Vec3(0,0,0)
		*/
		virtual Vec3 getActiveCameraWorldPos() const override;

	protected:
		CameraComponent* activeCamera;
	};
}