/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "GLCamera.h"
#include "GLM/gtc/matrix_transform.hpp"

glm::vec2 JFF::GLCamera::mousePos(0.0f);
glm::vec2 JFF::GLCamera::mouseDir(0.0f);

JFF::GLCamera::GLCamera() :
	camPos(0.0f),
	camRot(0.0f)
{
	setPerspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
}

void JFF::GLCamera::setPosition(float x, float y, float z)
{
	camPos = glm::vec3(x, y, z);
}

void JFF::GLCamera::addPosition(float x, float y, float z)
{
	glm::mat4 matrix(1.0f);

	matrix = glm::rotate(matrix, glm::radians(camRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	matrix = glm::rotate(matrix, glm::radians(camRot.y), glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(camRot.x), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec4 addedPos = matrix * glm::vec4(x, y, z, 1.0f);

	camPos += glm::vec3(addedPos);
}

void JFF::GLCamera::setRotation(float yawDeg, float pitchDeg, float rollDeg)
{
	camRot = glm::vec3(yawDeg, pitchDeg, rollDeg);
}

void JFF::GLCamera::addRotation(float yawDeg, float pitchDeg, float rollDeg)
{
	camRot += glm::vec3(yawDeg, pitchDeg, rollDeg);
}

void JFF::GLCamera::setOrthographic(float left, float right, float bottom, float top, float near, float far)
{
	projection = glm::ortho(left, right, bottom, top, near, far);
}

void JFF::GLCamera::setPerspective(float FOVDeg, float aspectRatio, float near, float far)
{
	projection = glm::perspective(glm::radians(FOVDeg), aspectRatio, near, far);
}

glm::mat4 JFF::GLCamera::getViewMatrix() const
{
	glm::mat4 matrix(1.0f);

	matrix = glm::rotate(matrix, glm::radians(camRot.z), -glm::vec3(0.0f, 0.0f, 1.0f));
	matrix = glm::rotate(matrix, glm::radians(camRot.y), -glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(camRot.x), -glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::translate(matrix, -camPos);

	return matrix;
}

glm::mat4 JFF::GLCamera::getProjectionMatrix() const
{
	return projection;
}

void JFF::GLCamera::updateCursorPos(float xPos, float yPos)
{
	glm::vec2 prevPos = GLCamera::mousePos;
	GLCamera::mousePos = glm::vec2(xPos, yPos);
	GLCamera::mouseDir = mousePos - prevPos;
}

glm::vec2 JFF::GLCamera::getCursorPos()
{
	glm::vec2 pos = mousePos;
	mousePos = glm::vec2(0.0f);
	return pos;
}

glm::vec2 JFF::GLCamera::getCursorDirection()
{
	glm::vec2 dir = mouseDir;
	mouseDir = glm::vec2(0.0f);
	return dir;
}
