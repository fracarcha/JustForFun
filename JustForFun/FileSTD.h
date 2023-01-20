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

#include "File.h"

namespace JFF
{
	class FileSTD : public File
	{
	public:
		// Ctor & Dtor
		explicit FileSTD(const char* filepath);
		virtual ~FileSTD();

		// Copy ctor and copy assignment
		FileSTD(const FileSTD& other) = delete;
		FileSTD& operator=(const FileSTD& other) = delete;

		// Move ctor and assignment
		FileSTD(FileSTD&& other) = delete;
		FileSTD operator=(FileSTD&& other) = delete;

		// Some functions
		virtual const std::string& rawData();

	protected:
		std::string rawText;
	};
}