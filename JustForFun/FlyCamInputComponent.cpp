/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "FlyCamInputComponent.h"

#include "Engine.h"

JFF::FlyCamInputComponent::FlyCamInputComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
	InputComponent(gameObject, name, initiallyEnabled),

	moveDir(),
	moveUp(0.0f),
	moveDown(0.0f),

	rotation(),

	maxSpeed(5.0f),
	speed(),
	boost(1.0f),
	boostMaxSpeed(1.0f),

	accelerationFactor(10.0f),
	brakeFactor(5.0f),
	speedThreshold(0.1f),

	boostAccelerationFactor(4.0f),
	boostMaxSpeedFactor(5.0f),

	gamepadRotationSensitivity(200.0f),
	mouseRotationSensitivity(0.1f),

	activeScheme(InputScheme::KEYBOARD_AND_MOUSE)
{
	JFF_LOG_INFO("Ctor FlyCamInputComponent")
}

JFF::FlyCamInputComponent::~FlyCamInputComponent()
{
	JFF_LOG_INFO("Dtor FlyCamInputComponent")
}

void JFF::FlyCamInputComponent::onStart()
{
	// Subscribe to input events
	auto input = gameObject->engine->input.lock();

	// Move listeners
	Input::AxesListener moveListener = [this](const Vec2& value, Input::Hardware hw)
	{
		switch (activeScheme)
		{
		case JFF::FlyCamInputComponent::InputScheme::KEYBOARD_AND_MOUSE:
			if (hw == Input::Hardware::GAMEPAD && value != Vec2::ZERO)
			{
				activeScheme = InputScheme::GAMEPAD;
				moveDir.x = value.x;
				moveDir.z = value.y;
			}
			else if (hw == Input::Hardware::KEYBOARD)
			{
				moveDir.x = value.x;
				moveDir.z = value.y;
			}
			break;
		case JFF::FlyCamInputComponent::InputScheme::GAMEPAD:
			if (hw == Input::Hardware::KEYBOARD)
			{
				activeScheme = InputScheme::KEYBOARD_AND_MOUSE;
				moveDir.x = value.x;
				moveDir.z = value.y;
			}
			else if (hw == Input::Hardware::GAMEPAD)
			{
				moveDir.x = value.x;
				moveDir.z = value.y;
			}
			break;
		default:
			break;
		}

	};
	input->addListener("default", "move", this, moveListener);

	Input::TriggerListener moveUpListener = [this](const float& value, Input::Hardware hw)
	{
		if (hw == Input::Hardware::GAMEPAD)
			activeScheme = InputScheme::GAMEPAD;
		else
			activeScheme = InputScheme::KEYBOARD_AND_MOUSE;

		moveUp = value;
	};
	input->addListener("default", "moveup", this, moveUpListener);

	Input::TriggerListener moveDownListener = [this](const float& value, Input::Hardware hw)
	{
		if (hw == Input::Hardware::GAMEPAD)
			activeScheme = InputScheme::GAMEPAD;
		else
			activeScheme = InputScheme::KEYBOARD_AND_MOUSE;

		moveDown = value;
	};
	input->addListener("default", "movedown", this, moveDownListener);

	Input::TriggerListener moveAccelListener = [this](const float& value, Input::Hardware hw)
	{
		switch (activeScheme)
		{
		case JFF::FlyCamInputComponent::InputScheme::KEYBOARD_AND_MOUSE:
			if (hw == Input::Hardware::GAMEPAD && value > -1.0f)
			{
				activeScheme = InputScheme::GAMEPAD;
				float normalizedValue = (value + 1.0f) * 0.5f; // From [-1,1] to [0,1]
				boost = normalizedValue * (boostAccelerationFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostAccelerationFactor]
				boostMaxSpeed = normalizedValue * (boostMaxSpeedFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostMaxSpeedFactor]
			}
			else if(hw == Input::Hardware::KEYBOARD)
			{
				boost = value * (boostAccelerationFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostAccelerationFactor]
				boostMaxSpeed = value * (boostMaxSpeedFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostMaxSpeedFactor]
			}
			break;
		case JFF::FlyCamInputComponent::InputScheme::GAMEPAD:
			if (hw == Input::Hardware::KEYBOARD)
			{
				activeScheme = InputScheme::KEYBOARD_AND_MOUSE;
				boost = value * (boostAccelerationFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostAccelerationFactor]
				boostMaxSpeed = value * (boostMaxSpeedFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostMaxSpeedFactor]
			}
			else if(hw == Input::Hardware::GAMEPAD)
			{
				float normalizedValue = (value + 1.0f) * 0.5f; // From [-1,1] to [0,1]
				boost = normalizedValue * (boostAccelerationFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostAccelerationFactor]
				boostMaxSpeed = normalizedValue * (boostMaxSpeedFactor - 1.0f) + 1.0f; // boost range: [1.0f, boostMaxSpeedFactor]
			}
			break;
		default:
			break;
		}
	};
	input->addListener("default", "moveaccel", this, moveAccelListener);

	// Look listener
	Input::AxesListener lookListener = [this](const Vec2& value, Input::Hardware hw)
	{
		switch (activeScheme)
		{
		case JFF::FlyCamInputComponent::InputScheme::KEYBOARD_AND_MOUSE:
			if (hw == Input::Hardware::GAMEPAD && value != Vec2::ZERO)
			{
				activeScheme = InputScheme::GAMEPAD;

				// Add new rotation
				float deltaTime = (float)gameObject->engine->time.lock()->deltaTime();
				rotation.yaw += value.x * gamepadRotationSensitivity * deltaTime;
				rotation.pitch += value.y * gamepadRotationSensitivity * deltaTime;
			}
			else if (hw == Input::Hardware::MOUSE)
			{
				// Add new rotation
				rotation.yaw += value.x * mouseRotationSensitivity;
				rotation.pitch += value.y * mouseRotationSensitivity;
			}
			break;
		case JFF::FlyCamInputComponent::InputScheme::GAMEPAD:
			if (hw == Input::Hardware::MOUSE)
			{
				activeScheme = InputScheme::KEYBOARD_AND_MOUSE;

				// Add new rotation
				rotation.yaw += value.x * mouseRotationSensitivity;
				rotation.pitch += value.y * mouseRotationSensitivity;
			}
			else if (hw == Input::Hardware::GAMEPAD)
			{
				// Add new rotation
				float deltaTime = (float)gameObject->engine->time.lock()->deltaTime();
				rotation.yaw += value.x * gamepadRotationSensitivity * deltaTime;
				rotation.pitch += value.y * gamepadRotationSensitivity * deltaTime;
			}
			break;
		default:
			break;
		}
	};
	input->addListener("default", "look", this, lookListener);
}

void JFF::FlyCamInputComponent::onUpdate()
{
	float deltaTime = (float)gameObject->engine->time.lock()->deltaTime();
	auto math = gameObject->engine->math.lock();

	// Limit pitch to (-90, 90)º
	rotation.pitch = gameObject->engine->math.lock()->clamp(rotation.pitch, -89.9f, 89.9f);

	// Rotate this game object
	gameObject->transform.setLocalRotation(rotation);

	// Moving this gameobject keeping in mind the relative orientation
	moveDir.y = moveUp - moveDown;
	Vec4 rotatedDir = gameObject->transform.getRotationMatrix() * moveDir;

	// Calculate speed and acceleration
	Vec3 acceleration(rotatedDir.x, rotatedDir.y, rotatedDir.z);
	float currentMaxSpeed = math->length(rotatedDir) * maxSpeed * boostMaxSpeed;

	float speedMagnitude = math->length(speed);
	if (speedMagnitude > currentMaxSpeed)
	{
		Vec3 deceleration = speed * brakeFactor * deltaTime;
		speed -= deceleration;
	}
	else if (speedMagnitude < currentMaxSpeed)
	{
		
		acceleration *= accelerationFactor * boost * deltaTime;
		speed += acceleration;
		speed = math->normalize(acceleration) * math->length(speed); // Use acceleration direction and speed magnitude
	}

	speedMagnitude = math->length(speed);
	if (speedMagnitude > (currentMaxSpeed - speedThreshold) &&
		speedMagnitude < (currentMaxSpeed + speedThreshold))
	{
		speed = math->normalize(acceleration) * currentMaxSpeed;
	}

	// Set final position
	Vec3 deltaDistance = speed * deltaTime;
	gameObject->transform.addToLocalX(deltaDistance.x);
	gameObject->transform.addToLocalY(deltaDistance.y);
	gameObject->transform.addToLocalZ(deltaDistance.z);
}

void JFF::FlyCamInputComponent::onDestroy() noexcept
{
	// Unsubscribe to input events
	auto input = gameObject->engine->input.lock();

	input->removeAxesListener("default", "move", this);
	input->removeAxesListener("default", "look", this);
	input->removeTriggerListener("default", "moveup", this);
	input->removeTriggerListener("default", "movedown", this);
	input->removeTriggerListener("default", "moveaccel", this);
}
