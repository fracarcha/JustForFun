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

namespace JFF
{
	/*	
	*	Interpretation of a file as raw string.
	*/
	class File
	{
	public:
		// Ctor & Dtor
		File() {}
		virtual ~File() {}

		// Copy ctor and copy assignment
		File(const File& other) = delete;
		File& operator=(const File& other) = delete;

		// Move ctor and assignment
		File(File&& other) = delete;
		File operator=(File&& other) = delete;
		
		// ------------------------------ FILE FUNCTIONS ------------------------------ //

		virtual const std::string& rawData() = 0;
	};
}