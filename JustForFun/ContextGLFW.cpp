#include "ContextGLFW.h"

#include "Log.h" // Log before Glew to calm down Windows.h redefinition warnings

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdexcept>
#include <algorithm>

JFF::ContextGLFW::ContextGLFW() : 
	window(nullptr),
	framebufferSizeCallbacks(),
	framebufferCallbackIndex(0ull)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: ContextGLFW")
}

JFF::ContextGLFW::~ContextGLFW()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: ContextGLFW")

	// Terminate GLFW, close windows and free resources
	glfwTerminate();
}

void JFF::ContextGLFW::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: ContextGLFW")

	// Init GLFW
	if (!glfwInit())
	{
		JFF_LOG_ERROR("GLFW could not be initialized")
		throw std::runtime_error("GLFW could not be initialized");
	}

	// TODO: Load config from file

	// Configure window hints before window creation
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version major: 3.x
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL version minor: x.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Removes older OpenGL functions
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Mac OSX mandatory feature
	glfwWindowHint(GLFW_SAMPLES, 4); // Sets MSAA (Multisample Anti-Aliasing) to 4 subsamples per pixel. This increases framebuffer size by 4 (MSAA is incompatible with deferred shading)
	// TODO: Window hints

	// Create a window with a OpenGL context and make it current for this thread
	bool fullscreen = false;
	window = glfwCreateWindow(800, 600, "MyWindow", fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr); // TODO: Load window options from file
	if (window == nullptr)
	{
		glfwTerminate();
		JFF_LOG_ERROR("GLFW could not create a window")
		throw std::runtime_error("GLFW could not create a window");
	}

	// Binds this thread with window's OpenGL context. From now on, all OpenGL calls will affect this window
	glfwMakeContextCurrent(window);

	// Enable VSync
	glfwSwapInterval(1); // TODO: Check vsync

	// Create the first instance of GLFWContextCallbackAdaptor and make it hold a pointer to this object
	GLFWContextCallbackAdaptor::getInstance(this);

	// Set framebuffer size (Viewport) callback. When window changes its size, change viewport framebuffer size in accordance
	glfwSetFramebufferSizeCallback(window, GLFWContextCallbackAdaptor::framebufferCallback);

	// Print context info
	printContextInfo();
}

void JFF::ContextGLFW::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: ContextGLFW")
}

JFF::Subsystem::UnloadOrder JFF::ContextGLFW::getUnloadOrder() const
{
	return UnloadOrder::CONTEXT;
}

JFF::ExecutableSubsystem::ExecutionOrder JFF::ContextGLFW::getExecutionOrder() const
{
	return ExecutableSubsystem::ExecutionOrder::CONTEXT;
}

bool JFF::ContextGLFW::execute()
{
	// Sawp buffers (Double buffer)
	glfwSwapBuffers(window);

	// Process input and windows events
	glfwPollEvents();

	return !glfwWindowShouldClose(window);
}

unsigned long long int JFF::ContextGLFW::addOnFramebufferSizeChangedListener(const std::function<void(int, int)>& listener)
{
	framebufferSizeCallbacks[framebufferCallbackIndex] = listener;
	return framebufferCallbackIndex++;
}

void JFF::ContextGLFW::removeOnFramebufferSizeChangedListener(unsigned long long int listenerHandler)
{
	auto numErased = framebufferSizeCallbacks.erase(listenerHandler);
	if (numErased == 0)
	{
		JFF_LOG_WARNING("Couldn't remove Context framebuffer size listener. Listener with given handler was not found. Aborted")
	}
}

std::string JFF::ContextGLFW::getClientGraphicsAPI() const
{
	if (!window)
	{
		JFF_LOG_ERROR("Error getting client graphics API. The window has to be created before calling this function")
			return std::string();
	}

	switch (glfwGetWindowAttrib(window, GLFW_CLIENT_API))
	{
	case GLFW_OPENGL_API:
		return "OpenGL";
	case GLFW_OPENGL_ES_API:
		return "OpenGL ES";
	case GLFW_NO_API:
	default:
		return "Unknown API";
	}
}

int JFF::ContextGLFW::getGraphicsAPIVersionMajor() const
{
	if (!window)
	{
		JFF_LOG_ERROR("Error getting graphics API version major. The window has to be created before calling this function")
		return -1;
	}

	return glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
}

int JFF::ContextGLFW::getGraphicsAPIVersionMinor() const
{
	if (!window)
	{
		JFF_LOG_ERROR("Error getting graphics API version minor. The window has to be created before calling this function")
			return -1;
	}

	return glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
}

int JFF::ContextGLFW::getGraphicsAPIRevisionNumber() const
{
	if (!window)
	{
		JFF_LOG_ERROR("Error getting graphics API revision number. The window has to be created before calling this function")
			return -1;
	}

	return glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
}

std::string JFF::ContextGLFW::getGraphicsAPIContexProfile() const
{
	switch (glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE))
	{
	case GLFW_OPENGL_CORE_PROFILE:
		return "core";
	case GLFW_OPENGL_COMPAT_PROFILE:
		return "compatibility";
	case GLFW_OPENGL_ANY_PROFILE:
		return "es";
	default:
		return "";
	}
}

void JFF::ContextGLFW::getWindowSizeInScreenCoordinates(int& outWidth, int& outHeight) const
{
	glfwGetWindowSize(window, &outWidth, &outHeight);
}

void JFF::ContextGLFW::getFramebufferSizeInPixels(int& outWidth, int& outHeight) const
{
	glfwGetFramebufferSize(window, &outWidth, &outHeight);
}

void JFF::ContextGLFW::printContextInfo() const
{
	if (!window)
	{
		JFF_LOG_ERROR("Error trying to print context info. The window has to be created before calling this function")
			return;
	}

	JFF_LOG_INFO("Using " << getClientGraphicsAPI() << " " << getGraphicsAPIVersionMajor() << "." << getGraphicsAPIVersionMinor() << "." << getGraphicsAPIRevisionNumber())

	switch (glfwGetWindowAttrib(window, GLFW_CONTEXT_CREATION_API))
	{
	case GLFW_NATIVE_CONTEXT_API:
		JFF_LOG_INFO("Contex creator API: Native context API")
		break;
	case GLFW_EGL_CONTEXT_API:
		JFF_LOG_INFO("Contex creator API: EGL context API")
		break;
	case GLFW_OSMESA_CONTEXT_API:
		JFF_LOG_INFO("Contex creator API: OSMESA context API")
		break;
	default:
		JFF_LOG_INFO("Contex creator API: Unknown context API")
		break;
	}

	JFF_LOG_INFO("OpenGL forward compatibility: " << (glfwGetWindowAttrib(window, GLFW_OPENGL_FORWARD_COMPAT) ? "true" : "false"))
	JFF_LOG_INFO("OpenGL " << getGraphicsAPIContexProfile() << " profile")
		
	switch (glfwGetWindowAttrib(window, GLFW_CONTEXT_RELEASE_BEHAVIOR))
	{
	case GLFW_ANY_RELEASE_BEHAVIOR:
		JFF_LOG_INFO("On context release behavior: Contex creator API dependant")
		break;
	case GLFW_RELEASE_BEHAVIOR_FLUSH:
		JFF_LOG_INFO("On context release behavior: flush pipeline")
		break;
	case GLFW_RELEASE_BEHAVIOR_NONE:
		JFF_LOG_INFO("On context release behavior: don't flush pipeline")
		break;
	default:
		JFF_LOG_INFO("On context release behavior: unknown behavior")
		break;
	}

	switch (glfwGetWindowAttrib(window, GLFW_CONTEXT_NO_ERROR))
	{
	case GLFW_TRUE:
		JFF_LOG_INFO("No errors enabled: If there is an error, the situation will cause undefined bahevior")
			break;
	case GLFW_FALSE:
		JFF_LOG_INFO("No errors disabled: If there is an error, the program will crash")
			break;
	default:
		JFF_LOG_INFO("No errors unknown")
		break;
	}

	switch (glfwGetWindowAttrib(window, GLFW_CONTEXT_ROBUSTNESS))
	{
	case GLFW_LOSE_CONTEXT_ON_RESET:
		JFF_LOG_INFO("Context robustness: Lose context on reset")
			break;
	case GLFW_NO_RESET_NOTIFICATION:
		JFF_LOG_INFO("Context robustness: No reset notification")
			break;
	case GLFW_NO_ROBUSTNESS:
	default:
		JFF_LOG_INFO("No context robustness")
			break;
	}
}

// ------------------------------- GLFW CONTEXT CALLBACK ADAPTOR ------------------------------- //

JFF::GLFWContextCallbackAdaptor::GLFWContextCallbackAdaptor(ContextGLFW* const context) : 
	context(context)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor GLFWContextCallbackAdaptor")
}

JFF::GLFWContextCallbackAdaptor::~GLFWContextCallbackAdaptor()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor GLFWContextCallbackAdaptor")
}

JFF::GLFWContextCallbackAdaptor& JFF::GLFWContextCallbackAdaptor::getInstance(ContextGLFW* const context)
{
	static GLFWContextCallbackAdaptor instance(context); // Internal static variable. This line will be executed the first time only
	return instance;
}

void JFF::GLFWContextCallbackAdaptor::framebufferCallback(GLFWwindow* window, int width, int height)
{
	auto& framebufferSizeCallbacks = getInstance().context->framebufferSizeCallbacks;
	std::for_each(framebufferSizeCallbacks.begin(), framebufferSizeCallbacks.end(), [width, height](const auto& pair)
		{
			pair.second(width, height);
		});
}

