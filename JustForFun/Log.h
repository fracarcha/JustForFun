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

// Debugging
#if defined(_DEBUG) && defined(_WIN64) // Console log (Debug only)
#	define NOMINMAX // Used to avoid Windows.h to define the very annoying macros "min" and "max"
#	include <Windows.h>
#	include <iostream>
	extern std::string JFFGetCurrentTime();
#	define _JFF_DATE_TIME JFFGetCurrentTime()
#	define _JFF_PARSE_FILENAME std::string(__FILE__).erase(std::string(__FILE__).rfind('.'), 4).erase(0, std::string(__FILE__).rfind('\\') + 1)
#	define _JFF_LOG(msg, type, textColor)																			 \
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor);									 \
			std::cout << "[" << _JFF_DATE_TIME << "][" << type << "][" << _JFF_PARSE_FILENAME << "] " << msg << std::endl;	

#	ifdef JFF_SUPRESS_LOW_PRIORITY_INFO_LOGS
#		define JFF_LOG_INFO_LOW_PRIORITY(msg)
#	else
#		define JFF_LOG_INFO_LOW_PRIORITY(msg) _JFF_LOG(msg, "INFO", 8)
#	endif // JFF_SUPRESS_INFO_LOGS_HERE

#	define JFF_LOG_INFO(msg) _JFF_LOG(msg, "INFO", 8)
#	define JFF_LOG_IMPORTANT(msg) _JFF_LOG(msg, "INFO", 15)
#	define JFF_LOG_SUPER_IMPORTANT(msg)	_JFF_LOG(msg, "INFO", 11)
#	define JFF_LOG_WARNING(msg) _JFF_LOG(msg, "WARNING", 14)
#	define JFF_LOG_ERROR(msg) _JFF_LOG(msg, "ERROR", 12)

#else

#	define JFF_LOG_INFO_LOW_PRIORITY(msg)
#	define JFF_LOG_INFO(msg) 
#	define JFF_LOG_IMPORTANT(msg)
#	define JFF_LOG_SUPER_IMPORTANT(msg)
#	define JFF_LOG_WARNING(msg)
#	define JFF_LOG_ERROR(msg)

#endif // _DEBUG Console log