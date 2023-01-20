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
	class GLCamera final
	{
	public:
		GLCamera();

		// Setting the camera
		void setPosition(float x, float y, float z);
		void addPosition(float x, float y, float z);
		void setRotation(float yawDeg, float pitchDeg, float rollDeg);
		void addRotation(float yawDeg, float pitchDeg, float rollDeg);
		
		// Setting projection
		void setOrthographic(float left, float right, float bottom, float top, float near, float far);
		void setPerspective(float FOVDeg, float aspectRatio, float near, float far);

		// Send matrices to OPpenGL context
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix() const;

		// Mouse cursor parameters (used for GLFW cursor callback)
		static void updateCursorPos(float xPos, float yPos);
		glm::vec2 getCursorPos(); // Only once get
		glm::vec2 getCursorDirection(); // Only once get

	private:
		glm::vec3 camPos;
		glm::vec3 camRot; // x:yaw, y:pitch, z:roll 

		glm::mat4 projection;

		static glm::vec2 mousePos;
		static glm::vec2 mouseDir;
	};
}