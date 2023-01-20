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
#include "GLM/glm.hpp"

namespace JFF
{
	class GLCamera2 final
	{
	public:
		GLCamera2();

		// Position, movement and rotation
		void setPosition(float x, float y, float z);
		void move(float forward, float right, float up);
		void rotateCursor(float rotationSpeed);

		// Setting projection
		void setOrthographic(float left, float right, float bottom, float top, float near, float far);
		void setPerspective(float FOVDeg, float aspectRatio, float near, float far);

		// Send matrices to OPpenGL context
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;

		// Mouse cursor parameters (used for GLFW cursor callback)
		static void updateCursorDir(float xPos, float yPos);
		glm::vec2 getCursorDirection(); // Only once get

		static void updateMouseWheel(float yOffset);
		float getMouseWheelFOV();

	private:
		glm::vec3 camPos;	// Position

		glm::vec3 worldUp;	// Direction relative to world
		glm::vec3 camFront; // Direction

		float yaw, pitch;

		glm::mat4 projection;

		static glm::vec2 mousePos, mouseDir;
		static float mouseYOffset;
	};
}