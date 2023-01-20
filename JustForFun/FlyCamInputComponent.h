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

#include "InputComponent.h"
#include "Vec.h"

namespace JFF
{
	class FlyCamInputComponent : public InputComponent
	{
	public:
		// Ctor & Dtor
		FlyCamInputComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled);
		virtual ~FlyCamInputComponent();

		// Copy ctor and copy assignment
		FlyCamInputComponent(const FlyCamInputComponent& other) = delete;
		FlyCamInputComponent& operator=(const FlyCamInputComponent& other) = delete;

		// Move ctor and assignment
		FlyCamInputComponent(FlyCamInputComponent&& other) = delete;
		FlyCamInputComponent operator=(FlyCamInputComponent&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() override;
		//virtual void onEnable() override;
		virtual void onUpdate() override;
		//virtual void onDisable() noexcept override;
		virtual void onDestroy() noexcept override;

	protected:
		// Input		
		Vec4 moveDir;
		float moveUp;
		float moveDown;
		
		Vec3 rotation;

		// Variables
		float maxSpeed;
		Vec3 speed;
		float boost;
		float boostMaxSpeed;

		// Constants
		const float accelerationFactor;
		const float brakeFactor;
		const float speedThreshold;
		
		const float boostAccelerationFactor;
		const float boostMaxSpeedFactor;

		const float gamepadRotationSensitivity;
		const float mouseRotationSensitivity;

		// Active scheme. Detects which input is being used
		enum class InputScheme
		{
			KEYBOARD_AND_MOUSE,
			GAMEPAD,
		} activeScheme;
	};
}