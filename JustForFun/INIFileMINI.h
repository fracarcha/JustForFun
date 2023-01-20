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

#include "INIFile.h"
#include "mINI.h"

namespace JFF
{
	class INIFileMINI : public INIFile
	{
	public:
		// Ctor & Dtor
		INIFileMINI(const char* filepath);
		virtual ~INIFileMINI();

		// Copy ctor and copy assignment
		INIFileMINI(const INIFileMINI& other) = delete;
		INIFileMINI& operator=(const INIFileMINI& other) = delete;

		// Move ctor and assignment
		INIFileMINI(INIFileMINI&& other) = delete;
		INIFileMINI operator=(INIFileMINI&& other) = delete;

		// ------------------------------ INI FILE OVERRIDES ------------------------------ //

		// NOTE: In this API, section refers to lines in brackets in a INI file. i.e. given [MATERIAL], section == material
		// NOTE: In this API, key is the left part of a pair in a INI file: key = value

		virtual bool has(const char* section, const char* key) override;
		virtual int getInt(const char* section, const char* key) override;
		virtual float getFloat(const char* section, const char* key) override;
		virtual std::string getString(const char* section, const char* key) override;
		virtual void getAllSections(std::vector<std::string>& outSections) override;
		virtual void visitKeyValuePairs(
			const char* section,
			const std::function<void(const std::pair<std::string, std::string>&)>& visitor) override;

	protected:
		mINI::INIStructure iniData;
	};
}