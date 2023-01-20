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

#include "InputBinding.h"
#include "InputGLFW.h"

namespace JFF
{
	class InputBindingTriggerGLFW : public InputBindingTrigger
	{
	public:
		// Ctor & Dtor
		explicit InputBindingTriggerGLFW(
			const std::string& name, 
			Engine* const engine,
			InputActionTrigger* const parentAction, 
			Mapping inputMapping);
		virtual ~InputBindingTriggerGLFW();

		// Copy ctor and copy assignment
		InputBindingTriggerGLFW(const InputBindingTriggerGLFW& other) = delete;
		InputBindingTriggerGLFW& operator=(const InputBindingTriggerGLFW& other) = delete;

		// Move ctor and assignment
		InputBindingTriggerGLFW(InputBindingTriggerGLFW&& other) = delete;
		InputBindingTriggerGLFW operator=(InputBindingTriggerGLFW&& other) = delete;

		// -------------------------------- INPUT BINDING BASE INTERFACE -------------------------------- //

		virtual std::string getName() const override { return name; }
		virtual void resetAccumulators() override;

		// -------------------------------- INPUT BINDING INTERFACE -------------------------------- //

		virtual void setBehavior(const std::shared_ptr<InputBehaviorTrigger>& behavior) override;
		virtual void addProcessor(const std::shared_ptr<InputProcessorTrigger>& processor) override;

		// -------------------------------- HELPER FUNCTION OVERRIDES -------------------------------- //

	protected:
		virtual void initSubscriptions() override;
		virtual void cancelSubscriptions() override;

	private:
		enum class MappingType : unsigned char
		{
			UNKNOWN,
			KEYBOARD,
			MOUSE_POS,
			MOUSE_SCROLL,
			MOUSE_INPUT,
			GAMEPAD_AXES,
			GAMEPAD_BUTTON,
		};
		 
		inline InputGLFW::KeyCallback createKeyCallback();
		inline InputGLFW::MousePosCallback createMousePosCallback();
		inline InputGLFW::MouseInputCallback createMouseInputCallback();
		inline InputGLFW::MouseScrollCallback createMouseScrollCallback();
		inline InputGLFW::GamepadButtonCallback createGamepadButtonCallback();
		inline InputGLFW::GamepadAxesCallback createGamepadAxesCallback();
		 
		inline void setActionValueIfChangedWASD(int key, int action, float inputValue, Input::Hardware hw);
		inline void setActionValueIfChangedArrowKeys(int key, int action, float inputValue, Input::Hardware hw);
		inline void setActionValueIfChangedGamepad(unsigned char start, unsigned char back, 
			unsigned char cross, unsigned char circle, unsigned char square, unsigned char triangle, 
			unsigned char dpadUp, unsigned char dpadDown, unsigned char dpadLeft, unsigned char dpadRight, 
			unsigned char leftBumper, unsigned char rightBumper, unsigned char leftThumb, unsigned char rightThumb, 
			float inputValue, Input::Hardware hw);
		inline void setActionValueIfChangedDpad(unsigned char dpadUp, unsigned char dpadDown, unsigned char dpadLeft, unsigned char dpadRight,
			float inputValue, Input::Hardware hw);
		inline void setActionValueCheckingAnyBehavior(int key, int newAction, float inputValue, Input::Hardware hw);
		inline void setActionValueCheckingBehavior(int newAction, float inputValue, Input::Hardware hw);
		inline void setActionValueCheckingBehavior(float inputValue, Input::Hardware hw);
		inline void setActionValueIfChanged(int newAction, float inputValue, Input::Hardware hw);
		inline void setActionValueIfAnyChanged(int newKey, int newAction, float inputValue, Input::Hardware hw);
		inline void setActionValue(float inputValue, Input::Hardware hw);

		inline float applyProcessors(float inputValue);

	protected:
		Engine* engine;
		GLFWwindow* window;
		InputGLFW* input;
		InputActionTrigger* parentAction;

		const std::string name;
		const Mapping inputMapping;
		MappingType mappingType;

		// Callback helpers
		bool firstMouseCapture; // Used to prevent a huge 'jump' on the first mouse position capture
		Vec2 lastMousePos;
		Vec2 mouseDeltaAccum; // Used to accumulate position of mouse pos events, which occurs many times in a frame

		// Behavior
		std::shared_ptr<InputBehaviorTrigger> behavior;

		// Processors
		std::vector<std::shared_ptr<InputProcessorTrigger>> processors;

		int lastKey;

		int lastAction;

		int lastActionUp;
		int lastActionDown;
		int lastActionLeft;
		int lastActionRight;

		int lastActionStart;
		int lastActionBack;

		int lastActionCross;
		int lastActionCircle;
		int lastActionTriangle;
		int lastActionSquare;

		int lastActionBumperLeft;
		int lastActionBumperRight;

		int lastActionThumbLeft;
		int lastActionThumbRight;
	};
}