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

#if defined(JFF_GL) && defined(JFF_GLFW) 

#	define GLEW_STATIC // Used when linked against GLEW static library
#	include "GL/glew.h"
#	include "GLFW/glfw3.h"

#	define JFF_INPUT_MAPPING_TYPE int

#	define JFF_UNKNOWN_MAPPING				-1

#	define JFF_GAMEPAD_ANY					1000

#	define JFF_GAMEPAD_D_PAD				1001
#	define JFF_GAMEPAD_D_PAD_UP				GLFW_GAMEPAD_BUTTON_DPAD_UP
#	define JFF_GAMEPAD_D_PAD_DOWN			GLFW_GAMEPAD_BUTTON_DPAD_DOWN
#	define JFF_GAMEPAD_D_PAD_LEFT			GLFW_GAMEPAD_BUTTON_DPAD_LEFT
#	define JFF_GAMEPAD_D_PAD_RIGHT			GLFW_GAMEPAD_BUTTON_DPAD_RIGHT

#	define JFF_GAMEPAD_LEFT_STICK			1002
#	define JFF_GAMEPAD_RIGHT_STICK			1003

#	define JFF_GAMEPAD_LEFT_STICK_PRESS		GLFW_GAMEPAD_BUTTON_LEFT_THUMB
#	define JFF_GAMEPAD_RIGHT_STICK_PRESS	GLFW_GAMEPAD_BUTTON_RIGHT_THUMB

#	define JFF_GAMEPAD_LEFT_SHOULDER		GLFW_GAMEPAD_BUTTON_LEFT_BUMPER
#	define JFF_GAMEPAD_RIGHT_SHOULDER		GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER

#	define JFF_GAMEPAD_LEFT_TRIGGER			GLFW_GAMEPAD_AXIS_LEFT_TRIGGER
#	define JFF_GAMEPAD_RIGHT_TRIGGER		GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER

#	define JFF_GAMEPAD_SELECT				GLFW_GAMEPAD_BUTTON_BACK
#	define JFF_GAMEPAD_START				GLFW_GAMEPAD_BUTTON_START

#	define JFF_GAMEPAD_BUTTON_NORTH			GLFW_GAMEPAD_BUTTON_TRIANGLE
#	define JFF_GAMEPAD_BUTTON_SOUTH			GLFW_GAMEPAD_BUTTON_CROSS
#	define JFF_GAMEPAD_BUTTON_EAST			GLFW_GAMEPAD_BUTTON_CIRCLE
#	define JFF_GAMEPAD_BUTTON_WEST			GLFW_GAMEPAD_BUTTON_SQUARE

#	define JFF_MOUSE_ANY					1004

#	define JFF_MOUSE_POSITION				1005
#	define JFF_MOUSE_DELTA					1006

#	define JFF_MOUSE_BUTTON_1 				 GLFW_MOUSE_BUTTON_1
#	define JFF_MOUSE_BUTTON_2 				 GLFW_MOUSE_BUTTON_2
#	define JFF_MOUSE_BUTTON_3 				 GLFW_MOUSE_BUTTON_3
#	define JFF_MOUSE_BUTTON_4 				 GLFW_MOUSE_BUTTON_4
#	define JFF_MOUSE_BUTTON_5 				 GLFW_MOUSE_BUTTON_5

#	define JFF_MOUSE_SCROLL 				1007
#	define JFF_MOUSE_SCROLL_UP 				1008
#	define JFF_MOUSE_SCROLL_DOWN 			1009
#	define JFF_MOUSE_SCROLL_LEFT 			1010
#	define JFF_MOUSE_SCROLL_RIGHT			1011
										
#	define JFF_KEYBOARD_ANY 				1012
											
#	define JFF_KEYBOARD_0					GLFW_KEY_0
#	define JFF_KEYBOARD_1					GLFW_KEY_1
#	define JFF_KEYBOARD_2					GLFW_KEY_2
#	define JFF_KEYBOARD_3					GLFW_KEY_3
#	define JFF_KEYBOARD_4					GLFW_KEY_4
#	define JFF_KEYBOARD_5					GLFW_KEY_5
#	define JFF_KEYBOARD_6					GLFW_KEY_6
#	define JFF_KEYBOARD_7					GLFW_KEY_7
#	define JFF_KEYBOARD_8					GLFW_KEY_8
#	define JFF_KEYBOARD_9					GLFW_KEY_9
											
#	define JFF_KEYBOARD_A					GLFW_KEY_A
#	define JFF_KEYBOARD_B					GLFW_KEY_B
#	define JFF_KEYBOARD_C					GLFW_KEY_C
#	define JFF_KEYBOARD_D					GLFW_KEY_D
#	define JFF_KEYBOARD_E					GLFW_KEY_E
#	define JFF_KEYBOARD_F					GLFW_KEY_F
#	define JFF_KEYBOARD_G					GLFW_KEY_G
#	define JFF_KEYBOARD_H					GLFW_KEY_H
#	define JFF_KEYBOARD_I					GLFW_KEY_I
#	define JFF_KEYBOARD_J					GLFW_KEY_J
#	define JFF_KEYBOARD_K					GLFW_KEY_K
#	define JFF_KEYBOARD_L					GLFW_KEY_L
#	define JFF_KEYBOARD_M					GLFW_KEY_M
#	define JFF_KEYBOARD_N					GLFW_KEY_N
#	define JFF_KEYBOARD_O					GLFW_KEY_O
#	define JFF_KEYBOARD_P					GLFW_KEY_P
#	define JFF_KEYBOARD_Q					GLFW_KEY_Q
#	define JFF_KEYBOARD_R					GLFW_KEY_R
#	define JFF_KEYBOARD_S					GLFW_KEY_S
#	define JFF_KEYBOARD_T					GLFW_KEY_T
#	define JFF_KEYBOARD_U					GLFW_KEY_U
#	define JFF_KEYBOARD_V					GLFW_KEY_V
#	define JFF_KEYBOARD_W					GLFW_KEY_W
#	define JFF_KEYBOARD_X					GLFW_KEY_X
#	define JFF_KEYBOARD_Y					GLFW_KEY_Y
#	define JFF_KEYBOARD_Z					GLFW_KEY_Z

#	define JFF_KEYBOARD_WASD				1013

#	define JFF_KEYBOARD_UP					GLFW_KEY_UP
#	define JFF_KEYBOARD_DOWN				GLFW_KEY_DOWN
#	define JFF_KEYBOARD_LEFT				GLFW_KEY_LEFT
#	define JFF_KEYBOARD_RIGHT				GLFW_KEY_RIGHT

#	define JFF_KEYBOARD_ARROWS				1014

#	define JFF_KEYBOARD_SPACE				GLFW_KEY_SPACE
#	define JFF_KEYBOARD_ESC					GLFW_KEY_ESCAPE
#	define JFF_KEYBOARD_ENTER				GLFW_KEY_ENTER
#	define JFF_KEYBOARD_BACKSPACE			GLFW_KEY_BACKSPACE
#	define JFF_KEYBOARD_TAB					GLFW_KEY_TAB
#	define JFF_KEYBOARD_INSERT				GLFW_KEY_INSERT
#	define JFF_KEYBOARD_DELETE				GLFW_KEY_DELETE
#	define JFF_KEYBOARD_PAGE_UP				GLFW_KEY_PAGE_UP
#	define JFF_KEYBOARD_PAGE_DOWN			GLFW_KEY_PAGE_DOWN
#	define JFF_KEYBOARD_HOME				GLFW_KEY_HOME
#	define JFF_KEYBOARD_END					GLFW_KEY_END

#	define JFF_KEYBOARD_CAPS_LOCK			GLFW_KEY_CAPS_LOCK
#	define JFF_KEYBOARD_SCROLL_LOCK			GLFW_KEY_SCROLL_LOCK
#	define JFF_KEYBOARD_NUM_LOCK			GLFW_KEY_NUM_LOCK
#	define JFF_KEYBOARD_PRINT_SCREEN		GLFW_KEY_PRINT_SCREEN
#	define JFF_KEYBOARD_PAUSE				GLFW_KEY_PAUSE

#	define JFF_KEYBOARD_LEFT_SHIFT			GLFW_KEY_LEFT_SHIFT
#	define JFF_KEYBOARD_RIGHT_SHIFT			GLFW_KEY_RIGHT_SHIFT

#	define JFF_KEYBOARD_LEFT_CONTROL		GLFW_KEY_LEFT_CONTROL
#	define JFF_KEYBOARD_RIGHT_CONTROL		GLFW_KEY_RIGHT_CONTROL

#	define JFF_KEYBOARD_LEFT_ALT			GLFW_KEY_LEFT_ALT
#	define JFF_KEYBOARD_RIGHT_ALT			GLFW_KEY_RIGHT_ALT

#	define JFF_KEYBOARD_LEFT_SUPER			GLFW_KEY_LEFT_SUPER
#	define JFF_KEYBOARD_RIGHT_SUPER			GLFW_KEY_RIGHT_SUPER

#else
#	error No API defined for input mapping
#endif

#include <string>
#include <map>
#include <stdexcept>

namespace JFF
{
	enum class Mapping : JFF_INPUT_MAPPING_TYPE
	{
		UNKNOWN_MAPPING						= JFF_UNKNOWN_MAPPING,

		// ----------------------------------- GAMEPAD ----------------------------------- //

		GAMEPAD_ANY							= JFF_GAMEPAD_ANY, // Any gamepad button (neither triggers nor sticks) movement will trigger an action
											  
		GAMEPAD_D_PAD						= JFF_GAMEPAD_D_PAD,
		GAMEPAD_D_PAD_UP					= JFF_GAMEPAD_D_PAD_UP,
		GAMEPAD_D_PAD_DOWN					= JFF_GAMEPAD_D_PAD_DOWN,
		GAMEPAD_D_PAD_LEFT					= JFF_GAMEPAD_D_PAD_LEFT,
		GAMEPAD_D_PAD_RIGHT					= JFF_GAMEPAD_D_PAD_RIGHT,
											  
		GAMEPAD_LEFT_STICK					= JFF_GAMEPAD_LEFT_STICK,
		GAMEPAD_RIGHT_STICK					= JFF_GAMEPAD_RIGHT_STICK,
											  
		GAMEPAD_LEFT_STICK_PRESS			= JFF_GAMEPAD_LEFT_STICK_PRESS,
		GAMEPAD_RIGHT_STICK_PRESS			= JFF_GAMEPAD_RIGHT_STICK_PRESS,
											  
		GAMEPAD_LEFT_SHOULDER				= JFF_GAMEPAD_LEFT_SHOULDER, // Example: L1 on PlayStation
		GAMEPAD_RIGHT_SHOULDER				= JFF_GAMEPAD_RIGHT_SHOULDER, // Example: R1 on Playstation
											  
		GAMEPAD_LEFT_TRIGGER				= JFF_GAMEPAD_LEFT_TRIGGER, // Example: L2 on PlayStation
		GAMEPAD_RIGHT_TRIGGER				= JFF_GAMEPAD_RIGHT_TRIGGER, // Example: R2 on PlayStation
											  
		GAMEPAD_SELECT						= JFF_GAMEPAD_SELECT,
		GAMEPAD_START						= JFF_GAMEPAD_START,
											  
		GAMEPAD_BUTTON_NORTH				= JFF_GAMEPAD_BUTTON_NORTH, // Example: Triangle on PlayStation
		GAMEPAD_BUTTON_SOUTH				= JFF_GAMEPAD_BUTTON_SOUTH, // Example: Cross on PlayStation
		GAMEPAD_BUTTON_EAST					= JFF_GAMEPAD_BUTTON_EAST, // Example: Circle on PlayStation
		GAMEPAD_BUTTON_WEST					= JFF_GAMEPAD_BUTTON_WEST, // Example: Square on PlayStation
											  
		// ----------------------------------- MOUSE ----------------------------------- //
											  
		MOUSE_ANY							= JFF_MOUSE_ANY, // Any mouse button (not mouse movement or scroll) will trigger an action
											  
		MOUSE_POSITION						= JFF_MOUSE_POSITION, // Position in pixel coordinates
		MOUSE_DELTA							= JFF_MOUSE_DELTA, // Change of position since the last frame
											  
		MOUSE_BUTTON_1						= JFF_MOUSE_BUTTON_1, // Left click
		MOUSE_BUTTON_2						= JFF_MOUSE_BUTTON_2, // Right click
		MOUSE_BUTTON_3						= JFF_MOUSE_BUTTON_3, // Center click
		MOUSE_BUTTON_4						= JFF_MOUSE_BUTTON_4, // Usually, one of the side buttons on modern mouses
		MOUSE_BUTTON_5						= JFF_MOUSE_BUTTON_5, // Usually, one of the side buttons on modern mouses
											  
		MOUSE_LEFT_CLICK = MOUSE_BUTTON_1,	  
		MOUSE_RIGHT_CLICK = MOUSE_BUTTON_2,	  
		MOUSE_CENTER_CLICK = MOUSE_BUTTON_3,  
											  
		MOUSE_SCROLL						= JFF_MOUSE_SCROLL, // Two direction vector when scrolling up, down, (left and right for mouses with scroll wheel tilt)
		MOUSE_SCROLL_UP						= JFF_MOUSE_SCROLL_UP, // Vector that points up when scrolling up
		MOUSE_SCROLL_DOWN					= JFF_MOUSE_SCROLL_DOWN, // Vector that points down when scrolling down
		MOUSE_SCROLL_LEFT					= JFF_MOUSE_SCROLL_LEFT, // Vector that points left when tilt scroll wheel on that direction
		MOUSE_SCROLL_RIGHT					= JFF_MOUSE_SCROLL_RIGHT, // Vector that points right when tilt scroll wheel on that direction
											  
		// ----------------------------------- KEYBOARD ----------------------------------- //
											  
		KEYBOARD_ANY						= JFF_KEYBOARD_ANY, // Any key press will trigger an action
											  
		KEYBOARD_0							= JFF_KEYBOARD_0,
		KEYBOARD_1							= JFF_KEYBOARD_1,
		KEYBOARD_2							= JFF_KEYBOARD_2,
		KEYBOARD_3							= JFF_KEYBOARD_3,
		KEYBOARD_4							= JFF_KEYBOARD_4,
		KEYBOARD_5							= JFF_KEYBOARD_5,
		KEYBOARD_6							= JFF_KEYBOARD_6,
		KEYBOARD_7							= JFF_KEYBOARD_7,
		KEYBOARD_8							= JFF_KEYBOARD_8,
		KEYBOARD_9							= JFF_KEYBOARD_9,
											  
		KEYBOARD_A							= JFF_KEYBOARD_A,
		KEYBOARD_B							= JFF_KEYBOARD_B,
		KEYBOARD_C							= JFF_KEYBOARD_C,
		KEYBOARD_D							= JFF_KEYBOARD_D,
		KEYBOARD_E							= JFF_KEYBOARD_E,
		KEYBOARD_F							= JFF_KEYBOARD_F,
		KEYBOARD_G							= JFF_KEYBOARD_G,
		KEYBOARD_H							= JFF_KEYBOARD_H,
		KEYBOARD_I							= JFF_KEYBOARD_I,
		KEYBOARD_J							= JFF_KEYBOARD_J,
		KEYBOARD_K							= JFF_KEYBOARD_K,
		KEYBOARD_L							= JFF_KEYBOARD_L,
		KEYBOARD_M							= JFF_KEYBOARD_M,
		KEYBOARD_N							= JFF_KEYBOARD_N,
		KEYBOARD_O							= JFF_KEYBOARD_O,
		KEYBOARD_P							= JFF_KEYBOARD_P,
		KEYBOARD_Q							= JFF_KEYBOARD_Q,
		KEYBOARD_R							= JFF_KEYBOARD_R,
		KEYBOARD_S							= JFF_KEYBOARD_S,
		KEYBOARD_T							= JFF_KEYBOARD_T,
		KEYBOARD_U							= JFF_KEYBOARD_U,
		KEYBOARD_V							= JFF_KEYBOARD_V,
		KEYBOARD_W							= JFF_KEYBOARD_W,
		KEYBOARD_X							= JFF_KEYBOARD_X,
		KEYBOARD_Y							= JFF_KEYBOARD_Y,
		KEYBOARD_Z							= JFF_KEYBOARD_Z,
											  
		KEYBOARD_WASD						= JFF_KEYBOARD_WASD, // Two dimensional vector with W,A,S,D keys mapped to describe a direction
											  
		KEYBOARD_UP							= JFF_KEYBOARD_UP,
		KEYBOARD_DOWN						= JFF_KEYBOARD_DOWN,
		KEYBOARD_LEFT						= JFF_KEYBOARD_LEFT,
		KEYBOARD_RIGHT						= JFF_KEYBOARD_RIGHT,
											  
		KEYBOARD_ARROWS						= JFF_KEYBOARD_ARROWS, // Two dimensional vector with UP, DOWN, LEFT and RIGHT keys mapped to describe a direction
											  
		KEYBOARD_SPACE						= JFF_KEYBOARD_SPACE,
		KEYBOARD_ESC						= JFF_KEYBOARD_ESC,
		KEYBOARD_ENTER						= JFF_KEYBOARD_ENTER,
		KEYBOARD_BACKSPACE					= JFF_KEYBOARD_BACKSPACE,
		KEYBOARD_TAB						= JFF_KEYBOARD_TAB,
		KEYBOARD_INSERT						= JFF_KEYBOARD_INSERT,
		KEYBOARD_DELETE						= JFF_KEYBOARD_DELETE, // 'Supr' on spanish keyboard
		KEYBOARD_PAGE_UP					= JFF_KEYBOARD_PAGE_UP,
		KEYBOARD_PAGE_DOWN					= JFF_KEYBOARD_PAGE_DOWN,
		KEYBOARD_HOME						= JFF_KEYBOARD_HOME, // 'Inicio' on spanish keyboard
		KEYBOARD_END						= JFF_KEYBOARD_END,
											  
		KEYBOARD_CAPS_LOCK					= JFF_KEYBOARD_CAPS_LOCK,
		KEYBOARD_SCROLL_LOCK				= JFF_KEYBOARD_SCROLL_LOCK,
		KEYBOARD_NUM_LOCK					= JFF_KEYBOARD_NUM_LOCK,
		KEYBOARD_PRINT_SCREEN				= JFF_KEYBOARD_PRINT_SCREEN,
		KEYBOARD_PAUSE						= JFF_KEYBOARD_PAUSE,
											  
		KEYBOARD_LEFT_SHIFT					= JFF_KEYBOARD_LEFT_SHIFT,
		KEYBOARD_RIGHT_SHIFT				= JFF_KEYBOARD_RIGHT_SHIFT,
											  
		KEYBOARD_LEFT_CONTROL				= JFF_KEYBOARD_LEFT_CONTROL,
		KEYBOARD_RIGHT_CONTROL				= JFF_KEYBOARD_RIGHT_CONTROL,
											  
		KEYBOARD_LEFT_ALT					= JFF_KEYBOARD_LEFT_ALT,
		KEYBOARD_RIGHT_ALT					= JFF_KEYBOARD_RIGHT_ALT,
											  
		KEYBOARD_LEFT_SUPER					= JFF_KEYBOARD_LEFT_SUPER, // Example: 'Windows' key on Windows
		KEYBOARD_RIGHT_SUPER				= JFF_KEYBOARD_RIGHT_SUPER,

		// TODO: Add more mappings (Joysticks, sensors, touchscreen, XR controllers, ...)

	};
}