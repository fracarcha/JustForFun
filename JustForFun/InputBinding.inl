/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputBinding.h"

#include "Log.h"

template<typename _Ret>
inline JFF::Mapping JFF::InputBinding<_Ret>::stringToMapping(const std::string& mappingName)
{
	static std::map<std::string, Mapping> staticMapping =
	{
		{"GAMEPAD_ANY", Mapping::GAMEPAD_ANY},

		{"GAMEPAD_D_PAD", Mapping::GAMEPAD_D_PAD},
		{"GAMEPAD_D_PAD_UP", Mapping::GAMEPAD_D_PAD_UP},
		{"GAMEPAD_D_PAD_DOWN", Mapping::GAMEPAD_D_PAD_DOWN},
		{"GAMEPAD_D_PAD_LEFT", Mapping::GAMEPAD_D_PAD_LEFT},
		{"GAMEPAD_D_PAD_RIGHT", Mapping::GAMEPAD_D_PAD_RIGHT},

		{"GAMEPAD_LEFT_STICK", Mapping::GAMEPAD_LEFT_STICK},
		{"GAMEPAD_RIGHT_STICK", Mapping::GAMEPAD_RIGHT_STICK},

		{"GAMEPAD_LEFT_STICK_PRESS", Mapping::GAMEPAD_LEFT_STICK_PRESS},
		{"GAMEPAD_RIGHT_STICK_PRESS", Mapping::GAMEPAD_RIGHT_STICK_PRESS},

		{"GAMEPAD_LEFT_SHOULDER", Mapping::GAMEPAD_LEFT_SHOULDER },
		{"GAMEPAD_RIGHT_SHOULDER", Mapping::GAMEPAD_RIGHT_SHOULDER },

		{"GAMEPAD_LEFT_TRIGGER", Mapping::GAMEPAD_LEFT_TRIGGER },
		{"GAMEPAD_RIGHT_TRIGGER", Mapping::GAMEPAD_RIGHT_TRIGGER },

		{"GAMEPAD_SELECT", Mapping::GAMEPAD_SELECT},
		{"GAMEPAD_START", Mapping::GAMEPAD_START},

		{"GAMEPAD_BUTTON_NORTH", Mapping::GAMEPAD_BUTTON_NORTH },
		{"GAMEPAD_BUTTON_SOUTH", Mapping::GAMEPAD_BUTTON_SOUTH },
		{"GAMEPAD_BUTTON_EAST", Mapping::GAMEPAD_BUTTON_EAST },
		{"GAMEPAD_BUTTON_WEST", Mapping::GAMEPAD_BUTTON_WEST },


		{"MOUSE_ANY", Mapping::MOUSE_ANY },

		{"MOUSE_POSITION", Mapping::MOUSE_POSITION },
		{"MOUSE_DELTA", Mapping::MOUSE_DELTA },

		{"MOUSE_BUTTON_1", Mapping::MOUSE_BUTTON_1 },
		{"MOUSE_BUTTON_2", Mapping::MOUSE_BUTTON_2 },
		{"MOUSE_BUTTON_3", Mapping::MOUSE_BUTTON_3 },
		{"MOUSE_BUTTON_4", Mapping::MOUSE_BUTTON_4 },
		{"MOUSE_BUTTON_5", Mapping::MOUSE_BUTTON_5 },

		{"MOUSE_LEFT_CLICK", Mapping::MOUSE_BUTTON_1},
		{"MOUSE_RIGHT_CLICK", Mapping::MOUSE_BUTTON_2},
		{"MOUSE_CENTER_CLICK", Mapping::MOUSE_BUTTON_3},

		{"MOUSE_SCROLL", Mapping::MOUSE_SCROLL },
		{"MOUSE_SCROLL_UP", Mapping::MOUSE_SCROLL_UP },
		{"MOUSE_SCROLL_DOWN", Mapping::MOUSE_SCROLL_DOWN },
		{"MOUSE_SCROLL_LEFT", Mapping::MOUSE_SCROLL_LEFT },
		{"MOUSE_SCROLL_RIGHT", Mapping::MOUSE_SCROLL_RIGHT },


		{"KEYBOARD_ANY", Mapping::KEYBOARD_ANY },

		{"KEYBOARD_0", Mapping::KEYBOARD_0},
		{"KEYBOARD_1", Mapping::KEYBOARD_1},
		{"KEYBOARD_2", Mapping::KEYBOARD_2},
		{"KEYBOARD_3", Mapping::KEYBOARD_3},
		{"KEYBOARD_4", Mapping::KEYBOARD_4},
		{"KEYBOARD_5", Mapping::KEYBOARD_5},
		{"KEYBOARD_6", Mapping::KEYBOARD_6},
		{"KEYBOARD_7", Mapping::KEYBOARD_7},
		{"KEYBOARD_8", Mapping::KEYBOARD_8},
		{"KEYBOARD_9", Mapping::KEYBOARD_9},

		{"KEYBOARD_A", Mapping::KEYBOARD_A},
		{"KEYBOARD_B", Mapping::KEYBOARD_B},
		{"KEYBOARD_C", Mapping::KEYBOARD_C},
		{"KEYBOARD_D", Mapping::KEYBOARD_D},
		{"KEYBOARD_E", Mapping::KEYBOARD_E},
		{"KEYBOARD_F", Mapping::KEYBOARD_F},
		{"KEYBOARD_G", Mapping::KEYBOARD_G},
		{"KEYBOARD_H", Mapping::KEYBOARD_H},
		{"KEYBOARD_I", Mapping::KEYBOARD_I},
		{"KEYBOARD_J", Mapping::KEYBOARD_J},
		{"KEYBOARD_K", Mapping::KEYBOARD_K},
		{"KEYBOARD_L", Mapping::KEYBOARD_L},
		{"KEYBOARD_M", Mapping::KEYBOARD_M},
		{"KEYBOARD_N", Mapping::KEYBOARD_N},
		{"KEYBOARD_O", Mapping::KEYBOARD_O},
		{"KEYBOARD_P", Mapping::KEYBOARD_P},
		{"KEYBOARD_Q", Mapping::KEYBOARD_Q},
		{"KEYBOARD_R", Mapping::KEYBOARD_R},
		{"KEYBOARD_S", Mapping::KEYBOARD_S},
		{"KEYBOARD_T", Mapping::KEYBOARD_T},
		{"KEYBOARD_U", Mapping::KEYBOARD_U},
		{"KEYBOARD_V", Mapping::KEYBOARD_V},
		{"KEYBOARD_W", Mapping::KEYBOARD_W},
		{"KEYBOARD_X", Mapping::KEYBOARD_X},
		{"KEYBOARD_Y", Mapping::KEYBOARD_Y},
		{"KEYBOARD_Z", Mapping::KEYBOARD_Z},

		{"KEYBOARD_WASD", Mapping::KEYBOARD_WASD },

		{"KEYBOARD_UP", Mapping::KEYBOARD_UP},
		{"KEYBOARD_DOWN", Mapping::KEYBOARD_DOWN},
		{"KEYBOARD_LEFT", Mapping::KEYBOARD_LEFT},
		{"KEYBOARD_RIGHT", Mapping::KEYBOARD_RIGHT},

		{"KEYBOARD_ARROWS", Mapping::KEYBOARD_ARROWS },

		{"KEYBOARD_SPACE", Mapping::KEYBOARD_SPACE},
		{"KEYBOARD_ESC", Mapping::KEYBOARD_ESC},
		{"KEYBOARD_ENTER", Mapping::KEYBOARD_ENTER},
		{"KEYBOARD_BACKSPACE", Mapping::KEYBOARD_BACKSPACE},
		{"KEYBOARD_TAB", Mapping::KEYBOARD_TAB},
		{"KEYBOARD_INSERT", Mapping::KEYBOARD_INSERT},
		{"KEYBOARD_DELETE", Mapping::KEYBOARD_DELETE },
		{"KEYBOARD_PAGE_UP", Mapping::KEYBOARD_PAGE_UP},
		{"KEYBOARD_PAGE_DOWN", Mapping::KEYBOARD_PAGE_DOWN},
		{"KEYBOARD_HOME", Mapping::KEYBOARD_HOME },
		{"KEYBOARD_END", Mapping::KEYBOARD_END},

		{"KEYBOARD_CAPS_LOCK", Mapping::KEYBOARD_CAPS_LOCK},
		{"KEYBOARD_SCROLL_LOCK", Mapping::KEYBOARD_SCROLL_LOCK},
		{"KEYBOARD_NUM_LOCK", Mapping::KEYBOARD_NUM_LOCK},
		{"KEYBOARD_PRINT_SCREEN", Mapping::KEYBOARD_PRINT_SCREEN},
		{"KEYBOARD_PAUSE", Mapping::KEYBOARD_PAUSE},

		{"KEYBOARD_LEFT_SHIFT", Mapping::KEYBOARD_LEFT_SHIFT},
		{"KEYBOARD_RIGHT_SHIFT", Mapping::KEYBOARD_RIGHT_SHIFT},

		{"KEYBOARD_LEFT_CONTROL", Mapping::KEYBOARD_LEFT_CONTROL},
		{"KEYBOARD_RIGHT_CONTROL", Mapping::KEYBOARD_RIGHT_CONTROL},

		{"KEYBOARD_LEFT_ALT", Mapping::KEYBOARD_LEFT_ALT},
		{"KEYBOARD_RIGHT_ALT", Mapping::KEYBOARD_RIGHT_ALT},

		{"KEYBOARD_LEFT_SUPER", Mapping::KEYBOARD_LEFT_SUPER },
		{"KEYBOARD_RIGHT_SUPER", Mapping::KEYBOARD_RIGHT_SUPER},
	};

	Mapping result = Mapping::UNKNOWN_MAPPING;
	try
	{
		result = staticMapping.at(mappingName);
	}
	catch (std::out_of_range e)
	{
		JFF_LOG_ERROR("Input mapping not found. Mapping name: " << mappingName)
	}

	return result;
}



