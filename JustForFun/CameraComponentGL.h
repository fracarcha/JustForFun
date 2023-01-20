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

#include "CameraComponent.h"

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

namespace JFF
{
	class CameraComponentGL : public CameraComponent
	{
	public:
		// Ctor & Dtor
		CameraComponentGL(GameObject* const gameObject, const char* name, bool initiallyEnabled, bool activeCameraOnStart = false);
		virtual ~CameraComponentGL();

		// Copy ctor and copy assignment
		CameraComponentGL(const CameraComponentGL& other) = delete;
		CameraComponentGL& operator=(const CameraComponentGL& other) = delete;

		// Move ctor and assignment
		CameraComponentGL(CameraComponentGL&& other) = delete;
		CameraComponentGL operator=(CameraComponentGL&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() override;
		virtual void onUpdate() override;
		virtual void onDestroy() noexcept override;

		// ------------------------------- CAMERA COMPONENT GL OVERRIDES ------------------------------- //

			// Set this camera as active for rendering. Do not confuse with Component::setEnabled()
		virtual void setActiveCamera() override;

		// Changes projection matrix to orthographic mode. CAREFUL! zNear tends to be negative and zFar positive
		virtual void setOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar) override;

		// Changes projection matrix to perspective mode
		virtual void setPerspectiveProjection(float FOVDeg, float aspectRatio, float zNear, float zFar) override;

		// Gets the view matrix of this camera.
		virtual Mat4 getViewMatrix() const override;

		// Gets the projection matrix of this camera.
		virtual Mat4 getProjectionMatrix() const override;

	private: // Helper functions
		inline void createUBO();
		inline void generateViewMatrix();

	protected:
		bool activeCameraOnStart;
		Mat4 projectionMatrix;
		Mat4 viewMatrix;
		GLuint ubo; // Uniform Buffer Object

		bool dirtyProjectionMatrix;

		GLintptr viewMatrixOffset;
		GLintptr projectionMatrixOffset;
		GLintptr camWorldPosOffset;
	};
}