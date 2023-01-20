/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputBindingButtonGLFW.h"
#include "ContextGLFW.h"

#include <sstream>

JFF::InputBindingButtonGLFW::InputBindingButtonGLFW(
	const std::string& name, 
	Engine* const engine,
	InputActionButton* const parentAction, 
	Mapping inputMapping) :
	InputBindingButton(name, engine, parentAction, inputMapping),
	name(name),
	engine(engine),
	parentAction(parentAction),
	inputMapping(inputMapping),
	firstMouseCapture(true),
	lastMousePos(),
	mouseDeltaAccum(),

	behavior(),
	processors(),

	lastKey(GLFW_KEY_UNKNOWN),

	lastAction(GLFW_RELEASE),

	lastActionUp(GLFW_RELEASE),
	lastActionDown(GLFW_RELEASE),
	lastActionLeft(GLFW_RELEASE),
	lastActionRight(GLFW_RELEASE),

	lastActionStart(GLFW_RELEASE),
	lastActionBack(GLFW_RELEASE),

	lastActionCross(GLFW_RELEASE),
	lastActionCircle(GLFW_RELEASE),
	lastActionTriangle(GLFW_RELEASE),
	lastActionSquare(GLFW_RELEASE),

	lastActionBumperLeft(GLFW_RELEASE),
	lastActionBumperRight(GLFW_RELEASE),

	lastActionThumbLeft(GLFW_RELEASE),
	lastActionThumbRight(GLFW_RELEASE)
{
	JFF_LOG_INFO("Ctor InputBindingButtonGLFW")

	// Perfectly valid casting because this class is GLFW dependant too
	window = std::dynamic_pointer_cast<ContextGLFW>(engine->context.lock())->getWindow();

	auto inputShared = engine->input.lock();
	auto inputGeneral = inputShared.get();
	input = dynamic_cast<InputGLFW*>(inputGeneral);

	// Subscribe to appropriate events based on inputMapping
	initSubscriptions();
}

JFF::InputBindingButtonGLFW::~InputBindingButtonGLFW()
{
	JFF_LOG_INFO("Dtor InputBindingButtonGLFW")

	cancelSubscriptions(); // Cancel all callbacks before destruction
}

void JFF::InputBindingButtonGLFW::resetAccumulators()
{
	mouseDeltaAccum.x = 0.0f;
	mouseDeltaAccum.y = 0.0f;
}

void JFF::InputBindingButtonGLFW::setBehavior(const std::shared_ptr<InputBehaviorButton>& behavior)
{
	this->behavior = behavior;
}

void JFF::InputBindingButtonGLFW::addProcessor(const std::shared_ptr<InputProcessorButton>& processor)
{
	processors.push_back(processor);
}

void JFF::InputBindingButtonGLFW::initSubscriptions()
{
	// Tokenize binding name to check what type is it
	std::vector<std::string> bindingNameTokens;
	std::stringstream bindingSs(name);
	std::string iterBindingString;
	while (std::getline(bindingSs, iterBindingString, '_'))
	{
		bindingNameTokens.push_back(iterBindingString);
	}

	// Check if mapping is a keyboard
	if (bindingNameTokens[0] == "KEYBOARD")
	{
		mappingType = MappingType::KEYBOARD;
		input->subscribeToKeyInputs(this, createKeyCallback());
	}

	// Check if mapping is a mouse
	else if (bindingNameTokens[0] == "MOUSE")
	{
		if ((bindingNameTokens.size() >= 3 && bindingNameTokens[2] == "CLICK") || bindingNameTokens[1] == "ANY" || bindingNameTokens[1] == "BUTTON")
		{
			mappingType = MappingType::MOUSE_INPUT;
			input->subscribeToMouseInputs(this, createMouseInputCallback());
		}
		else if (bindingNameTokens[1] == "POSITION" || bindingNameTokens[1] == "DELTA")
		{
			mappingType = MappingType::MOUSE_POS;
			input->subscribeToMousePos(this, createMousePosCallback());
		}
		else if (bindingNameTokens[1] == "SCROLL")
		{
			mappingType = MappingType::MOUSE_SCROLL;
			input->subscribeToMouseScroll(this, createMouseScrollCallback());
		}
	}

	// Check if mapping is a gamepad
	else if (bindingNameTokens[0] == "GAMEPAD")
	{
		if (bindingNameTokens.size() >= 3 && (bindingNameTokens[2] == "STICK" || bindingNameTokens[2] == "TRIGGER"))
		{
			if (bindingNameTokens.size() >= 4 && bindingNameTokens[3] == "PRESS")
			{
				mappingType = MappingType::GAMEPAD_BUTTON;
				input->subscribeToGamepadButtonInputs(this, createGamepadButtonCallback());
			}
			else
			{
				mappingType = MappingType::GAMEPAD_AXES;
				input->subscribeToGamepadAxesInputs(this, createGamepadAxesCallback());
			}
		}
		else
		{
			mappingType = MappingType::GAMEPAD_BUTTON;
			input->subscribeToGamepadButtonInputs(this, createGamepadButtonCallback());
		}
	}

	// Unknown mapping
	else
	{
		mappingType = MappingType::UNKNOWN;
		JFF_LOG_WARNING("Unknown mapping")
	}
}

void JFF::InputBindingButtonGLFW::cancelSubscriptions()
{
	switch (mappingType)
	{
		break;
	case JFF::InputBindingButtonGLFW::MappingType::KEYBOARD:
		input->unsubscribeToKeyInputs(this);
		break;
	case JFF::InputBindingButtonGLFW::MappingType::MOUSE_POS:
		input->unsubscribeToMousePos(this);
		break;
	case JFF::InputBindingButtonGLFW::MappingType::MOUSE_SCROLL:
		input->unsubscribeToMouseScroll(this);
		break;
	case JFF::InputBindingButtonGLFW::MappingType::MOUSE_INPUT:
		input->unsubscribeToMouseInputs(this);
		break;
	case JFF::InputBindingButtonGLFW::MappingType::GAMEPAD_AXES:
		input->unsubscribeToGamepadAxesInputs(this);
		break;
	case JFF::InputBindingButtonGLFW::MappingType::GAMEPAD_BUTTON:
		input->unsubscribeToGamepadButtonInputs(this);
		break;
	case JFF::InputBindingButtonGLFW::MappingType::UNKNOWN:
	default:
		JFF_LOG_WARNING("Unknown mapping")
			break;
	}
}


inline JFF::InputGLFW::KeyCallback JFF::InputBindingButtonGLFW::createKeyCallback()
{
	return [this](int key, int action, int mods)
	{
		// Special keys checking
		if (inputMapping == Mapping::KEYBOARD_ANY)
		{
			bool output = action == GLFW_PRESS;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingAnyBehavior(key, action, output, Input::Hardware::KEYBOARD);
			else // Default behavior
				setActionValueIfAnyChanged(key, action, output, Input::Hardware::KEYBOARD);
		}
		else if (inputMapping == Mapping::KEYBOARD_WASD)
		{
			if (!(key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_D || key == GLFW_KEY_A))
				return;

			bool output =
				glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
			output = applyProcessors(output); // Check processors

			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(action, output, Input::Hardware::KEYBOARD);
			else // Default behavior
				setActionValueIfChangedWASD(key, action, output, Input::Hardware::KEYBOARD);
		}
		else if (inputMapping == Mapping::KEYBOARD_ARROWS)
		{
			if (!(key == GLFW_KEY_UP || key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT))
				return;

			bool output =
				glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
				glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;
			output = applyProcessors(output); // Check processors

			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(action, output, Input::Hardware::KEYBOARD);
			else // Default behavior
				setActionValueIfChangedArrowKeys(key, action, output, Input::Hardware::KEYBOARD);
		}

		// General checking
		else if (key == static_cast<int>(inputMapping))
		{
			bool output = action == GLFW_PRESS;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(action, output, Input::Hardware::KEYBOARD);
			else // Default behavior
				setActionValueIfChanged(action, output, Input::Hardware::KEYBOARD);
		}

	};
}

inline JFF::InputGLFW::MousePosCallback JFF::InputBindingButtonGLFW::createMousePosCallback()
{
	return [this](double xpos, double ypos)
	{
		// Mouse position checking
		if (inputMapping == Mapping::MOUSE_POSITION)
		{
			Vec2 mousePos(static_cast<float>(xpos), static_cast<float>(ypos));
			bool output = engine->math.lock()->length(mousePos) > 0.0f;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValue(output, Input::Hardware::MOUSE);
		}

		// Mouse delta checking
		else if (inputMapping == Mapping::MOUSE_DELTA)
		{
			if (firstMouseCapture)
			{
				lastMousePos.x = static_cast<float>(xpos);
				lastMousePos.y = static_cast<float>(ypos);
				firstMouseCapture = false;
			}
			else
			{
				Vec2 newPos(static_cast<float>(xpos), static_cast<float>(ypos));
				mouseDeltaAccum += newPos - lastMousePos; // Delta mouse accumulated
				lastMousePos = newPos;

				bool output = engine->math.lock()->length(mouseDeltaAccum) > 0.0f;
				output = applyProcessors(output); // Check processors
				if (behavior) // Check behavior (press, release, hold, double hit, ...)
					setActionValueCheckingBehavior(output, Input::Hardware::MOUSE);
				else // Default behavior
					setActionValue(output, Input::Hardware::MOUSE);
			}
		}

	};
}

inline JFF::InputGLFW::MouseInputCallback JFF::InputBindingButtonGLFW::createMouseInputCallback()
{
	return [this](int button, int action, int mods)
	{
		// Special mouse checking
		if (inputMapping == Mapping::MOUSE_ANY)
		{
			bool output = action == GLFW_PRESS;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingAnyBehavior(button, action, output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValueIfAnyChanged(button, action, output, Input::Hardware::MOUSE);
		}

		// General checking
		else if (button == static_cast<int>(inputMapping))
		{
			bool output = action == GLFW_PRESS;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(action, output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValueIfChanged(action, output, Input::Hardware::MOUSE);
		}

	};
}

inline JFF::InputGLFW::MouseScrollCallback JFF::InputBindingButtonGLFW::createMouseScrollCallback()
{
	return [this](double xoffset, double yoffset)
	{
		// Any scroll checking
		if (inputMapping == Mapping::MOUSE_SCROLL)
		{
			Vec2 scrollDir(static_cast<float>(xoffset), static_cast<float>(yoffset));

			bool output = engine->math.lock()->length(scrollDir) > 0.0f;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValue(output, Input::Hardware::MOUSE);
		}

		// Concrete scroll checking
		else if (inputMapping == Mapping::MOUSE_SCROLL_UP)
		{
			bool output = yoffset > 0.0;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValue(output, Input::Hardware::MOUSE);

		}
		else if (inputMapping == Mapping::MOUSE_SCROLL_DOWN)
		{
			bool output = yoffset < 0.0;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValue(output, Input::Hardware::MOUSE);
		}
		else if (inputMapping == Mapping::MOUSE_SCROLL_LEFT)
		{
			bool output = xoffset > 0.0;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValue(output, Input::Hardware::MOUSE);
		}
		else if (inputMapping == Mapping::MOUSE_SCROLL_RIGHT)
		{
			bool output = xoffset < 0.0;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::MOUSE);
			else // Default behavior
				setActionValue(output, Input::Hardware::MOUSE);
		}

	};
}

inline JFF::InputGLFW::GamepadButtonCallback JFF::InputBindingButtonGLFW::createGamepadButtonCallback()
{
	return [this](const GLFWgamepadstate& gamepadState)
	{
		// Special gamepad checking
		if (inputMapping == Mapping::GAMEPAD_ANY)
		{
			auto start = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_START];
			auto back = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_BACK];

			auto cross = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_CROSS];
			auto circle = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE];
			auto square = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_SQUARE];
			auto triangle = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_TRIANGLE];

			auto dpadUp = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
			auto dpadDown = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
			auto dpadLeft = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
			auto dpadRight = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];

			auto leftBumper = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
			auto rightBumper = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];

			auto leftThumb = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
			auto rightThumb = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];

			bool output = start == GLFW_PRESS || back == GLFW_PRESS ||
				cross == GLFW_PRESS || circle == GLFW_PRESS || square == GLFW_PRESS || triangle == GLFW_PRESS ||
				dpadUp == GLFW_PRESS || dpadDown == GLFW_PRESS || dpadLeft == GLFW_PRESS || dpadRight == GLFW_PRESS ||
				leftBumper == GLFW_PRESS || rightBumper == GLFW_PRESS ||
				leftThumb == GLFW_PRESS || rightThumb == GLFW_PRESS;
			output = applyProcessors(output); // Check processors

			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::GAMEPAD);
			else // Default behavior
				setActionValueIfChangedGamepad(start, back, 
					cross, circle, square, triangle,
					dpadUp, dpadDown, dpadLeft, dpadRight, 
					leftBumper, rightBumper, leftThumb, rightThumb, 
					output, Input::Hardware::GAMEPAD);
		}
		else if (inputMapping == Mapping::GAMEPAD_D_PAD)
		{
			auto dpadRight = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
			auto dpadLeft = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
			auto dpadUp = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
			auto dpadDown = gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];

			bool output = dpadRight == GLFW_PRESS || dpadLeft == GLFW_PRESS || dpadUp == GLFW_PRESS || dpadDown == GLFW_PRESS;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::GAMEPAD);
			else // Default behavior
				setActionValueIfChangedDpad(dpadUp, dpadDown, dpadLeft, dpadRight, output, Input::Hardware::GAMEPAD);
		}

		// General checking
		else
		{
			// Button checking
			auto action = gamepadState.buttons[static_cast<int>(inputMapping)];
			bool output = action == GLFW_PRESS;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::GAMEPAD);
			else // Default behavior
				setActionValueIfChanged(action, output, Input::Hardware::GAMEPAD);
		}

	};
}

inline JFF::InputGLFW::GamepadAxesCallback JFF::InputBindingButtonGLFW::createGamepadAxesCallback()
{
	return [this](const GLFWgamepadstate& gamepadState)
	{
		// Special gamepad checking
		if (inputMapping == Mapping::GAMEPAD_LEFT_STICK)
		{
			Vec2 stickDir(gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);

			bool output = engine->math.lock()->length(stickDir) > 0.0f;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::GAMEPAD);
			else // Default behavior
				setActionValue(output, Input::Hardware::GAMEPAD);
		}
		else if (inputMapping == Mapping::GAMEPAD_RIGHT_STICK)
		{
			Vec2 stickDir(gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);

			bool output = engine->math.lock()->length(stickDir) > 0.0f;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::GAMEPAD);
			else // Default behavior
				setActionValue(output, Input::Hardware::GAMEPAD);
		}

		// General checking
		else
		{
			// Axis checking
			bool output = gamepadState.axes[static_cast<int>(inputMapping)] > 0.0f;
			output = applyProcessors(output); // Check processors
			if (behavior) // Check behavior (press, release, hold, double hit, ...)
				setActionValueCheckingBehavior(output, Input::Hardware::GAMEPAD);
			else // Default behavior
				setActionValue(output, Input::Hardware::GAMEPAD);
		}

	};
}


void JFF::InputBindingButtonGLFW::setActionValueIfChangedWASD(int key, int action, bool inputValue, Input::Hardware hw)
{
	if (action == GLFW_REPEAT) // Ignore GLFW repeat events
		return;

	bool inputChanged = false;
	if (key == GLFW_KEY_W && action != lastActionUp)
	{
		lastActionUp = action;
		inputChanged = inputChanged || true;
	}
	if (key == GLFW_KEY_S && action != lastActionDown)
	{
		lastActionDown = action;
		inputChanged = inputChanged || true;
	}
	if (key == GLFW_KEY_A && action != lastActionLeft)
	{
		lastActionLeft = action;
		inputChanged = inputChanged || true;
	}
	if (key == GLFW_KEY_D && action != lastActionRight)
	{
		lastActionRight = action;
		inputChanged = inputChanged || true;
	}
	if (inputChanged)
	{
		setActionValue(inputValue, hw);
	}
}

void JFF::InputBindingButtonGLFW::setActionValueIfChangedArrowKeys(int key, int action, bool inputValue, Input::Hardware hw)
{
	if (action == GLFW_REPEAT) // Ignore GLFW repeat events
		return;

	bool inputChanged = false;
	if (key == GLFW_KEY_UP && action != lastActionUp)
	{
		lastActionUp = action;
		inputChanged = inputChanged || true;
	}
	if (key == GLFW_KEY_DOWN && action != lastActionDown)
	{
		lastActionDown = action;
		inputChanged = inputChanged || true;
	}
	if (key == GLFW_KEY_LEFT && action != lastActionLeft)
	{
		lastActionLeft = action;
		inputChanged = inputChanged || true;
	}
	if (key == GLFW_KEY_RIGHT && action != lastActionRight)
	{
		lastActionRight = action;
		inputChanged = inputChanged || true;
	}
	if (inputChanged)
	{
		setActionValue(inputValue, hw);
	}
}

void JFF::InputBindingButtonGLFW::setActionValueIfChangedGamepad(unsigned char start, unsigned char back, 
	unsigned char cross, unsigned char circle, unsigned char square, unsigned char triangle, 
	unsigned char dpadUp, unsigned char dpadDown, unsigned char dpadLeft, unsigned char dpadRight, 
	unsigned char leftBumper, unsigned char rightBumper, unsigned char leftThumb, unsigned char rightThumb, 
	bool inputValue, Input::Hardware hw)
{
	bool inputChanged = false;
	if (start != lastActionStart)
	{
		lastActionStart = start;
		inputChanged = inputChanged || true;
	}
	if (back != lastActionBack)
	{
		lastActionBack = back;
		inputChanged = inputChanged || true;
	}
	if (cross != lastActionCross)
	{
		lastActionCross = cross;
		inputChanged = inputChanged || true;
	}
	if (circle != lastActionCircle)
	{
		lastActionCircle = circle;
		inputChanged = inputChanged || true;
	}
	if (square != lastActionSquare)
	{
		lastActionSquare = square;
		inputChanged = inputChanged || true;
	}
	if (triangle != lastActionTriangle)
	{
		lastActionTriangle = triangle;
		inputChanged = inputChanged || true;
	}
	if (dpadUp != lastActionUp)
	{
		lastActionUp = dpadUp;
		inputChanged = inputChanged || true;
	}
	if (dpadDown != lastActionDown)
	{
		lastActionDown = dpadDown;
		inputChanged = inputChanged || true;
	}
	if (dpadLeft != lastActionLeft)
	{
		lastActionLeft = dpadLeft;
		inputChanged = inputChanged || true;
	}
	if (dpadRight != lastActionRight)
	{
		lastActionRight = dpadRight;
		inputChanged = inputChanged || true;
	}
	if (leftBumper != lastActionBumperLeft)
	{
		lastActionBumperLeft = leftBumper;
		inputChanged = inputChanged || true;
	}
	if (rightBumper != lastActionBumperRight)
	{
		lastActionBumperRight = rightBumper;
		inputChanged = inputChanged || true;
	}
	if (leftThumb != lastActionThumbLeft)
	{
		lastActionThumbLeft = leftThumb;
		inputChanged = inputChanged || true;
	}
	if (rightThumb != lastActionThumbRight)
	{
		lastActionThumbRight = rightThumb;
		inputChanged = inputChanged || true;
	}
	if (inputChanged)
	{
		setActionValue(inputValue, hw);
	}
}

void JFF::InputBindingButtonGLFW::setActionValueIfChangedDpad(
	unsigned char dpadUp, unsigned char dpadDown, unsigned char dpadLeft, unsigned char dpadRight, bool inputValue, Input::Hardware hw)
{
	bool inputChanged = false;
	if (dpadUp != lastActionUp)
	{
		lastActionUp = dpadUp;
		inputChanged = inputChanged || true;
	}
	if (dpadDown != lastActionDown)
	{
		lastActionDown = dpadDown;
		inputChanged = inputChanged || true;
	}
	if (dpadLeft != lastActionLeft)
	{
		lastActionLeft = dpadLeft;
		inputChanged = inputChanged || true;
	}
	if (dpadRight != lastActionRight)
	{
		lastActionRight = dpadRight;
		inputChanged = inputChanged || true;
	}
	if (inputChanged)
	{
		setActionValue(inputValue, hw);
	}
}

inline void JFF::InputBindingButtonGLFW::setActionValueCheckingAnyBehavior(int key, int newAction, bool inputValue, Input::Hardware hw)
{
	if (newAction == GLFW_REPEAT) // Ignore GLFW repeat events
		return;

	bool ignoreLastValueChecking = false;
	if (key != lastKey)
	{
		lastKey = key;
		ignoreLastValueChecking = true;
	}

	if (behavior->checkBehavior(inputValue, ignoreLastValueChecking))
	{
		setActionValue(inputValue, hw);
	}
}

void JFF::InputBindingButtonGLFW::setActionValueCheckingBehavior(int newAction, bool inputValue, Input::Hardware hw)
{
	if (newAction == GLFW_REPEAT) // Ignore GLFW repeat events
		return;

	setActionValueCheckingBehavior(inputValue, hw);
}

void JFF::InputBindingButtonGLFW::setActionValueCheckingBehavior(bool inputValue, Input::Hardware hw)
{
	if (behavior->checkBehavior(inputValue))
	{
		setActionValue(inputValue, hw);
	}
}

void JFF::InputBindingButtonGLFW::setActionValueIfChanged(int newAction, bool inputValue, Input::Hardware hw)
{
	if (newAction == GLFW_REPEAT) // Ignore GLFW repeat events
		return;

	if (newAction != lastAction)
	{
		lastAction = newAction;
		setActionValue(inputValue, hw);
	}
}

void JFF::InputBindingButtonGLFW::setActionValueIfAnyChanged(int newKey, int newAction, bool inputValue, Input::Hardware hw)
{
	if (newAction == GLFW_REPEAT) // Ignore GLFW repeat events
		return;

	if (newKey != lastKey)
	{
		lastKey = newKey;
		lastAction = newAction;
		setActionValue(inputValue, hw);
	}
	else if (newAction != lastAction)
	{
		lastAction = newAction;
		setActionValue(inputValue, hw);
	}
}

void JFF::InputBindingButtonGLFW::setActionValue(bool inputValue, Input::Hardware hw)
{
	parentAction->setActionValue(inputValue, hw); // Send result to parent action
}

inline bool JFF::InputBindingButtonGLFW::applyProcessors(bool inputValue)
{
	bool result(inputValue);
	std::for_each(processors.begin(), processors.end(), [&result](const auto& proc)
		{
			result = proc->process(result);
		});
	return result;
}
