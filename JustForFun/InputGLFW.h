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

#include "Input.h"
#include "InputBindingBase.h"
//#include "INIFile.h" // TODO remove

#include <functional>
#include <vector>
#include <map>

struct GLFWgamepadstate;
struct GLFWwindow;

namespace JFF
{
	// GLFW implementation of Input
	class InputGLFW : public Input
	{
	public:
		// Ctor & Dtor
		InputGLFW();
		virtual ~InputGLFW();

		// Copy ctor and copy assignment
		InputGLFW(const InputGLFW& other) = delete;
		InputGLFW& operator=(const InputGLFW& other) = delete;

		// Move ctor and assignment
		InputGLFW(InputGLFW&& other) = delete;
		InputGLFW operator=(InputGLFW&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ExecutableSubsystem impl
		virtual ExecutableSubsystem::ExecutionOrder getExecutionOrder() const override;
		virtual bool execute() override;

		// --------------------------------------- INPUT IMPL --------------------------------------- //

		// Enables or disables the current active action set
		virtual void setEnabled(bool enabled) override;
		
		/*
		* Enables the action set with given name.
		* Other active action set will be disabled.
		* Only the active action set will process inputs
		*/
		virtual void setActiveInputActionSet(const std::string& name) override;

		/*
		* Mark as active the action with given name.
		* The action must be part of the active input action set.
		* All active actions will be executed on next call to execute() function
		*/
		virtual void markActiveAction(const std::string& actionName) override;

		/*
		* Defines the behavior of the mouse within this application. There are three options:
		* * NORMAL: The mouse shows and behaves normally.
		* * HIDDEN: The mouse is invisible while is inside the application area, but behaves normally and can leave the area.
		* * DISABLED: The mouse is hidden and attached to the application area, thus providing ilimited cursor movement
		*/
		virtual void setCursorMode(CursorMode mode) override;

		// ---------------- Input listeners ---------------- //

		virtual void addListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp,
			const AxesListener& listener) override;

		virtual void addListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp,
			const ButtonListener& listener) override;

		virtual void addListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp,
			const TriggerListener& listener) override;

		virtual void removeAxesListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp) override;

		virtual void removeButtonListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp) override;

		virtual void removeTriggerListener(
			const std::string& inputActionSetName,
			const std::string& actionName,
			const InputComponent* const inputComp) override;

		// --------------------------------------- INPUT GLFW HELPER FUNCTIONS --------------------------------------- //

		using KeyCallback = std::function<void(int key, int action, int mods)>;
		using MousePosCallback = std::function<void(double xpos, double ypos)>;
		using MouseInputCallback = std::function<void(int button, int action, int mods)>;
		using MouseScrollCallback = std::function<void(double xoffset, double yoffset)>;
		using GamepadButtonCallback = std::function<void(const GLFWgamepadstate& gamepadState)>;
		using GamepadAxesCallback = GamepadButtonCallback;

		virtual void subscribeToKeyInputs(const InputBindingBase* const binding, const KeyCallback& callback);
		virtual void unsubscribeToKeyInputs(const InputBindingBase* const binding);

		virtual void subscribeToMousePos(const InputBindingBase* const binding, const MousePosCallback& callback);
		virtual void unsubscribeToMousePos(const InputBindingBase* const binding);

		virtual void subscribeToMouseInputs(const InputBindingBase* const binding, const MouseInputCallback& callback);
		virtual void unsubscribeToMouseInputs(const InputBindingBase* const binding);

		virtual void subscribeToMouseScroll(const InputBindingBase* const binding, const MouseScrollCallback& callback);
		virtual void unsubscribeToMouseScroll(const InputBindingBase* const binding);

		virtual void subscribeToGamepadButtonInputs(const InputBindingBase* const binding, const GamepadButtonCallback& callback);
		virtual void unsubscribeToGamepadButtonInputs(const InputBindingBase* const binding);

		virtual void subscribeToGamepadAxesInputs(const InputBindingBase* const binding, const GamepadAxesCallback& callback);
		virtual void unsubscribeToGamepadAxesInputs(const InputBindingBase* const binding);

	public:
		friend class GLFWInputCallbackAdaptor;
		using ActionSet = std::map<std::string, std::shared_ptr<InputActionBase>>;

	private:
		struct Params
		{
			bool enabled;
			CursorMode cursorMode;
		};
		inline Params loadConfigFile() const;
		inline void detectJoysticks();
		inline std::shared_ptr<InputActionBase> getInputAction(
			const std::string& inputActionSetName, 
			const std::string& actionName) const;
		inline void loadInputsFromFile();

	protected:
		Engine* engine;

		bool enabled;
		std::map<std::string, std::shared_ptr<ActionSet>> actionSets;
		std::weak_ptr<ActionSet> activeActionSet;

		std::map<std::string, std::weak_ptr<InputActionBase>> activeActions;

		// Callbacks
		std::vector<std::shared_ptr<InputBindingBase>> inputBindings; // List of all input bindings
		std::map<const InputBindingBase* const, KeyCallback> keyCallbacks;
		std::map<const InputBindingBase* const, MousePosCallback> mousePosCallbacks;
		std::map<const InputBindingBase* const, MouseInputCallback> mouseInputCallbacks;
		std::map<const InputBindingBase* const, MouseScrollCallback> mouseScrollCallbacks;
		std::map<const InputBindingBase* const, GamepadButtonCallback> gamepadButtonCallbacks;
		std::map<const InputBindingBase* const, GamepadAxesCallback> gamepadAxesCallbacks;

		// Joysticks
		std::vector<int> connectedGamepads;
	};

	/*
	* Static class intended to use as GLFW callback function holder.
	* It would be much easier if GLFW would allow std::function as callback, but it's a C style library
	* and we have to work the hard way
	*/
	class GLFWInputCallbackAdaptor final
	{
	private: // Singleton objects need to hide constructors, cpoies, moves and assignments
		explicit GLFWInputCallbackAdaptor(InputGLFW* const input);

	public:
		~GLFWInputCallbackAdaptor();

		// Copy ctor and copy assignment
		GLFWInputCallbackAdaptor(const GLFWInputCallbackAdaptor& other) = delete;
		GLFWInputCallbackAdaptor& operator=(const GLFWInputCallbackAdaptor& other) = delete;

		// Move ctor and assignment
		GLFWInputCallbackAdaptor(GLFWInputCallbackAdaptor&& other) = delete;
		GLFWInputCallbackAdaptor operator=(GLFWInputCallbackAdaptor&& other) = delete;

		// Singleton creation
		static GLFWInputCallbackAdaptor& getInstance(InputGLFW* const input = nullptr);

		// Input callbacks
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
		static void mouseInputCallback(GLFWwindow* window, int button, int action, int mods);
		static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		// Joystick connection and disconnection callback
		static void joystickConnectionCallback(int jid, int joystickEvent);

	private:
		InputGLFW* input;
	};
}