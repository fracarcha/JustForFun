/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputGLFW.h"

#include "Log.h"
#include "Engine.h"
#include "ContextGLFW.h"
#include "INIFile.h"
#include "FileSystemSetup.h"

#include "InputAction.h"
#include "InputBinding.h"

#include "InputBehaviorHold.h"
#include "InputBehaviorMultipress.h"
#include "InputBehaviorPress.h"

#include "InputProcessorDeadZone.h"
#include "InputProcessorInverter.h"
#include "InputProcessorNormalizer.h"

#include <sstream>
#include <algorithm>
#include <stdexcept>

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"
#include "GLFW/glfw3.h"

extern std::shared_ptr<JFF::INIFile> createINIFile(const char* filepath);

JFF::InputGLFW::InputGLFW() : 
	engine(nullptr),

	actionSets(),
	enabled(false),
	activeActionSet(),
	activeActions(),

	inputBindings(),
	keyCallbacks(),
	mousePosCallbacks(),
	mouseInputCallbacks(),
	mouseScrollCallbacks(),
	gamepadButtonCallbacks(),
	gamepadAxesCallbacks(),

	connectedGamepads()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: InputGLFW")
}

JFF::InputGLFW::~InputGLFW()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: InputGLFW")

	// ----------------------------- Unsubscribe callbacks from GLFW ----------------------------- //
		
	// Perfectly valid casting because this class is GLFW dependant too
	GLFWwindow* window = std::dynamic_pointer_cast<ContextGLFW>(engine->context.lock())->getWindow();

	// Unbind adaptor's functions to GLFW's callback functions
	glfwSetKeyCallback(window, NULL);
	glfwSetCursorPosCallback(window, NULL);
	glfwSetMouseButtonCallback(window, NULL);
	glfwSetScrollCallback(window, NULL);

	// Cancel detection of joystick changes (hot connections and disconnections)
	glfwSetJoystickCallback(NULL);

	// Delete arrays in certain order to avoid null pointer exceptions
	inputBindings.clear();
	actionSets.clear();
}

void JFF::InputGLFW::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: InputGLFW")
		
	// Joystick initial detection. Any joystick change is detected in GLFW callback (see postLoad() function)
	detectJoysticks();
}

void JFF::InputGLFW::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: InputGLFW")

	this->engine = engine;

	// ------------------------------------ INI CONFIG FILE ------------------------------------ //

	// Load config file to set default behavior
	Params params = loadConfigFile();
	setEnabled(params.enabled);
	setCursorMode(params.cursorMode);

	// Load config file and configure all actions and mappings
	loadInputsFromFile();

	// ------------------------------------ GLFW CALLBACKS ------------------------------------ //

	// Perfectly valid casting because this class is GLFW dependant too
	GLFWwindow* window = std::dynamic_pointer_cast<ContextGLFW>(engine->context.lock())->getWindow();

	// Create the first instance of GLFWInputCallbackAdaptor and make it hold a pointer to this object
	GLFWInputCallbackAdaptor::getInstance(this);

	// Bind adaptor's functions to GLFW's callback functions
	glfwSetKeyCallback(window, GLFWInputCallbackAdaptor::keyCallback);
	glfwSetCursorPosCallback(window, GLFWInputCallbackAdaptor::mousePosCallback);
	glfwSetMouseButtonCallback(window, GLFWInputCallbackAdaptor::mouseInputCallback);
	glfwSetScrollCallback(window, GLFWInputCallbackAdaptor::mouseScrollCallback);

	// Detect joystick changes (hot connections and disconnections)
	glfwSetJoystickCallback(GLFWInputCallbackAdaptor::joystickConnectionCallback);
}

JFF::Subsystem::UnloadOrder JFF::InputGLFW::getUnloadOrder() const
{
	return UnloadOrder::INPUT;
}

JFF::ExecutableSubsystem::ExecutionOrder JFF::InputGLFW::getExecutionOrder() const
{
	return ExecutableSubsystem::ExecutionOrder::INPUT;
}

bool JFF::InputGLFW::execute()
{
	if (!enabled)
		return true; // Signal that the main loop should continue
	
	// Execute and clear active actions (except gamepad inputs)
	std::for_each(activeActions.begin(), activeActions.end(), [](const auto& pair)
		{
			pair.second.lock()->execute();
		});
	activeActions.clear();

	// Gamepad input detection GLFW. Need to be done by polling (it doesn't use callbacks)
	GLFWgamepadstate gamepadState;
	if (!connectedGamepads.empty() && glfwGetGamepadState(connectedGamepads[0], &gamepadState))
	{
		std::for_each(gamepadAxesCallbacks.begin(), gamepadAxesCallbacks.end(), [&gamepadState](const auto& pair)
			{
				pair.second(gamepadState);
			});

		std::for_each(gamepadButtonCallbacks.begin(), gamepadButtonCallbacks.end(), [&gamepadState](const auto& pair)
			{
				pair.second(gamepadState);
			});
	}

	// Execute and clear active actions (gamepad inputs only)
	// NOTE: Gamepad polling 'silences' all other inputs mapped on the same action. This is why gamepad actions
	// are dispatched after all other inputs coming from other hardware like keyboard or mouse
	std::for_each(activeActions.begin(), activeActions.end(), [](const auto& pair)
		{
			pair.second.lock()->execute();
		});
	activeActions.clear();

	// Clear all binding accumulators at the end of Input execution
	std::for_each(inputBindings.begin(), inputBindings.end(), [](const auto& binding)
		{
			binding->resetAccumulators();
		});

	return true; // Signal that the main loop should continue
}

void JFF::InputGLFW::setEnabled(bool enabled)
{
	this->enabled = enabled;
	activeActions.clear(); // Flush active actions on any change of Input
}

void JFF::InputGLFW::setActiveInputActionSet(const std::string& name)
{
	// Check if provided name matches with an existent action set
	auto iter = actionSets.find(name);
	if (iter == actionSets.end())
	{
		JFF_LOG_WARNING("There isn't an action set with name " << name)
		return;
	}
	
	// Change active action set
	activeActionSet = actionSets[name];

	// Flush active actions on any change of Input
	activeActions.clear();
}

void JFF::InputGLFW::markActiveAction(const std::string& actionName)
{
	// Return if Input subsystem is not enabled
	if (!enabled)
		return;

	// Check if there's any action set active
	if (activeActionSet.expired())
	{
		JFF_LOG_WARNING("Couldn't mark the action as active. There's no active action set. Operation aborted")
		return;
	}

	// Check if provided name matches with an existent action in active action set
	auto activeActionSetHandler = activeActionSet.lock();
	auto iter = activeActionSetHandler->find(actionName);
	if (iter == activeActionSetHandler->end())
	{
		JFF_LOG_WARNING("Couldn't mark the action as active. There's no action with name " << actionName << " in current active set. Operation aborted")
		return;
	}

	// Put the active action on activeAction list
	activeActions[actionName] = iter->second;
}

void JFF::InputGLFW::setCursorMode(CursorMode mode)
{
	// Perfectly valid casting because this class is GLFW dependant too
	GLFWwindow* window = std::dynamic_pointer_cast<ContextGLFW>(engine->context.lock())->getWindow();

	switch (mode)
	{
	case JFF::Input::CursorMode::NORMAL:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case JFF::Input::CursorMode::HIDDEN:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case JFF::Input::CursorMode::DISABLED:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	default:
		break;
	}
}

void JFF::InputGLFW::addListener(
	const std::string& inputActionSetName, 
	const std::string& actionName, 
	const InputComponent* const inputComp, 
	const AxesListener& listener)
{
	auto inputAction = getInputAction(inputActionSetName, actionName);
	if (!inputAction)
		return;

	std::shared_ptr<InputActionAxes> inputActionAxes = std::dynamic_pointer_cast<InputActionAxes>(inputAction);
	if (!inputActionAxes)
	{
		JFF_LOG_WARNING("Cannot add input listener to action with name " << actionName << ". Action's type isn't 'axes'")
		return;
	}

	inputActionAxes->addListener(inputComp, listener);
}

void JFF::InputGLFW::addListener(
	const std::string& inputActionSetName, 
	const std::string& actionName, 
	const InputComponent* const inputComp, 
	const ButtonListener& listener)
{
	auto inputAction = getInputAction(inputActionSetName, actionName);
	if (!inputAction)
		return;

	std::shared_ptr<InputActionButton> inputActionButton = std::dynamic_pointer_cast<InputActionButton>(inputAction);
	if (!inputActionButton)
	{
		JFF_LOG_WARNING("Cannot add input listener to action with name " << actionName << ". Action's type isn't 'button'")
			return;
	}

	inputActionButton->addListener(inputComp, listener);
}

void JFF::InputGLFW::addListener(
	const std::string& inputActionSetName, 
	const std::string& actionName, 
	const InputComponent* const inputComp, 
	const TriggerListener& listener)
{
	auto inputAction = getInputAction(inputActionSetName, actionName);
	if (!inputAction)
		return;

	std::shared_ptr<InputActionTrigger> inputActionTrigger = std::dynamic_pointer_cast<InputActionTrigger>(inputAction);
	if (!inputActionTrigger)
	{
		JFF_LOG_WARNING("Cannot add input listener to action with name " << actionName << ". Action's type isn't 'trigger'")
			return;
	}

	inputActionTrigger->addListener(inputComp, listener);
}

void JFF::InputGLFW::removeAxesListener(
	const std::string& inputActionSetName, 
	const std::string& actionName, 
	const InputComponent* const inputComp)
{
	auto inputAction = getInputAction(inputActionSetName, actionName);
	if (!inputAction)
		return;

	std::shared_ptr<InputActionAxes> inputActionAxes = std::dynamic_pointer_cast<InputActionAxes>(inputAction);
	if (!inputActionAxes)
	{
		JFF_LOG_WARNING("Cannot remove input listener from action with name " << actionName << ". Action's type isn't 'axes'")
			return;
	}

	inputActionAxes->removeListener(inputComp);
}

void JFF::InputGLFW::removeButtonListener(
	const std::string& inputActionSetName, 
	const std::string& actionName, 
	const InputComponent* const inputComp)
{
	auto inputAction = getInputAction(inputActionSetName, actionName);
	if (!inputAction)
		return;

	std::shared_ptr<InputActionButton> inputActionButton = std::dynamic_pointer_cast<InputActionButton>(inputAction);
	if (!inputActionButton)
	{
		JFF_LOG_WARNING("Cannot remove input listener from action with name " << actionName << ". Action's type isn't 'button'")
			return;
	}

	inputActionButton->removeListener(inputComp);
}

void JFF::InputGLFW::removeTriggerListener(
	const std::string& inputActionSetName, 
	const std::string& actionName, 
	const InputComponent* const inputComp)
{
	auto inputAction = getInputAction(inputActionSetName, actionName);
	if (!inputAction)
		return;

	std::shared_ptr<InputActionTrigger> inputActionTrigger = std::dynamic_pointer_cast<InputActionTrigger>(inputAction);
	if (!inputActionTrigger)
	{
		JFF_LOG_WARNING("Cannot remove input listener from action with name " << actionName << ". Action's type isn't 'trigger'")
			return;
	}

	inputActionTrigger->removeListener(inputComp);
}

void JFF::InputGLFW::subscribeToKeyInputs(const InputBindingBase* const binding, const KeyCallback& callback)
{
	auto iter = keyCallbacks.find(binding);
	if (iter != keyCallbacks.end())
	{
		JFF_LOG_WARNING("Provided binding is already subscribed to key events. The new binding will replace the old one")
	}
	
	keyCallbacks[binding] = callback;
}

void JFF::InputGLFW::unsubscribeToKeyInputs(const InputBindingBase* const binding)
{
	size_t numErased = keyCallbacks.erase(binding);
	if (numErased <= 0)
	{
		JFF_LOG_WARNING("Couldn't delete input binding from key events. The provided binding wasn't part of the callback list")
	}
}

void JFF::InputGLFW::subscribeToMousePos(const InputBindingBase* const binding, const MousePosCallback& callback)
{
	auto iter = mousePosCallbacks.find(binding);
	if (iter != mousePosCallbacks.end())
	{
		JFF_LOG_WARNING("Provided binding is already subscribed to cursor pos events. The new binding will replace the old one")
	}

	mousePosCallbacks[binding] = callback;
}

void JFF::InputGLFW::unsubscribeToMousePos(const InputBindingBase* const binding)
{
	size_t numErased = mousePosCallbacks.erase(binding);
	if (numErased <= 0)
	{
		JFF_LOG_WARNING("Couldn't delete input binding from cursor pos events. The provided binding wasn't part of the callback list")
	}
}

void JFF::InputGLFW::subscribeToMouseInputs(const InputBindingBase* const binding, const MouseInputCallback& callback)
{
	auto iter = mouseInputCallbacks.find(binding);
	if (iter != mouseInputCallbacks.end())
	{
		JFF_LOG_WARNING("Provided binding is already subscribed to mouse input events. The new binding will replace the old one")
	}

	mouseInputCallbacks[binding] = callback;
}

void JFF::InputGLFW::unsubscribeToMouseInputs(const InputBindingBase* const binding)
{
	size_t numErased = mouseInputCallbacks.erase(binding);
	if (numErased <= 0)
	{
		JFF_LOG_WARNING("Couldn't delete input binding from mouse input events. The provided binding wasn't part of the callback list")
	}
}

void JFF::InputGLFW::subscribeToMouseScroll(const InputBindingBase* const binding, const MouseScrollCallback& callback)
{
	auto iter = mouseScrollCallbacks.find(binding);
	if (iter != mouseScrollCallbacks.end())
	{
		JFF_LOG_WARNING("Provided binding is already subscribed to mouse scroll events. The new binding will replace the old one")
	}

	mouseScrollCallbacks[binding] = callback;
}

void JFF::InputGLFW::unsubscribeToMouseScroll(const InputBindingBase* const binding)
{
	size_t numErased = mouseScrollCallbacks.erase(binding);
	if (numErased <= 0)
	{
		JFF_LOG_WARNING("Couldn't delete input binding from mouse scroll events. The provided binding wasn't part of the callback list")
	}
}

void JFF::InputGLFW::subscribeToGamepadButtonInputs(const InputBindingBase* const binding, const GamepadButtonCallback& callback)
{
	auto iter = gamepadButtonCallbacks.find(binding);
	if (iter != gamepadButtonCallbacks.end())
	{
		JFF_LOG_WARNING("Provided binding is already subscribed to gamepad button events. The new binding will replace the old one")
	}

	gamepadButtonCallbacks[binding] = callback;
}

void JFF::InputGLFW::unsubscribeToGamepadButtonInputs(const InputBindingBase* const binding)
{
	size_t numErased = gamepadButtonCallbacks.erase(binding);
	if (numErased <= 0)
	{
		JFF_LOG_WARNING("Couldn't delete input binding from gamepad button events. The provided binding wasn't part of the callback list")
	}
}

void JFF::InputGLFW::subscribeToGamepadAxesInputs(const InputBindingBase* const binding, const GamepadAxesCallback& callback)
{
	auto iter = gamepadAxesCallbacks.find(binding);
	if (iter != gamepadAxesCallbacks.end())
	{
		JFF_LOG_WARNING("Provided binding is already subscribed to gamepad axes events. The new binding will replace the old one")
	}

	gamepadAxesCallbacks[binding] = callback;
}

void JFF::InputGLFW::unsubscribeToGamepadAxesInputs(const InputBindingBase* const binding)
{
	size_t numErased = gamepadAxesCallbacks.erase(binding);
	if (numErased <= 0)
	{
		JFF_LOG_WARNING("Couldn't delete input binding from gamepad axes events. The provided binding wasn't part of the callback list")
	}
}

inline JFF::InputGLFW::Params JFF::InputGLFW::loadConfigFile() const
{
	std::string filePath = std::string("Config") + JFF_SLASH_STRING + "Engine.ini";
	auto INIFile = createINIFile(filePath.c_str());
	Params params;

	params.enabled = INIFile->has("input", "enabled") ? INIFile->getString("input", "enabled") == "true" : false;

	if (INIFile->has("input", "cursor-mode"))
	{
		std::string option = INIFile->getString("input", "cursor-mode");
		if (option == "HIDDEN")
		{
			params.cursorMode = CursorMode::HIDDEN;
		}
		else if (option == "DISABLED")
		{
			params.cursorMode = CursorMode::DISABLED;
		}
		else // option == "NORMAL"
		{
			params.cursorMode = CursorMode::NORMAL;
		}
	}
	else
	{
		params.cursorMode = CursorMode::NORMAL;
	}

	return params;
}

inline void JFF::InputGLFW::detectJoysticks()
{
	// GLFW supports up to 16 joysticks. Next code will iterate over them to check if they are connected
	int maxJoysticks = 16;
	for (int i = 0; i < maxJoysticks; ++i)
	{
		// Checks if joystick is connected and has a gamepad button mapping
		int joystick_i = GLFW_JOYSTICK_1 + i;
		if (glfwJoystickIsGamepad(joystick_i) == GLFW_TRUE)
			connectedGamepads.push_back(joystick_i);
	}
}

inline std::shared_ptr<JFF::InputActionBase>  JFF::InputGLFW::getInputAction(
	const std::string& inputActionSetName, 
	const std::string& actionName) const
{
	// Check if provided name matches with an existent action set
	auto actionSetIter = actionSets.find(inputActionSetName);
	if (actionSetIter == actionSets.end())
	{
		JFF_LOG_WARNING("There isn't an action set with name " << inputActionSetName)
		return std::shared_ptr<InputActionBase>();
	}

	// Check if provided name matches with an existent action
	auto actionIter = actionSetIter->second->find(actionName);
	if (actionIter == actionSetIter->second->end())
	{
		JFF_LOG_WARNING("There isn't an action with name " << actionName)
		return std::shared_ptr<InputActionBase>();
	}

	// Return the input action
	return actionIter->second;
}

// ---------------------------------- Extern functions defined in Setup.h ---------------------------------- //

extern std::shared_ptr<JFF::InputBindingButton> createInputBindingButton(const std::string& name, JFF::Engine* const engine,
	JFF::InputActionButton* const parentAction, JFF::Mapping inputMapping);
extern std::shared_ptr<JFF::InputBindingTrigger> createInputBindingTrigger(const std::string& name, JFF::Engine* const engine,
	JFF::InputActionTrigger* const parentAction, JFF::Mapping inputMapping);
extern std::shared_ptr<JFF::InputBindingAxes> createInputBindingAxes(const std::string& name, JFF::Engine* const engine,
	JFF::InputActionAxes* const parentAction, JFF::Mapping inputMapping);

// ---------------------------------- Helper non member functions ---------------------------------- //

template<typename T>
inline void parseModifiers(JFF::Engine* const engine, std::string& modifierToken,
	std::shared_ptr<JFF::InputBehavior<T>>& behavior, std::vector<std::shared_ptr<JFF::InputProcessor<T>>>& processors)
{
	// Remove curly braces
	modifierToken.erase(std::remove_if(modifierToken.begin(), modifierToken.end(), [](const auto& ch) { return ch == '{' || ch == '}'; }), modifierToken.end());

	// tokenize modifiers. Modifier name and pairs of params name-value
	std::map<std::string, std::map<std::string, std::string>> modifiers;
	
	std::stringstream modifierSs(modifierToken);
	std::string iterModifierString;
	while (std::getline(modifierSs, iterModifierString, '>'))
	{
		std::stringstream subModifierSs(iterModifierString);
		std::string modifierName;
		if (std::getline(subModifierSs, modifierName, '<')) // Modifier name
		{
			modifiers[modifierName] = {};

			std::string modifierParams;
			if (std::getline(subModifierSs, modifierParams, '<')) // Modifier params
			{
				std::stringstream modifierParamsSs(modifierParams);
				std::string keyValueStr;
				while (std::getline(modifierParamsSs, keyValueStr, '|'))
				{
					std::stringstream keyValueSs(keyValueStr);
					std::string key, value;
					if (std::getline(keyValueSs, key, ':') && std::getline(keyValueSs, value, ':'))
					{
						modifiers[modifierName][key] = value;
					}
				}
			}
		}
	}
	
	// Create modifier objects based on data extracted previously
	for (const auto& pair : modifiers)
	{
		if (pair.first == "behavior-press")
		{
			typename JFF::InputBehaviorPress<T>::Type pressType = JFF::InputBehaviorPress<T>::Type::PRESS_AND_RELEASE;
			float pressPoint = 0.5f;

			try
			{
				std::string typeStr = pair.second.at("press-type");
				if (typeStr == "PRESS_AND_RELEASE")
					pressType = JFF::InputBehaviorPress<T>::Type::PRESS_AND_RELEASE;
				else if (typeStr == "PRESS")
					pressType = JFF::InputBehaviorPress<T>::Type::PRESS;
				else if (typeStr == "RELEASE")
					pressType = JFF::InputBehaviorPress<T>::Type::RELEASE;
			}
			catch (...){}

			try
			{
				pressPoint = std::stof(pair.second.at("press-point"));
			}
			catch (...) {}

			behavior = std::make_shared<JFF::InputBehaviorPress<T>>(engine, pressType, pressPoint);
		}
		else if (pair.first == "behavior-hold")
		{
			double minHoldTime = 0.4;
			float pressPoint = 0.5f;

			try
			{
				minHoldTime = std::stod(pair.second.at("time"));
			}
			catch(...){}

			try
			{
				pressPoint = std::stof(pair.second.at("press-point"));
			}
			catch (...) {}

			behavior = std::make_shared<JFF::InputBehaviorHold<T>>(engine, minHoldTime, pressPoint);
		}
		else if (pair.first == "behavior-multipress")
		{
			unsigned int pressCount = 2; 
			double maxTime = 0.5f; 
			float pressPoint = 0.5f;

			try
			{
				pressCount = std::stoul(pair.second.at("count"));
			}
			catch(...){}

			try
			{
				maxTime = std::stod(pair.second.at("time"));
			}
			catch(...){}

			try
			{
				pressPoint = std::stof(pair.second.at("press-point"));
			}
			catch (...) {}

			behavior = std::make_shared<JFF::InputBehaviorMultipress<T>>(engine, pressCount, maxTime, pressPoint);
		}
		else if (pair.first == "processor-normalizer")
		{
			processors.push_back(std::make_shared<JFF::InputProcessorNormalizer<T>>(engine));
		}
		else if (pair.first == "processor-inverter")
		{
			bool invertX = false;
			bool invertY = false;
			
			try
			{
				invertX = pair.second.at("invert-x") == "true";
			}
			catch(...){}

			try
			{
				invertY = pair.second.at("invert-y") == "true";
			}
			catch (...) {}

			processors.push_back(std::make_shared<JFF::InputProcessorInverter<T>>(engine, invertX, invertY));
		}
		else if (pair.first == "processor-dead-zone")
		{
			float min = 0.2f;
			float max = 0.9f;

			try
			{
				min = std::stof(pair.second.at("min"));
			}
			catch(...){}

			try
			{
				max = std::stof(pair.second.at("max"));
			}
			catch (...) {}

			processors.push_back(std::make_shared<JFF::InputProcessorDeadZone<T>>(engine, min, max));
		}
	}
}

template<typename T>
inline void applyModifiers(std::shared_ptr<JFF::InputBinding<T>>& inputBinding,
	const std::shared_ptr<JFF::InputBehavior<T>>& globalBehavior, const std::vector<std::shared_ptr<JFF::InputProcessor<T>>>& globalProcessors,
	const std::shared_ptr<JFF::InputBehavior<T>>& localBehavior, const std::vector<std::shared_ptr<JFF::InputProcessor<T>>>& localProcessors)
{
	if (localBehavior)
		inputBinding->setBehavior(localBehavior);
	else
		inputBinding->setBehavior(globalBehavior);

	std::for_each(globalProcessors.begin(), globalProcessors.end(), [&inputBinding](const auto& processor) 
		{
			inputBinding->addProcessor(processor); 
		});
	std::for_each(localProcessors.begin(), localProcessors.end(), [&inputBinding](const auto& processor) 
		{ 
			inputBinding->addProcessor(processor); 
		});
}

template<typename T>
inline void applyModifiers(std::shared_ptr<JFF::InputBinding<T>>& inputBinding,
	const std::shared_ptr<JFF::InputBehavior<T>>& behavior, const std::vector<std::shared_ptr<JFF::InputProcessor<T>>>& processors)
{
	inputBinding->setBehavior(behavior);
	std::for_each(processors.begin(), processors.end(), [&inputBinding](const auto& processor) 
		{ 
			inputBinding->addProcessor(processor); 
		});
}

inline void parseInputActionAxes(std::vector<std::shared_ptr<JFF::InputBindingBase>>& inputBindings, JFF::Engine* const engine,
	const std::shared_ptr<JFF::InputGLFW::ActionSet>& actionSet,const std::vector<std::string>& actionTokens, 
	std::vector<std::string>& bindingTokens)
{
	using namespace JFF;

	// Create the action
	std::shared_ptr<InputActionAxes> inputAction = std::make_shared<JFF::InputActionAxes>(actionTokens[0], engine);
	actionSet->operator[](actionTokens[0]) = inputAction;

	// Extract global behavior and global processors token
	std::string modifierToken;
	if (bindingTokens.size() > 0 && bindingTokens[0][0] == '{')
	{
		modifierToken = bindingTokens[0];
		bindingTokens.erase(bindingTokens.begin());
	}

	// Create all bindings associated to the action
	for (const std::string& bindingName : bindingTokens)
	{
		if (bindingName[0] == '{')
		{
			JFF_LOG_WARNING("Global input modifiers are allowed before all bindings only. Binding token will be ignored")
				continue;
		}

		auto iter = std::find(bindingName.begin(), bindingName.end(), '{');
		if (iter != bindingName.end())
		{
			std::string bindingNameSubstr(bindingName.begin(), iter);
			std::string bindingModifiers(iter, bindingName.end());

			auto inputBinding = createInputBindingAxes(bindingNameSubstr, engine, inputAction.get(), InputBindingAxes::stringToMapping(bindingNameSubstr));
			inputBindings.push_back(inputBinding);

			// Create global behavior and global processors
			std::string globalModifierToken = modifierToken;
			std::shared_ptr<InputBehaviorAxes> globalBehavior;
			std::vector<std::shared_ptr<InputProcessorAxes>> globalProcessors;
			parseModifiers(engine, globalModifierToken, globalBehavior, globalProcessors);

			// Create local behavior and global processors
			std::shared_ptr<InputBehaviorAxes> localBehavior;
			std::vector<std::shared_ptr<InputProcessorAxes>> localProcessors;
			parseModifiers(engine, bindingModifiers, localBehavior, localProcessors);

			// Apply global or local modifiers to created binding. Local modifiers have higher priority
			applyModifiers(inputBinding, globalBehavior, globalProcessors, localBehavior, localProcessors);
		}
		else
		{
			auto inputBinding = createInputBindingAxes(bindingName, engine, inputAction.get(), InputBindingAxes::stringToMapping(bindingName));
			inputBindings.push_back(inputBinding);

			// Create global behavior and global processors
			std::string globalModifierToken = modifierToken;
			std::shared_ptr<InputBehaviorAxes> globalBehavior;
			std::vector<std::shared_ptr<InputProcessorAxes>> globalProcessors;
			parseModifiers(engine, globalModifierToken, globalBehavior, globalProcessors);

			// Apply global modifiers to created binding
			applyModifiers(inputBinding, globalBehavior, globalProcessors);
		}
	}
}

inline void parseInputActionTrigger(std::vector<std::shared_ptr<JFF::InputBindingBase>>& inputBindings, JFF::Engine* const engine,
	const std::shared_ptr<JFF::InputGLFW::ActionSet>& actionSet, const std::vector<std::string>& actionTokens, 
	std::vector<std::string>& bindingTokens)
{
	using namespace JFF;

	std::shared_ptr<InputActionTrigger> inputAction = std::make_shared<InputActionTrigger>(actionTokens[0], engine);
	actionSet->operator[](actionTokens[0]) = inputAction;

	// Extract global behavior and global processors token
	std::string modifierToken;
	if (bindingTokens.size() > 0 && bindingTokens[0][0] == '{')
	{
		modifierToken = bindingTokens[0];
		bindingTokens.erase(bindingTokens.begin());
	}

	// Create all bindings associated to the action
	for (const std::string& bindingName : bindingTokens)
	{
		if (bindingName[0] == '{')
		{
			JFF_LOG_WARNING("Global input modifiers are allowed before all bindings only. Binding token will be ignored")
				continue;
		}

		auto iter = std::find(bindingName.begin(), bindingName.end(), '{');
		if (iter != bindingName.end())
		{
			std::string bindingNameSubstr(bindingName.begin(), iter);
			std::string bindingModifiers(iter, bindingName.end());

			auto inputBinding = createInputBindingTrigger(bindingNameSubstr, engine, inputAction.get(), InputBindingTrigger::stringToMapping(bindingNameSubstr));
			inputBindings.push_back(inputBinding);

			// Create global behavior and global processors
			std::string globalModifierToken = modifierToken;
			std::shared_ptr<InputBehaviorTrigger> globalBehavior;
			std::vector<std::shared_ptr<InputProcessorTrigger>> globalProcessors;
			parseModifiers(engine, globalModifierToken, globalBehavior, globalProcessors);

			// Create local behavior and global processors
			std::shared_ptr<InputBehaviorTrigger> localBehavior;
			std::vector<std::shared_ptr<InputProcessorTrigger>> localProcessors;
			parseModifiers(engine, bindingModifiers, localBehavior, localProcessors);

			// Apply global or local modifiers to created binding. Local modifiers have higher priority
			applyModifiers(inputBinding, globalBehavior, globalProcessors, localBehavior, localProcessors);
		}
		else
		{
			auto inputBinding = createInputBindingTrigger(bindingName, engine, inputAction.get(), InputBindingTrigger::stringToMapping(bindingName));
			inputBindings.push_back(inputBinding);

			// Create global behavior and global processors
			std::string globalModifierToken = modifierToken;
			std::shared_ptr<InputBehaviorTrigger> globalBehavior;
			std::vector<std::shared_ptr<InputProcessorTrigger>> globalProcessors;
			parseModifiers(engine, globalModifierToken, globalBehavior, globalProcessors);

			// Apply global modifiers to created binding
			applyModifiers(inputBinding, globalBehavior, globalProcessors);
		}

	}
}

inline void parseInputActionButton(std::vector<std::shared_ptr<JFF::InputBindingBase>>& inputBindings, JFF::Engine* const engine,
	const std::shared_ptr<JFF::InputGLFW::ActionSet>& actionSet, const std::vector<std::string>& actionTokens, 
	std::vector<std::string>& bindingTokens)
{
	using namespace JFF;

	std::shared_ptr<InputActionButton> inputAction = std::make_shared<InputActionButton>(actionTokens[0], engine);
	actionSet->operator[](actionTokens[0]) = inputAction;

	// Extract global behavior and global processors token
	std::string modifierToken;
	if (bindingTokens.size() > 0 && bindingTokens[0][0] == '{')
	{
		modifierToken = bindingTokens[0];
		bindingTokens.erase(bindingTokens.begin());
	}

	// Create all bindings associated to the action
	for (const std::string& bindingName : bindingTokens)
	{
		if (bindingName[0] == '{')
		{
			JFF_LOG_WARNING("Global input modifiers are allowed before all bindings only. Binding token will be ignored")
				continue;
		}

		auto iter = std::find(bindingName.begin(), bindingName.end(), '{');
		if (iter != bindingName.end())
		{
			std::string bindingNameSubstr(bindingName.begin(), iter);
			std::string bindingModifiers(iter, bindingName.end());

			auto inputBinding = createInputBindingButton(bindingNameSubstr, engine, inputAction.get(), InputBindingButton::stringToMapping(bindingNameSubstr));
			inputBindings.push_back(inputBinding);

			// Create global behavior and global processors
			std::string globalModifierToken = modifierToken;
			std::shared_ptr<InputBehaviorButton> globalBehavior;
			std::vector<std::shared_ptr<InputProcessorButton>> globalProcessors;
			parseModifiers(engine, globalModifierToken, globalBehavior, globalProcessors);

			// Create local behavior and global processors
			std::shared_ptr<InputBehaviorButton> localBehavior;
			std::vector<std::shared_ptr<InputProcessorButton>> localProcessors;
			parseModifiers(engine, bindingModifiers, localBehavior, localProcessors);

			// Apply global or local modifiers to created binding. Local modifiers have higher priority
			applyModifiers(inputBinding, globalBehavior, globalProcessors, localBehavior, localProcessors);
		}
		else
		{
			auto inputBinding = createInputBindingButton(bindingName, engine, inputAction.get(), InputBindingButton::stringToMapping(bindingName));
			inputBindings.push_back(inputBinding);

			// Create global behavior and global processors
			std::string globalModifierToken = modifierToken;
			std::shared_ptr<InputBehaviorButton> globalBehavior;
			std::vector<std::shared_ptr<InputProcessorButton>> globalProcessors;
			parseModifiers(engine, globalModifierToken, globalBehavior, globalProcessors);

			// Apply global modifiers to created binding
			applyModifiers(inputBinding, globalBehavior, globalProcessors);
		}
	}
}

inline void parseActionSet(std::vector<std::shared_ptr<JFF::InputBindingBase>>& inputBindings, JFF::Engine* const engine,
	const std::shared_ptr<JFF::INIFile>& iniFile, const std::string& actionSetName, 
	const std::shared_ptr<JFF::InputGLFW::ActionSet>& actionSet)
{
	iniFile->visitKeyValuePairs(actionSetName.c_str(), [&inputBindings, &engine, &actionSet](const auto& pair)
		{
			// ----------------------------------- Parse action ----------------------------------- //
			
			// Tokenize action into two parts: action name and action type
			std::vector<std::string> actionTokens; // [0]:action name [1]: action type
			std::stringstream actionSs(pair.first);
			std::string iterActionString;
			while (std::getline(actionSs, iterActionString, '-'))
			{
				actionTokens.push_back(iterActionString);
			}

			// If tokenized action doesn't have two parts, ignore the line
			if (actionTokens.size() != 2)
			{
				JFF_LOG_WARNING("Incomplete action name:" << pair.first << ". Action names must have the form name-[axes|trigger|button]")
				return;
			}

			// ----------------------------------- Parse bindings ----------------------------------- //

			// Remove white spaces
			std::string bindStr = pair.second;
			bindStr.erase(std::remove_if(bindStr.begin(), bindStr.end(), [](const auto& ch) { return ch == ' '; }), bindStr.end());

			// Tokenize binding splitted by commas
			std::vector<std::string> bindingTokens;
			std::stringstream bindSs(bindStr);
			std::string iterBindString;
			while (std::getline(bindSs, iterBindString, ','))
			{
				bindingTokens.push_back(iterBindString);
			}

			// ----------------------------------- Action and binding creation ----------------------------------- //
			
			if (actionTokens[1] == "axes")
				parseInputActionAxes(inputBindings, engine, actionSet, actionTokens, bindingTokens);
			else if (actionTokens[1] == "trigger")
				parseInputActionTrigger(inputBindings, engine, actionSet, actionTokens, bindingTokens);
			else if (actionTokens[1] == "button")
				parseInputActionButton(inputBindings, engine, actionSet, actionTokens, bindingTokens);
		});
}

inline void JFF::InputGLFW::loadInputsFromFile()
{
	// Read ini file. The file name is fixed
	std::string filePath = std::string("Config") + JFF_SLASH_STRING + "Input.ini";
	std::shared_ptr<INIFile> iniFile = engine->io.lock()->loadINIFile(filePath.c_str());

	// Loop on all input action sets
	std::string firstActionSet;
	
	std::vector<std::string> actionSetNames;
	iniFile->getAllSections(actionSetNames);
	if (actionSetNames.empty())
	{
		JFF_LOG_WARNING("There's no input configured on Input.h")
		return;
	}

	std::vector<std::string> allActionNames;

	std::for_each(actionSetNames.begin(), actionSetNames.end(), [this, &iniFile, &firstActionSet, &allActionNames](const std::string& actionSetName)
		{
			// Crete a new action set.
			std::shared_ptr<ActionSet> actionSet = std::make_shared<ActionSet>();

			// Store the name of the first action set found
			if (actionSets.empty())
				firstActionSet = actionSetName;

			// Warn if there are actions sets that share the same name
			auto predicate = [&actionSetName](const auto& pair) { return pair.first == actionSetName; };
			if (std::find_if(actionSets.begin(), actionSets.end(), predicate) != actionSets.end())
			{
				JFF_LOG_WARNING("An action set with name " << actionSetName << " already exist in actionSets. The old one will be replaced")
			}
			
			// Add new action set to list
			actionSets[actionSetName] = actionSet;

			// Read all actions in current action set
			parseActionSet(inputBindings, engine, iniFile, actionSetName, actionSet);

			// Store all action names from all action sets. This is used to detect if there are repeated action names on ini file
			std::for_each(actionSet->begin(), actionSet->end(), [&allActionNames](const auto& pair) 
				{
					if (std::find(allActionNames.begin(), allActionNames.end(), pair.first) != allActionNames.end())
					{
						JFF_LOG_ERROR("Repeated action name '" << pair.first << "' detected. Swapping action sets won't work correctly unless you choose unique action names. ")
					}
					else
					{
						allActionNames.push_back(pair.first); 
					}
				});
		});

	// Mark as active the first action set by default
	activeActionSet = actionSets[firstActionSet];
}


// ------------------------------------------ GLFW INPUT CALLBACK ADAPTOR ------------------------------------------ //

JFF::GLFWInputCallbackAdaptor::GLFWInputCallbackAdaptor(InputGLFW* const input) :
	input(input)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor GLFWInputCallbackAdaptor")
}

JFF::GLFWInputCallbackAdaptor::~GLFWInputCallbackAdaptor()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor GLFWInputCallbackAdaptor")
}

JFF::GLFWInputCallbackAdaptor& JFF::GLFWInputCallbackAdaptor::getInstance(InputGLFW* const input)
{
	static GLFWInputCallbackAdaptor instance(input); // Internal static variable. This line will be executed the first time only
	return instance;
}

void JFF::GLFWInputCallbackAdaptor::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto& keyCallbacks = getInstance().input->keyCallbacks;
	std::for_each(keyCallbacks.begin(), keyCallbacks.end(), [key, action, mods](const auto& pair)
		{
			pair.second(key, action, mods);
		});
}

void JFF::GLFWInputCallbackAdaptor::mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	auto& mousePosCallbacks = getInstance().input->mousePosCallbacks;
	std::for_each(mousePosCallbacks.begin(), mousePosCallbacks.end(), [xpos, ypos](const auto& pair)
		{
			pair.second(xpos, ypos);
		});
}

void JFF::GLFWInputCallbackAdaptor::mouseInputCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto& mouseInputCallbacks = getInstance().input->mouseInputCallbacks;
	std::for_each(mouseInputCallbacks.begin(), mouseInputCallbacks.end(), [button, action, mods](const auto& pair)
		{
			pair.second(button, action, mods);
		});
}

void JFF::GLFWInputCallbackAdaptor::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto& mouseScrollCallbacks = getInstance().input->mouseScrollCallbacks;
	std::for_each(mouseScrollCallbacks.begin(), mouseScrollCallbacks.end(), [xoffset, yoffset](const auto& pair)
		{
			pair.second(xoffset, yoffset);
		});
}

void JFF::GLFWInputCallbackAdaptor::joystickConnectionCallback(int jid, int joystickEvent)
{
	auto& connectedGamepads = getInstance().input->connectedGamepads;

	// Find changed joystick in connected list
	auto iter = std::find(connectedGamepads.begin(), connectedGamepads.end(), jid);

	// Check if this is a connection or disconnection event
	if (joystickEvent == GLFW_CONNECTED)
	{
		// Ensure connectedGamepads doesn't contain connected joystick already
		if (iter != connectedGamepads.end())
		{
			JFF_LOG_ERROR("Coonected gamepads contain recently connected gamepad already")
		}
		else
		{
			JFF_LOG_WARNING("New gamepad detected. Gamepad ID: " << jid)
			connectedGamepads.push_back(jid);
		}
	}
	else if (joystickEvent == GLFW_DISCONNECTED)
	{
		// Ensure connectedGamepads contains connected joystick
		if (iter == connectedGamepads.end())
		{
			JFF_LOG_ERROR("Coonected gamepads doesn't contain recently disconnected gamepad")
		}
		else
		{
			JFF_LOG_WARNING("Gamepad with ID " << jid << " was disconnected")
			connectedGamepads.erase(iter);
		}
	}
}
