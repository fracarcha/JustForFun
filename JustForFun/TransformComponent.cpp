/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "TransformComponent.h"

#include "Engine.h"
#include "Log.h"

JFF::TransformComponent::TransformComponent(
	GameObject* const gameObject,
	const char* name,
	bool initiallyEnabled,
	const Vec3& localPosition, 
	const Vec3& localRotation, 
	const Vec3& localScale) :
	Component(gameObject, name, initiallyEnabled),
	
	localPos(localPosition),
	localRot(localRotation),
	localScale(localScale),
	
	dirtyMatrices(true),
	rotationMatrix(),
	modelMatrix(),
	modelMatrixNoTranslations()
{
	JFF_LOG_INFO("Ctor TransformComponent")
}

JFF::TransformComponent::~TransformComponent()
{
	JFF_LOG_INFO("Dtor TransformComponent")
}

void JFF::TransformComponent::setLocalPos(Vec3 localPos)
{
	this->localPos = localPos;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalPos(float x, float y, float z)
{
	localPos.x = x;
	localPos.y = y;
	localPos.z = z;

	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalX(float x)
{
	localPos.x = x;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalY(float y)
{
	localPos.y = y;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalZ(float z)
{
	localPos.z = z;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalRotation(Vec3 localRot)
{
	this->localRot = localRot;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalRotation(float pitch, float yaw, float roll)
{
	localRot.pitch = pitch;
	localRot.yaw = yaw;
	localRot.roll = roll;

	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalPitch(float pitch)
{
	localRot.pitch = pitch;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalYaw(float yaw)
{
	localRot.yaw = yaw;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalRoll(float roll)
{
	localRot.roll = roll;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalScale(Vec3 localScale)
{
	this->localScale = localScale;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalScale(float x, float y, float z)
{
	localScale.x = x;
	localScale.y = y;
	localScale.z = z;

	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalScaleX(float x)
{
	localScale.x = x;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalScaleY(float y)
{
	localScale.y = y;
	dirtyMatrices = true;
}

void JFF::TransformComponent::setLocalScaleZ(float z)
{
	localScale.z = z;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalPos(Vec3 addedLocalPos)
{
	localPos += addedLocalPos;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalPos(float x, float y, float z)
{
	localPos.x += x;
	localPos.y += y;
	localPos.z += z;

	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalX(float x)
{
	localPos.x += x;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalY(float y)
{
	localPos.y += y;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalZ(float z)
{
	localPos.z += z;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalRotation(Vec3 addedLocalRot)
{
	localRot += addedLocalRot;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalRotation(float pitch, float yaw, float roll)
{
	localRot.pitch += pitch;
	localRot.yaw += yaw;
	localRot.roll += roll;

	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalPitch(float pitch)
{
	localRot.pitch += pitch;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalYaw(float yaw)
{
	localRot.yaw += yaw;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalRoll(float roll)
{
	localRot.roll += roll;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalScale(Vec3 addedLocalScale)
{
	localScale += addedLocalScale;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalScale(float x, float y, float z)
{
	localScale.x += x;
	localScale.y += y;
	localScale.z += z;

	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalScaleX(float x)
{
	localScale.x += x;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalScaleY(float y)
{
	localScale.y += y;
	dirtyMatrices = true;
}

void JFF::TransformComponent::addToLocalScaleZ(float z)
{
	localScale.z += z;
	dirtyMatrices = true;
}

JFF::Vec3 JFF::TransformComponent::getLocalPos() const
{
	return localPos;
}

float JFF::TransformComponent::getLocalX() const
{
	return localPos.x;
}

float JFF::TransformComponent::getLocalY() const
{
	return localPos.y;
}

float JFF::TransformComponent::getLocalZ() const
{
	return localPos.z;
}

JFF::Vec3 JFF::TransformComponent::getLocalRot() const
{
	return localRot;
}

float JFF::TransformComponent::getLocalPitch() const
{
	return localRot.pitch;
}

float JFF::TransformComponent::getLocalYaw() const
{
	return localRot.yaw;
}

float JFF::TransformComponent::getLocalRoll() const
{
	return localRot.roll;
}

JFF::Vec3 JFF::TransformComponent::getLocalScale() const
{
	return localScale;
}

float JFF::TransformComponent::getLocalScaleX() const
{
	return localScale.x;
}

float JFF::TransformComponent::getLocalScaleY() const
{
	return localScale.y;
}

float JFF::TransformComponent::getLocalScalaZ() const
{
	return localScale.z;
}

JFF::Vec3 JFF::TransformComponent::getWorldPos()
{
	Vec4 worldPosVec4 = getModelMatrix() * Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	return Vec3(worldPosVec4.x, worldPosVec4.y, worldPosVec4.z);
}

JFF::Mat4 JFF::TransformComponent::getRotationMatrix()
{
	if (dirtyMatrices)
	{
		rebuildMatrices();
		dirtyMatrices = false;
	}

	if (gameObject->parent.expired()) // The game object has no parent
	{
		return rotationMatrix;
	}
	else // The game object has parent
	{
		return gameObject->parent.lock()->transform.getRotationMatrix() * rotationMatrix;
	}
}

JFF::Mat4 JFF::TransformComponent::getModelMatrix()
{
	if (dirtyMatrices)
	{
		rebuildMatrices();
		dirtyMatrices = false;
	}

	if (gameObject->parent.expired()) // The game object has no parent
	{
		return modelMatrix;
	}
	else // The game object has parent
	{
		return gameObject->parent.lock()->transform.getModelMatrix() * modelMatrix;
	}
}

JFF::Mat3 JFF::TransformComponent::getNormalMatrix()
{
	// Extract Math subsystem
	std::shared_ptr<Math> math = gameObject->engine->math.lock();

	// To get more info about why next line builds a normal matrix, check: http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
	return math->transpose(math->inverse(getNormalMatrixRecursive()));
}

inline void JFF::TransformComponent::rebuildMatrices()
{
	// Extract Math subsystem
	std::shared_ptr<Math> math = gameObject->engine->math.lock();
	Mat4 identityMatrix;

	// Build a local rotation matrix (remember the order of application is reversed of the order of next lines)
	rotationMatrix = JFF::rotate(identityMatrix, math->radians(localRot.yaw), Vec3::UP); // 3º-yaw
	rotationMatrix = JFF::rotate(rotationMatrix, math->radians(localRot.pitch), Vec3::RIGHT); // 2º-pitch
	rotationMatrix = JFF::rotate(rotationMatrix, math->radians(localRot.roll), Vec3::FORWARD); // 1º-roll

	// Build a local model matrix (remember the order of application is reversed of the order of next lines)
	modelMatrix = JFF::translate(identityMatrix, localPos); // Last transformation is translate
	modelMatrix *= rotationMatrix;
	modelMatrix = JFF::scale(modelMatrix, localScale); // Scale is applied first

	// Remove translations from model
	modelMatrixNoTranslations = math->reduceOrder(modelMatrix);
}

inline JFF::Mat3 JFF::TransformComponent::getNormalMatrixRecursive()
{
	if (dirtyMatrices)
	{
		rebuildMatrices();
		dirtyMatrices = false;
	}

	if (gameObject->parent.expired()) // The game object has no parent
	{
		return modelMatrixNoTranslations;
	}
	else // The game object has parent
	{
		return gameObject->parent.lock()->transform.getNormalMatrixRecursive() * modelMatrixNoTranslations;
	}
}
