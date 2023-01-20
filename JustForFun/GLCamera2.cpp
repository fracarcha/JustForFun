/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "GLCamera2.h"
#include "GLM/gtc/matrix_transform.hpp"

#include <iostream>

glm::vec2 JFF::GLCamera2::mousePos(0.0f);
glm::vec2 JFF::GLCamera2::mouseDir(0.0f);
float JFF::GLCamera2::mouseYOffset = 45.0f;

JFF::GLCamera2::GLCamera2() :
	camPos(0.0f),
	worldUp(0.0f, 1.0f, 0.0f),
	camFront(0.0f, 0.0f, -1.0f),
	yaw(-90.0f),
	pitch(0.0f)
{
	setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
}

void JFF::GLCamera2::setPosition(float x, float y, float z)
{
	camPos = glm::vec3(x, y, z);
}

void JFF::GLCamera2::move(float forward, float right, float up)
{
	// Calculate the rest of axes
	glm::vec3 camRight = glm::normalize(glm::cross(camFront, worldUp));
	glm::vec3 camUp = glm::normalize(glm::cross(camRight, camFront));

	glm::vec3 frontDir = forward * camFront;
	glm::vec3 rightDir = right * camRight;
	glm::vec3 upDir = up * camUp;
	camPos += frontDir + rightDir + upDir;
}

void JFF::GLCamera2::rotateCursor(float rotationSpeed)
{
	glm::mat4 identity(1.0f);
	glm::vec2 cursorDir = getCursorDirection();

	yaw += rotationSpeed * cursorDir.x;
	pitch = glm::clamp(pitch + rotationSpeed * -cursorDir.y, -89.0f, 89.0f);

	camFront.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	camFront.y = glm::sin(glm::radians(pitch));
	camFront.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

	camFront = glm::normalize(camFront);
}

void JFF::GLCamera2::setOrthographic(float left, float right, float bottom, float top, float near, float far)
{
	projection = glm::ortho(left, right, bottom, top, near, far);
}

void JFF::GLCamera2::setPerspective(float FOVDeg, float aspectRatio, float near, float far)
{
	projection = glm::perspective(glm::radians(FOVDeg), aspectRatio, near, far);
}

glm::mat4 JFF::GLCamera2::getViewMatrix() const
{
	return glm::lookAt(camPos, camPos + camFront, worldUp);
}

glm::mat4 JFF::GLCamera2::getProjectionMatrix() const
{
	return projection;
}

void JFF::GLCamera2::updateCursorDir(float xPos, float yPos)
{
	static bool initialized = false;
	if (initialized)
	{
		glm::vec2 prevPos = GLCamera2::mousePos;
		GLCamera2::mousePos = glm::vec2(xPos, yPos);
		GLCamera2::mouseDir = mousePos - prevPos;
	}
	else
	{
		initialized = true;
		GLCamera2::mousePos = glm::vec2(xPos, yPos);
		GLCamera2::mouseDir = glm::vec2(0.0f);
	}
}

glm::vec2 JFF::GLCamera2::getCursorDirection()
{
	glm::vec2 dir = mouseDir;
	mouseDir = glm::vec2(0.0f);
	return dir;
}

void JFF::GLCamera2::updateMouseWheel(float yOffset)
{
	mouseYOffset = glm::clamp(mouseYOffset + yOffset * 5.0f, 5.0f, 120.0f);
}

float JFF::GLCamera2::getMouseWheelFOV()
{
	return mouseYOffset;
}
