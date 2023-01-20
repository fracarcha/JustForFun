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

#include "ExecutableSubsystem.h"

#include <functional>

namespace JFF
{
	class Context : public ExecutableSubsystem
	{
	public:
		// Ctor & Dtor
		Context() {}
		virtual ~Context() {}

		// Copy ctor and copy assignment
		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

		// Move ctor and assignment
		Context(Context&& other) = delete;
		Context operator=(Context&& other) = delete;

		// --------------------------------- CONTEXT INTERFACE --------------------------------- //
		
		/*
		* Adds or removes a listener that will receive a notification when framebuffer size is changed.
		* The returned value is needed to remove the listener
		*/
		virtual unsigned long long int addOnFramebufferSizeChangedListener(const std::function<void(int,int)>& listener) = 0;
		virtual void removeOnFramebufferSizeChangedListener(unsigned long long int listenerHandler) = 0;

		virtual std::string getClientGraphicsAPI() const = 0;
		virtual int getGraphicsAPIVersionMajor() const = 0;
		virtual int getGraphicsAPIVersionMinor() const = 0;
		virtual int getGraphicsAPIRevisionNumber() const = 0;
		virtual std::string getGraphicsAPIContexProfile() const = 0;

		virtual void getWindowSizeInScreenCoordinates(int& outWidth, int& outHeight) const = 0;
		virtual void getFramebufferSizeInPixels(int& outWidth, int& outHeight) const = 0;
	};
}