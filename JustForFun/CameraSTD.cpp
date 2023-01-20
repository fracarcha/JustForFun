/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "CameraSTD.h"

#include "GameObject.h"
#include "Log.h"

JFF::CameraSTD::CameraSTD() : 
	activeCamera(nullptr)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: CameraSTD")
}

JFF::CameraSTD::~CameraSTD()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: CameraSTD")
}

void JFF::CameraSTD::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: CameraSTD")
}

void JFF::CameraSTD::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: CameraSTD")
}

JFF::Subsystem::UnloadOrder JFF::CameraSTD::getUnloadOrder() const
{
	return UnloadOrder::CAMERA;
}

void JFF::CameraSTD::setActiveCamera(CameraComponent* const camera)
{
	activeCamera = camera;
}

void JFF::CameraSTD::resetActiveCamera()
{
	activeCamera = nullptr;
}

bool JFF::CameraSTD::hasAnyActiveCamera() const
{
	return activeCamera && activeCamera->isEnabled();
}

bool JFF::CameraSTD::isCameraActive(const CameraComponent* const camera) const
{
	return activeCamera == camera; // Compare pointer adresses
}

std::weak_ptr<JFF::CameraComponent> JFF::CameraSTD::getActiveCamera() const
{
	// Return empty weak pointer if there isn't any active camera
	if (!activeCamera)
		return std::weak_ptr<CameraComponent>();

	// Extract weak pointer from owner gameobject
	return activeCamera->gameObject->getComponent<CameraComponent>(activeCamera->getName()); // This consult should never fail
}

JFF::Mat4 JFF::CameraSTD::getActiveCameraViewMatrix() const
{
	// Return identity matrix if there isn't any active camera
	if (!activeCamera)
		return Mat4();

	return activeCamera->getViewMatrix();
}

JFF::Mat4 JFF::CameraSTD::getActiveCameraProjectionMatrix() const
{
	// Return identity matrix if there isn't any active camera
	if (!activeCamera)
		return Mat4();

	return activeCamera->getProjectionMatrix();
}

JFF::Vec3 JFF::CameraSTD::getActiveCameraWorldPos() const
{
	if (!activeCamera)
		return Vec3::ZERO;

	return activeCamera->gameObject->transform.getWorldPos();
}
