/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "INIFileMINI.h"
#include "FileSystemSetup.h"

#include "Log.h"
#include <stdexcept>

JFF::INIFileMINI::INIFileMINI(const char* filepath) : 
	iniData()
{
	JFF_LOG_INFO("Ctor INIFileMINI")

	std::string fullPath = std::string("Assets").append(1, JFF_SLASH).append(filepath);
	mINI::INIFile file(fullPath);
	file.read(iniData);
}

JFF::INIFileMINI::~INIFileMINI()
{
	JFF_LOG_INFO("Dtor INIFileMINI")
}

bool JFF::INIFileMINI::has(const char* section, const char* key) 
{
	return iniData.has(section) && iniData[section].has(key);
}

int JFF::INIFileMINI::getInt(const char* section, const char* key) 
{
	int result = 0;
	if (has(section, key))
	{
		std::string intString = iniData[section][key];
		try
		{
			result = std::stoi(intString);
		}
		catch (std::invalid_argument e)
		{
			JFF_LOG_ERROR("Cannot convert INI file value to int. Exception trace: " << e.what())
		}
		catch (std::out_of_range e)
		{
			JFF_LOG_ERROR("Overflow while converting INI file value to int. Exception trace: " << e.what())
		}
	}
	else
	{
		JFF_LOG_WARNING("INI file doesn't contain section " << section << " or key " << key)
	}

	return result;
}

float JFF::INIFileMINI::getFloat(const char* section, const char* key) 
{
	float result = 0;
	if (has(section, key))
	{
		std::string floatString = iniData[section][key];
		try
		{
			result = std::stof(floatString);
		}
		catch (std::invalid_argument e)
		{
			JFF_LOG_ERROR("Cannot convert INI file value to float. Exception trace: " << e.what())
		}
		catch (std::out_of_range e)
		{
			JFF_LOG_ERROR("Overflow while converting INI file value to float. Exception trace: " << e.what())
		}
	}
	else
	{
		JFF_LOG_WARNING("INI file doesn't contain section " << section << " or key " << key)
	}

	return result;
}

std::string JFF::INIFileMINI::getString(const char* section, const char* key) 
{
	if (has(section, key))
	{
		return iniData[section][key];
	}
	else
	{
		JFF_LOG_WARNING("INI file doesn't contain section " << section << " or key " << key)
	}

	return std::string();
}

void JFF::INIFileMINI::getAllSections(std::vector<std::string>& outSections)
{
	std::for_each(iniData.begin(), iniData.end(), [&outSections](const auto& pair)
		{
			outSections.push_back(pair.first);
		});
}

void JFF::INIFileMINI::visitKeyValuePairs(
	const char* section,
	const std::function<void(const std::pair<std::string, std::string>&)>& visitor)
{
	auto& sectionContainer = iniData[section];
	std::for_each(sectionContainer.begin(), sectionContainer.end(), visitor);
}
