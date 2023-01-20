/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Image.h"

#include <sstream>

std::string JFF::Image::generateCacheName(const char* filepath)
{
	std::ostringstream ss;
	ss << "Image://";
	ss << filepath;

	return ss.str();
}
