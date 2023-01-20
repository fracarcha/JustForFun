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

#include "Context.h"
#include <map>

struct GLFWwindow;
struct GLFWmonitor;

namespace JFF
{
	// GLFW implementation of Context
	class ContextGLFW : public Context
	{
	public:
		// Ctor & Dtor
		ContextGLFW();
		virtual ~ContextGLFW();

		// Copy ctor and copy assignment
		ContextGLFW(const ContextGLFW& other) = delete;
		ContextGLFW& operator=(const ContextGLFW& other) = delete;

		// Move ctor and assignment
		ContextGLFW(ContextGLFW&& other) = delete;
		ContextGLFW operator=(ContextGLFW&& other) = delete;

		// ---------------------------------------- SUBSYSTEM INTERFACE ---------------------------------------- //

		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ---------------------------------------- EXECUTABLESUBSYSTEM INTERFACE ---------------------------------------- //

		virtual ExecutableSubsystem::ExecutionOrder getExecutionOrder() const override;
		virtual bool execute() override;

		// ---------------------------------------- CONTEXT INTERFACE ---------------------------------------- //

		virtual unsigned long long int addOnFramebufferSizeChangedListener(const std::function<void(int, int)>& listener) override;
		virtual void removeOnFramebufferSizeChangedListener(unsigned long long int listenerHandler) override;

		virtual std::string getClientGraphicsAPI() const override;
		virtual int getGraphicsAPIVersionMajor() const override;
		virtual int getGraphicsAPIVersionMinor() const override;
		virtual int getGraphicsAPIRevisionNumber() const override;
		virtual std::string getGraphicsAPIContexProfile() const override;

		virtual void getWindowSizeInScreenCoordinates(int& outWidth, int& outHeight) const override;
		virtual void getFramebufferSizeInPixels(int& outWidth, int& outHeight) const override;

		// ---------------------------------------- GLFW ONLY INTERFACE ---------------------------------------- //

		virtual GLFWwindow* getWindow() const { return window; }

	public:
		friend class GLFWContextCallbackAdaptor;

	private:
		enum class ScreenMode : char
		{
			WINDOWED,
			FULLSCREEN,
			WINDOWED_FULLSCREEN,
		};

		struct Params
		{
			int OpenGLVersionMajor;
			int OpenGLVersionMinor;
			ScreenMode screenMode;
			int resolutionWidth;
			int resolutionHeight;

			int framebufferRedBits;
			int framebufferGreenBits;
			int framebufferBlueBits;

			int monitorRefreshRate;
			bool vsync;
			int msaa;
		};
		inline Params loadConfigFile() const;
		inline void printContextInfo() const;

	protected:
		GLFWwindow* window;
		GLFWmonitor* monitor; // Representation of the physical monitor
		std::map<unsigned long long int, std::function<void(int, int)>> framebufferSizeCallbacks;
		unsigned long long int framebufferCallbackIndex; // Uniquely identifies each framebuffer callback function inside framebufferSizeCallbacks map
		bool vsync;
	};

	/*
	* Static class intended to use as GLFW callback function holder.
	* It would be much easier if GLFW would allow std::function as callback, but it's a C style library
	* and we have to work the hard way
	*/
	class GLFWContextCallbackAdaptor final
	{
	private: // Singleton objects need to hide constructors, cpoies, moves and assignments
		explicit GLFWContextCallbackAdaptor(ContextGLFW* const context);

	public:
		~GLFWContextCallbackAdaptor();

		// Copy ctor and copy assignment
		GLFWContextCallbackAdaptor(const GLFWContextCallbackAdaptor& other) = delete;
		GLFWContextCallbackAdaptor& operator=(const GLFWContextCallbackAdaptor& other) = delete;

		// Move ctor and assignment
		GLFWContextCallbackAdaptor(GLFWContextCallbackAdaptor&& other) = delete;
		GLFWContextCallbackAdaptor operator=(GLFWContextCallbackAdaptor&& other) = delete;

		// Singleton creation
		static GLFWContextCallbackAdaptor& getInstance(ContextGLFW* const context = nullptr);

		// Context callbacks
		static void framebufferCallback(GLFWwindow* window, int width, int height);

	private:
		ContextGLFW* context;
	};
}