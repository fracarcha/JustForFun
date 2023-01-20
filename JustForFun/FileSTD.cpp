/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "FileSTD.h"
#include "FileSystemSetup.h"

#include "Log.h"
#include <fstream>
#include <sstream>

JFF::FileSTD::FileSTD(const char* filepath)
{
	JFF_LOG_INFO("Creating file")

	std::ifstream file;
	file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	try
	{
		std::string fullPath = std::string("Assets").append(1, JFF_SLASH).append(filepath);
		file.open(fullPath);
		std::ostringstream sstream;
		sstream << file.rdbuf();
		file.close();
		rawText = sstream.str();
	}
	catch (const std::ios_base::failure e)
	{
		JFF_LOG_ERROR(std::string("File error: ") + e.what())
	}
}

JFF::FileSTD::~FileSTD()
{
	JFF_LOG_INFO("Deleting file")
}

const std::string& JFF::FileSTD::rawData()
{
	return rawText;
}

