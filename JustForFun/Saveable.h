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

namespace JFF
{
	// Interface indicating that whatever class that implements it can create a file to disk
	class Saveable
	{
	public:
		// Ctor & Dtor
		Saveable() {}
		virtual ~Saveable() {}

		// Copy ctor and copy assignment
		Saveable(const Saveable& other) = delete;
		Saveable& operator=(const Saveable& other) = delete;

		// Move ctor and assignment
		Saveable(Saveable&& other) = delete;
		Saveable operator=(Saveable&& other) = delete;

		// -------------------------------- SAVEABLE INTERFACE -------------------------------- //

		/*
		* Interface for writing a file to disk. The file will be stored in Assets folder and
		* Generated subfolder if the flag is true
		*/
		virtual void writeToFile(const char* newFilename, bool storeInGeneratedSubfolder = true) = 0;
	};
}