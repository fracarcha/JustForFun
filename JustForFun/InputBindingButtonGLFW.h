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
	class InputBindingButtonGLFW : public InputBindingButton
	{
	public:
		// Ctor & Dtor
		explicit InputBindingButtonGLFW(
			const std::string& name, 
			Engine* const engine,
			InputActionButton* const parentAction, 
			Mapping inputMapping);
		virtual ~InputBindingButtonGLFW();

		// Copy ctor and copy assignment
		InputBindingButtonGLFW(const InputBindingButtonGLFW& other) = delete;
		InputBindingButtonGLFW& operator=(const InputBindingButtonGLFW& other) = delete;

		// Move ctor and assignment
		InputBindingButtonGLFW(InputBindingButtonGLFW&& other) = delete;
		InputBindingButtonGLFW operator=(InputBindingButtonGLFW&& other) = delete;

		// -------------------------------- INPUT BINDING BASE INTERFACE -------------------------------- //

		virtual std::string getName() const override { return name; }
		virtual void resetAccumulators() override;

		// -------------------------------- INPUT BINDING INTERFACE -------------------------------- //

		virtual void setBehavior(const std::shared_ptr<InputBehaviorButton>& behavior) override;
		virtual void addProcessor(const std::shared_ptr<InputProcessorButton>& processor) override;

		// -------------------------------- HELPER FUNCTION OVERRIDES -------------------------------- //

	private:
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

		inline void setActionValueIfChangedWASD(int key, int action, bool inputValue, Input::Hardware hw);
		inline void setActionValueIfChangedArrowKeys(int key, int action, bool inputValue, Input::Hardware hw);
		inline void setActionValueIfChangedGamepad(unsigned char start, unsigned char back,
			unsigned char cross, unsigned char circle, unsigned char square, unsigned char triangle, 
			unsigned char dpadUp, unsigned char dpadDown, unsigned char dpadLeft, unsigned char dpadRight, 
			unsigned char leftBumper, unsigned char rightBumper, unsigned char leftThumb, unsigned char rightThumb, 
			bool inputValue, Input::Hardware hw);
		inline void setActionValueIfChangedDpad(unsigned char dpadUp, unsigned char dpadDown, unsigned char dpadLeft, unsigned char dpadRight,
			bool inputValue, Input::Hardware hw);
		inline void setActionValueCheckingAnyBehavior(int key, int newAction, bool inputValue, Input::Hardware hw);
		inline void setActionValueCheckingBehavior(int newAction, bool inputValue, Input::Hardware hw);
		inline void setActionValueCheckingBehavior(bool inputValue, Input::Hardware hw);
		inline void setActionValueIfChanged(int newAction, bool inputValue, Input::Hardware hw);
		inline void setActionValueIfAnyChanged(int newKey, int newAction, bool inputValue, Input::Hardware hw);
		inline void setActionValue(bool inputValue, Input::Hardware hw);

		inline bool applyProcessors(bool inputValue);

	protected:
		Engine* engine;
		GLFWwindow* window;
		InputGLFW* input;
		InputActionButton* parentAction;

		const std::string name;
		const Mapping inputMapping;
		MappingType mappingType;

		// Callback helpers
		bool firstMouseCapture; // Used to prevent a huge 'jump' on the first mouse position capture
		Vec2 lastMousePos;
		Vec2 mouseDeltaAccum; // Used to accumulate position of mouse pos events, which occurs many times in a frame

		// Behavior
		std::shared_ptr<InputBehaviorButton> behavior;

		// Processors
		std::vector<std::shared_ptr<InputProcessorButton>> processors;

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