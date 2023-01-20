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

#include "ExecutableSubsystem.h"

#include "InputActionBase.h"
#include "InputComponent.h"
#include "Vec.h"

#include <memory>

namespace JFF
{
	class Input : public ExecutableSubsystem
	{
	public:
		enum class CursorMode
		{
			NORMAL,
			HIDDEN,
			DISABLED,
		};

		enum class Hardware
		{
			GAMEPAD,
			MOUSE,
			KEYBOARD,
		};

		// Ctor & Dtor
		Input() {}
		virtual ~Input() {}

		// Copy ctor and copy assignment
		Input(const Input& other) = delete;
		Input& operator=(const Input& other) = delete;

		// Move ctor and assignment
		Input(Input&& other) = delete;
		Input operator=(Input&& other) = delete;

		// --------------------------------------- INPUT IMPL --------------------------------------- //

		// Enables or disables the current active action set
		virtual void setEnabled(bool enabled) = 0;

		/*
		* Enables the action set with given name.
		* Other active action set will be disabled.
		* Only the active action set will process inputs
		*/
		virtual void setActiveInputActionSet(const std::string& name) = 0;

		/*
		* Mark as active the action with given name.
		* The action must be part of the active input action set.
		* All active actions will be executed on next call to execute() function
		*/
		virtual void markActiveAction(const std::string& actionName) = 0;

		/*
		* Defines the behavior of the mouse within this application. There are three options:
		* * NORMAL: The mouse shows and behaves normally.
		* * HIDDEN: The mouse is invisible while is inside the application area, but behaves normally and can leave the area.
		* * DISABLED: The mouse is hidden and attached to the application area, thus providing unlimited cursor movement
		*/
		virtual void setCursorMode(CursorMode mode) = 0;

		// ---------------- Input listeners ---------------- //

		using AxesListener = std::function<void(const Vec2&, Hardware)>;
		using ButtonListener = std::function<void(const bool&, Hardware)>;
		using TriggerListener = std::function<void(const float&, Hardware)>;

		virtual void addListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp,
			const AxesListener& listener) = 0;

		virtual void addListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp,
			const ButtonListener& listener) = 0;

		virtual void addListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp,
			const TriggerListener& listener) = 0;

		virtual void removeAxesListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp) = 0;

		virtual void removeButtonListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp) = 0;

		virtual void removeTriggerListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp) = 0;
	};
}