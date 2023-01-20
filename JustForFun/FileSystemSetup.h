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

#ifdef _WIN64 // Targeting Windows x64. TODO: Should add more macros here?
#	define JFF_SLASH '\\'
#	define JFF_SLASH_STRING "\\"
#else
#	define JFF_SLASH '/'
#	define JFF_SLASH_STRING "/"
#	error Unknown filesystem platform
#endif