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

#include <string>
#include <vector>
#include <functional>

namespace JFF
{
	class INIFile
	{
	public:
		// Ctor & Dtor
		INIFile() {}
		virtual ~INIFile() {}

		// Copy ctor and copy assignment
		INIFile(const INIFile& other) = delete;
		INIFile& operator=(const INIFile& other) = delete;

		// Move ctor and assignment
		INIFile(INIFile&& other) = delete;
		INIFile operator=(INIFile&& other) = delete;

		// ------------------------------ INI FILE FUNCTIONS ------------------------------ //

		// NOTE: In this API, section refers to lines in brackets in a INI file. i.e. given [MATERIAL], section == material
		// NOTE: In this API, key is the left part of a pair in a INI file: key = value

		virtual bool has(const char* section, const char* key) = 0;
		virtual int getInt(const char* section, const char* key) = 0;
		virtual float getFloat(const char* section, const char* key) = 0;
		virtual std::string getString(const char* section, const char* key) = 0;
		virtual void getAllSections(std::vector<std::string>& outSections) = 0;
		virtual void visitKeyValuePairs(
			const char* section, 
			const std::function<void(const std::pair<std::string, std::string>&)>& visitor) = 0;
	};
}