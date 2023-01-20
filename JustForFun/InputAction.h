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

#include "InputActionBase.h"
#include "InputComponent.h"

#include "Engine.h"
#include <map>
#include <functional>

namespace JFF
{
	template<typename _Ret>
	class InputAction : public InputActionBase
	{
	public:
		// Ctor & Dtor
		explicit InputAction(const std::string& name, Engine* const engine);
		virtual ~InputAction();

		// Copy ctor and copy assignment
		InputAction(const InputAction& other) = delete;
		InputAction& operator=(const InputAction& other) = delete;

		// Move ctor and assignment
		InputAction(InputAction&& other) = delete;
		InputAction operator=(InputAction&& other) = delete;

		// -------------------------------- INPUT ACTION BASE OVERRIDES -------------------------------- //

		virtual std::string getName() const override;
		virtual void execute() override;

		// -------------------------------- INPUT ACTION INTERFACE -------------------------------- //

		virtual void setActionValue(const _Ret& value, Input::Hardware hw);

		virtual void addListener(const InputComponent* const inputComp, const std::function<void(const _Ret&, Input::Hardware hw)>& listener);
		virtual void removeListener(const InputComponent* const inputComp);

	protected:
		Engine* engine;

		std::string name;
		_Ret lastActionValue;
		Input::Hardware lastActionHardware; // The physical device who triggers the last input
		std::map<const InputComponent* const, std::function<void(const _Ret&, Input::Hardware)>> listeners;
	};

	// Template typical uses
	using InputActionAxes = InputAction<Vec2>;
	using InputActionButton = InputAction<bool>;
	using InputActionTrigger = InputAction<float>;
}

// Template definitions
#include "InputAction.inl"