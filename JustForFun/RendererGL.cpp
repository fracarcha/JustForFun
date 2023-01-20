/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "RendererGL.h"

#include "Log.h"
#include "Engine.h"
#include "FileSystemSetup.h"

#include "RenderPassBackground.h"
#include "RenderPassDebug.h"
#include "RenderPassPostProcess.h"
#include "RenderPassPostProcessPreLighting.h"
#include "RenderPassRenderToScreen.h"
#include "RenderPassShadowCast.h"
#include "RenderPassSurface.h"
#include "RenderPassTranslucent.h"
#include "RenderPassUI.h"

#include "RenderPassGeometryDeferred.h"
#include "RenderPassDirectionalLightingDeferred.h"
#include "RenderPassPointLightingDeferred.h"
#include "RenderPassSpotLightingDeferred.h"
#include "RenderPassEnvironmentLightingDeferred.h"
#include "RenderPassEmissiveLightingDeferred.h"

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

#include <stdexcept>
#include <algorithm>

extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(JFF::Framebuffer::PrefabFramebuffer fboType,
	unsigned int width, unsigned int height, unsigned int samplesPerPixel = 0);

extern std::shared_ptr<JFF::INIFile> createINIFile(const char* filepath);

JFF::RendererGL::RendererGL() : 
	engine(nullptr),
	activeRenderPath(RenderPath::FORWARD),
	renderables(),

	FBOs(),
	fbWidth(0),
	fbHeight(0),
	samplesPerPixel(0),

	framebufferCallbackHandler(0ull),

	maxPointLightsForwardShading(0),
	maxDirectionalLightsForwardShading(0),
	maxSpotLightsForwardShading(0),

	maxEnvironmentMapsForwardShading(1)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: RendererGL")
}

JFF::RendererGL::~RendererGL()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: RendererGL")

	// Unregister from Context's framebuffer change callback
	engine->context.lock()->removeOnFramebufferSizeChangedListener(framebufferCallbackHandler);

	// Destroy framebuffers
	std::for_each(FBOs.begin(), FBOs.end(), [](auto& fbo) { fbo->destroy(); });
}

void JFF::RendererGL::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: RendererGL")

	// TODO: Check this function

	// ------------------------------------ INI CONFIG FILE ------------------------------------ //

	// Load config file to set default behavior
	Params params = loadConfigFile();
	activeRenderPath = params.renderPath;
	maxPointLightsForwardShading = params.maxPointLightsForwardShading;
	maxDirectionalLightsForwardShading = params.maxDirectionalLightsForwardShading;
	maxSpotLightsForwardShading = params.maxSpotLightsForwardShading;

	JFF_LOG_INFO("Render path: " << (activeRenderPath == RenderPath::FORWARD ? "FORWARD" : "DEFERRED"))

	// ------------------------------------ INIT GLEW ------------------------------------ //

	GLenum glewerr = glewInit();
	if (glewerr != GLEW_OK)
	{
		std::basic_string<GLubyte> errorMsgStr(reinterpret_cast<const GLubyte*>("GLEW could not be initialized: Error trace: "));
		errorMsgStr += glewGetErrorString(glewerr);
		const char* errorMsg = reinterpret_cast<const char*>(errorMsgStr.c_str());

		JFF_LOG_ERROR(errorMsg)
		throw std::runtime_error(errorMsg);
	}

	// Check for GLEW extensions
	JFF_LOG_SUPER_IMPORTANT(glGetString(GL_VERSION))
	if (GLEW_ARB_debug_output)
	{
		auto callback = [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
			if (type == GL_DEBUG_TYPE_ERROR)
			{
				JFF_LOG_ERROR(message)
			}
			else
			{
				JFF_LOG_SUPER_IMPORTANT(message)
			}
		};
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallbackARB(callback, (void*)0);
	}

	// ------------------------------------ Enable MSAA if it's available ------------------------------------ //

	glGetIntegerv(GL_SAMPLES, &samplesPerPixel);
	if (samplesPerPixel)
	{
		JFF_LOG_INFO("Samples per pixel (MSAA): " << samplesPerPixel)
		glEnable(GL_MULTISAMPLE);
	}
	else
	{
		JFF_LOG_INFO("MSAA is disabled")
	}

	// Enable Gamma correction
	// NOTE: There are two ways to apply gamma correction: on fragment shaders or uncommenting next line. We chose the first one
	//glEnable(GL_FRAMEBUFFER_SRGB);

	/* 
	* Under the standard filtering rules for cubemaps, filtering does not work across faces of the cubemap.
	* This results in a seam across the faces of a cubemap. The next line solves that problem. This is useful 
	* for mipmapeed cubemaps like PBR Pre-filter Environment map
	*/
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Print some framebuffer info. Upon next calls, default framebuffer is pressumed bound in this moment
	GLint redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,		&redBits);
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,	&greenBits);
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,	&blueBits);
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_BACK_LEFT, GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,	&alphaBits);
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH,		GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,	&depthBits);
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL,	GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilBits);
	JFF_LOG_INFO("Default framebuffer attributes: R=" << redBits << " G=" << greenBits << " B=" << blueBits << " A=" << alphaBits << " Depth=" << depthBits << " Stencil=" << stencilBits)
}

void JFF::RendererGL::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: RendererGL")

	this->engine = engine;

	// ------------------------------------ DEFINE RENDER PASSES ------------------------------------ //

	// Create a render pass per material domain
	renderables[Material::MaterialDomain::SHADOW_CAST]		= std::make_shared<RenderPassShadowCast>(engine);
	switch (activeRenderPath)
	{
	case JFF::Renderer::RenderPath::FORWARD:
		renderables[Material::MaterialDomain::SURFACE]		= std::make_shared<RenderPassSurface>(engine);
		break;
	case JFF::Renderer::RenderPath::DEFERRED:
		renderables[Material::MaterialDomain::GEOMETRY_DEFERRED]			 = std::make_shared<RenderPassGeometryDeferred>(engine);
		renderables[Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED] = std::make_shared<RenderPassDirectionalLightingDeferred>(engine);
		renderables[Material::MaterialDomain::POINT_LIGHTING_DEFERRED]		 = std::make_shared<RenderPassPointLightingDeferred>(engine);
		renderables[Material::MaterialDomain::SPOT_LIGHTING_DEFERRED]		 = std::make_shared<RenderPassSpotLightingDeferred>(engine);
		renderables[Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED] = std::make_shared<RenderPassEnvironmentLightingDeferred>(engine);
		renderables[Material::MaterialDomain::EMISSIVE_LIGHTING_DEFERRED]	 = std::make_shared<RenderPassEmissiveLightingDeferred>(engine);

		// Surface domain is used in deferred shading exclusively for unlit objects
		renderables[Material::MaterialDomain::SURFACE]		= std::make_shared<RenderPassSurface>(engine);
		break;
	default:
		break;
	}
	renderables[Material::MaterialDomain::BACKGROUND]					= std::make_shared<RenderPassBackground>(engine);
	renderables[Material::MaterialDomain::TRANSLUCENT]					= std::make_shared<RenderPassTranslucent>(engine);
	renderables[Material::MaterialDomain::DEBUG]						= std::make_shared<RenderPassDebug>(engine);
	renderables[Material::MaterialDomain::POST_PROCESS_PRE_LIGHTING]	= std::make_shared<RenderPassPostProcessPreLighting>(engine);
	renderables[Material::MaterialDomain::POST_PROCESS]					= std::make_shared<RenderPassPostProcess>(engine);
	renderables[Material::MaterialDomain::UI]							= std::make_shared<RenderPassUI>(engine);
	renderables[Material::MaterialDomain::RENDER_TO_SCREEN]				= std::make_shared<RenderPassRenderToScreen>(engine);

	// ------------------------------------ DEFINE FRAMEBUFFERS ------------------------------------ //

	// Retrieve default framebuffer size from context
	this->engine->context.lock()->getFramebufferSizeInPixels(fbWidth, fbHeight);

	// Set OpenGL Viewport size, in pixels. This size will be used from clip to window space
	restoreViewport();

	// Configure pre-process FBO (FrameBuffer Object)
	switch (activeRenderPath)
	{
	case JFF::Renderer::RenderPath::FORWARD:
		// This will create a multisample or normal framebuffer depending on the number of samples per pixel (>=2 -> multisample)
		FBOs.push_back(createFramebuffer(Framebuffer::PrefabFramebuffer::FBO_PRE_PROCESS_FORWARD, fbWidth, fbHeight, samplesPerPixel));
		break;
	case JFF::Renderer::RenderPath::DEFERRED:
		// This will create a framebuffer which stores geometry data and another to calculate light contributions
		FBOs.push_back(createFramebuffer(Framebuffer::PrefabFramebuffer::FBO_GEOMETRY_DEFERRED, fbWidth, fbHeight, samplesPerPixel));
		FBOs.push_back(createFramebuffer(Framebuffer::PrefabFramebuffer::FBO_LIGHTING_DEFERRED, fbWidth, fbHeight, samplesPerPixel));
		break;
	default:
		break;
	}
	
	// Register framebuffer size changes and adapt Viewport and fbo to the new window size
	framebufferCallbackHandler = engine->context.lock()->addOnFramebufferSizeChangedListener([this](int width, int height)
		{
			if (width == 0 || height == 0) // Ignore request to invalid sizes
				return;

			fbWidth = width;
			fbHeight = height;

			restoreViewport();
			std::for_each(FBOs.begin(), FBOs.end(), [this](auto& fbo) { fbo->setSize(fbWidth, fbHeight); });
		});

	// --------------- CONFIGURE SOME ASPECTS OF OPENGL FIXED PIPELINE --------------- //

	// Color buffer config
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth buffer config
	restoreDepthTest();

	// Stencil buffer config
	// TODO: configure stencil functions
	//glEnable(GL_STENCIL_TEST); // TODO: Ensure GLFW creates the stencil buffer and check its depth (usually 8 bits depth)
	//glStencilMask(0xFF); // Fills stencil buffer with provided value on each pixel. This value will be ANDed with writes on stencil buffer
	//glStencilFunc(GL_EQUAL, 1, 0xFF); // Compares 1 with stencil buffer value and test passes if they are equal. In this example: Test = (1 & 0xFF) == (stencilBufferValue & 0xFF)
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Decides how to write on stencil buffer if stencil test fails, if stencil passes but depth test fails and if both test pasees, respectively. If replace is set, the stencil buffer value will be replaced by the 1 in stencilFunc()

	// Alpha blending config
	disableBlending();

	// Face culling
	restoreFaceCulling();

	// Polygon mode
	disableWireframeMode();
}

JFF::Subsystem::UnloadOrder JFF::RendererGL::getUnloadOrder() const
{
	return UnloadOrder::RENDERER;
}

JFF::ExecutableSubsystem::ExecutionOrder JFF::RendererGL::getExecutionOrder() const
{
	return ExecutableSubsystem::ExecutionOrder::RENDERER;
}

bool JFF::RendererGL::execute()
{
	switch (activeRenderPath)
	{
	case JFF::Renderer::RenderPath::FORWARD:
		executeForward();
		break;
	case JFF::Renderer::RenderPath::DEFERRED:
		executeDeferred();
		break;
	default:
		break;
	}

	return true;
}

void JFF::RendererGL::addRenderable(RenderComponent* const renderable)
{
	auto materialDomain = renderable->getMaterialDomain();
	auto materialLightingModel = renderable->getLightModel();

	if (activeRenderPath == Renderer::RenderPath::DEFERRED && 
		materialDomain == Material::MaterialDomain::SURFACE && 
		materialLightingModel != Material::LightModel::UNLIT)
	{
		renderables[Material::MaterialDomain::GEOMETRY_DEFERRED]->addRenderable(renderable);
	}
	else
	{
		renderables[materialDomain]->addRenderable(renderable);
	}

	// TODO: should ask if renderComponents receive shadows
	// Add opaque & translucent objects to shadow caster
	if (materialDomain == Material::MaterialDomain::SURFACE || materialDomain == Material::MaterialDomain::TRANSLUCENT)
		renderables[Material::MaterialDomain::SHADOW_CAST]->addRenderable(renderable);

	// Add a copy of post process renderables to render-to-screen renderables and post-process pre-lighting pass
	if (materialDomain == Material::MaterialDomain::POST_PROCESS)
	{
		renderables[Material::MaterialDomain::RENDER_TO_SCREEN]->addRenderable(renderable);
		renderables[Material::MaterialDomain::POST_PROCESS_PRE_LIGHTING]->addRenderable(renderable);
	}
}

void JFF::RendererGL::removeRenderable(RenderComponent* const renderable)
{
	auto materialDomain = renderable->getMaterialDomain();
	auto materialLightingModel = renderable->getLightModel();

	if (activeRenderPath == Renderer::RenderPath::DEFERRED && 
		materialDomain == Material::MaterialDomain::SURFACE && 
		materialLightingModel != Material::LightModel::UNLIT)
	{
		renderables[Material::MaterialDomain::GEOMETRY_DEFERRED]->removeRenderable(renderable);
	}
	else
	{
		renderables[materialDomain]->removeRenderable(renderable);
	}

	// TODO: should ask if renderComponents receive shadows
	// Remove opaque & translucent objects from shadow caster
	if (materialDomain == Material::MaterialDomain::SURFACE || materialDomain == Material::MaterialDomain::TRANSLUCENT)
		renderables[Material::MaterialDomain::SHADOW_CAST]->removeRenderable(renderable);

	// Remove the copy of post process renderables to render-to-screen renderables and post-process pre-lighting pass
	if (materialDomain == Material::MaterialDomain::POST_PROCESS)
	{
		renderables[Material::MaterialDomain::RENDER_TO_SCREEN]->removeRenderable(renderable);
		renderables[Material::MaterialDomain::POST_PROCESS_PRE_LIGHTING]->removeRenderable(renderable);
	}
}

void JFF::RendererGL::addLight(LightComponent* const light)
{
	switch (activeRenderPath)
	{
	case JFF::Renderer::RenderPath::FORWARD:
		renderables[Material::MaterialDomain::SURFACE]->addLight(light);
		renderables[Material::MaterialDomain::TRANSLUCENT]->addLight(light);
		break;
	case JFF::Renderer::RenderPath::DEFERRED:
		renderables[Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED]->addLight(light);
		renderables[Material::MaterialDomain::POINT_LIGHTING_DEFERRED]->addLight(light);
		renderables[Material::MaterialDomain::SPOT_LIGHTING_DEFERRED]->addLight(light);

		renderables[Material::MaterialDomain::TRANSLUCENT]->addLight(light);
		break;
	default:
		break;
	}

	if (light->castShadows())
		renderables[Material::MaterialDomain::SHADOW_CAST]->addLight(light);
}

void JFF::RendererGL::removeLight(LightComponent* const light)
{
	switch (activeRenderPath)
	{
	case JFF::Renderer::RenderPath::FORWARD:
		renderables[Material::MaterialDomain::SURFACE]->removeLight(light);
		renderables[Material::MaterialDomain::TRANSLUCENT]->removeLight(light);
		break;
	case JFF::Renderer::RenderPath::DEFERRED:
		renderables[Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED]->removeLight(light);
		renderables[Material::MaterialDomain::POINT_LIGHTING_DEFERRED]->removeLight(light);
		renderables[Material::MaterialDomain::SPOT_LIGHTING_DEFERRED]->removeLight(light);

		renderables[Material::MaterialDomain::TRANSLUCENT]->removeLight(light);
		break;
	default:
		break;
	}

	if (light->castShadows())
		renderables[Material::MaterialDomain::SHADOW_CAST]->removeLight(light);
}

void JFF::RendererGL::addEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	switch (activeRenderPath)
	{
	case JFF::Renderer::RenderPath::FORWARD:
		renderables[Material::MaterialDomain::SURFACE]->addEnvironmentMap(envMap);
		renderables[Material::MaterialDomain::TRANSLUCENT]->addEnvironmentMap(envMap);
		break;
	case JFF::Renderer::RenderPath::DEFERRED:
		renderables[Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED]->addEnvironmentMap(envMap);
		
		renderables[Material::MaterialDomain::TRANSLUCENT]->addEnvironmentMap(envMap);
		break;
	default:
		break;
	}
}

void JFF::RendererGL::removeEnvironmentMap(EnvironmentMapComponent* const envMap)
{
	switch (activeRenderPath)
	{
	case JFF::Renderer::RenderPath::FORWARD:
		renderables[Material::MaterialDomain::SURFACE]->removeEnvironmentMap(envMap);
		renderables[Material::MaterialDomain::TRANSLUCENT]->removeEnvironmentMap(envMap);
		break;
	case JFF::Renderer::RenderPath::DEFERRED:
		renderables[Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED]->removeEnvironmentMap(envMap);
		
		renderables[Material::MaterialDomain::TRANSLUCENT]->removeEnvironmentMap(envMap);
		break;
	default:
		break;
	}
}

int JFF::RendererGL::getForwardShadingMaxPointLights() const
{
	return maxPointLightsForwardShading;
}

int JFF::RendererGL::getForwardShadingMaxDirectionalLights() const
{
	return maxDirectionalLightsForwardShading;
}

int JFF::RendererGL::getForwardShadingMaxSpotLights() const
{
	return maxSpotLightsForwardShading;
}

int JFF::RendererGL::getForwardShadingMaxEnvironmentMaps() const
{
	return maxEnvironmentMapsForwardShading;
}

JFF::Renderer::RenderPath JFF::RendererGL::getRenderPath() const
{
	return activeRenderPath;
}

std::weak_ptr<JFF::Framebuffer> JFF::RendererGL::getFramebuffer() const
{
	return FBOs.back();
}

std::weak_ptr<JFF::Framebuffer> JFF::RendererGL::getGeometryFramebuffer() const
{
	return FBOs[0];
}

void JFF::RendererGL::setViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void JFF::RendererGL::restoreViewport()
{
	glViewport(0, 0, fbWidth, fbHeight);
}

void JFF::RendererGL::enableDepthTest()
{
	glEnable(GL_DEPTH_TEST);
}

void JFF::RendererGL::enableDepthTest(bool writeToDepthBuffer)
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(writeToDepthBuffer ? GL_TRUE : GL_FALSE);
}

void JFF::RendererGL::enableDepthTest(bool writeToDepthBuffer, DepthOp depthOp)
{
	enableDepthTest(writeToDepthBuffer);

	switch (depthOp)
	{
	case JFF::Renderer::DepthOp::NEVER_PASS:
		glDepthFunc(GL_NEVER);
		break;
	case JFF::Renderer::DepthOp::ALWAYS_PASS:
		glDepthFunc(GL_ALWAYS);
		break;
	case JFF::Renderer::DepthOp::PASS_IF_DEPTH_IS_LESS:
		glDepthFunc(GL_LESS);
		break;
	case JFF::Renderer::DepthOp::PASS_IF_DEPTH_IS_LESS_OR_EQUAL:
		glDepthFunc(GL_LEQUAL);
		break;
	case JFF::Renderer::DepthOp::PASS_IF_DEPTH_IS_GREATER:
		glDepthFunc(GL_GREATER);
		break;
	case JFF::Renderer::DepthOp::PASS_IF_DEPTH_IS_GREATER_OR_EQUAL:
		glDepthFunc(GL_GEQUAL);
		break;
	case JFF::Renderer::DepthOp::PASS_IF_DEPTH_IS_EQUAL:
		glDepthFunc(GL_EQUAL);
		break;
	case JFF::Renderer::DepthOp::PASS_IF_DEPTH_IS_NOT_EQUAL:
		glDepthFunc(GL_NOTEQUAL);
		break;
	default:
		break;
	}
}

void JFF::RendererGL::disableDepthTest()
{
	glDisable(GL_DEPTH_TEST);
}

void JFF::RendererGL::restoreDepthTest()
{
	glEnable(GL_DEPTH_TEST); // TODO: Ensure GLFW creates the depth buffer and check its color depth (1, 24 or 32 bit floats)
	glDepthMask(GL_TRUE); // If GL_FALSE, depth testing is done to the fragment, but this fragment won't draw into this buffer if it's not discarded
	glDepthFunc(GL_LESS); // Default depth function is GL_LESS
}

void JFF::RendererGL::enableBlending()
{
	glEnable(GL_BLEND);
}

void JFF::RendererGL::enableBlending(BlendOp op)
{
	glEnable(GL_BLEND);

	switch (op)
	{
	case JFF::Renderer::BlendOp::ALPHA_BLEND:
		// Selects how is the relationship between source and destination (color buffer). Example: Dst = Src * Src.a + Dst * (1.0 - Src.a)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case JFF::Renderer::BlendOp::ADDITIVE:
		glBlendFunc(GL_ONE, GL_ONE); // Dst = Src * 1 + Dst * 1
		break;
	case JFF::Renderer::BlendOp::MULTIPLY:
		glBlendFunc(GL_ZERO, GL_SRC_COLOR); // Dst = Src * 0 + Dst * Src
		break;
	default:
		break;
	}

	// TODO: Other interesting blend functions
	//glBlendFuncSeparate() // Same as glBlendFunc(), but separating rgb treatment from alpha
	//glBlendColor(r,g,b,a); // Select the constant color used in some enums used in glBlendFunc()
	//glBlendEquation(); // Select the operator (+, -, min, max, ...) used between source and destination values when blending. Example: Dst = Src * Src.a - Dst * (1.0 - Src.a)
}

void JFF::RendererGL::disableBlending()
{
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Default operation is ALPHA_BLEND
}

void JFF::RendererGL::faceCulling(FaceCullOp op)
{
	switch (op)
	{
	case JFF::Renderer::FaceCullOp::DISABLE:
		glDisable(GL_CULL_FACE);
		break;
	case JFF::Renderer::FaceCullOp::CULL_FRONT_FACES:
		glCullFace(GL_FRONT);
		break;
	case JFF::Renderer::FaceCullOp::CULL_BACK_FACES:
		glCullFace(GL_BACK);
		break;
	default:
		break;
	}
}

void JFF::RendererGL::restoreFaceCulling()
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW); // Define what is a front face. CCW for counter-clock wise. CW for clock wise
	glCullFace(GL_BACK); // Cull back face
}

void JFF::RendererGL::enableWireframeMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void JFF::RendererGL::disableWireframeMode()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

inline JFF::RendererGL::Params JFF::RendererGL::loadConfigFile() const
{
	std::string filePath = std::string("Config") + JFF_SLASH_STRING + "Engine.ini";
	auto INIFile = createINIFile(filePath.c_str());
	Params params;

	if (INIFile->has("renderer", "render-path"))
	{
		std::string option = INIFile->getString("renderer", "render-path");
		if (option == "DEFERRED")
		{
			params.renderPath = RenderPath::DEFERRED;
		}
		else // option == "FORWARD"
		{
			params.renderPath = RenderPath::FORWARD;
		}
	}
	else
	{
		params.renderPath = RenderPath::FORWARD;
	}

	params.maxDirectionalLightsForwardShading	= INIFile->has("renderer", "max-directional-lights") ? INIFile->getInt("renderer", "max-directional-lights") : 4;
	params.maxPointLightsForwardShading			= INIFile->has("renderer", "max-point-lights") ? INIFile->getInt("renderer", "max-point-lights") : 4;
	params.maxSpotLightsForwardShading			= INIFile->has("renderer", "max-spot-lights") ? INIFile->getInt("renderer", "max-spot-lights") : 4;

	return params;
}

inline void JFF::RendererGL::executeForward()
{
	// ----------------- SHADOW CAST RENDER PASS ----------------- //

	renderables[Material::MaterialDomain::SHADOW_CAST]->execute();

	// ----------------- PRE-PROCESS (GEOMETRY AND LIGHTS) RENDER PASSES ----------------- //

	auto forwardFBO = FBOs[0];
	forwardFBO->enable();
	restoreViewport(); // fbo viewport size (set every frame because shadow cast render pass change the viewport many times)

	renderables[Material::MaterialDomain::SURFACE]->execute();
	renderables[Material::MaterialDomain::BACKGROUND]->execute();
	renderables[Material::MaterialDomain::TRANSLUCENT]->execute();
	renderables[Material::MaterialDomain::DEBUG]->execute();

	forwardFBO->disable(); // In multisample FBOs, this call 'resolves' multisample textures onto normal (sampleable) textures

	// ----------------- POST-PROCESSING RENDER PASSES ----------------- //

	renderables[Material::MaterialDomain::POST_PROCESS_PRE_LIGHTING]->execute(); // In forward shading, lighting is calculated with objects, so this pass is done here
	renderables[Material::MaterialDomain::POST_PROCESS]->execute();
	renderables[Material::MaterialDomain::UI]->execute();

	// ----------------- RENDER TO DEFAULT FRAMEBUFFER ----------------- //

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderables[Material::MaterialDomain::RENDER_TO_SCREEN]->execute();
}

inline void JFF::RendererGL::executeDeferred()
{
	auto renderer = engine->renderer.lock();

	// ----------------- SHADOW CAST RENDER PASS ----------------- //

	renderables[Material::MaterialDomain::SHADOW_CAST]->execute();

 	// ----------------- GEOMETRY RENDER PASSES ----------------- //

	auto geometryFBO = FBOs[0];
	geometryFBO->enable();
	restoreViewport(); // fbo viewport size (set every frame because shadow cast render pass change the viewport many times)

	renderables[Material::MaterialDomain::GEOMETRY_DEFERRED]->execute();

	geometryFBO->disable();

	// ----------------- LIGHTING RENDER PASS ----------------- //

	auto lightingFBO = FBOs[1];
	lightingFBO->enable();

	renderer->disableDepthTest();
	renderer->enableBlending(Renderer::BlendOp::ADDITIVE);
	renderables[Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED]->execute();
	renderables[Material::MaterialDomain::POINT_LIGHTING_DEFERRED]->execute();
	renderables[Material::MaterialDomain::SPOT_LIGHTING_DEFERRED]->execute();
	renderables[Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED]->execute();
	renderables[Material::MaterialDomain::EMISSIVE_LIGHTING_DEFERRED]->execute();
	renderer->disableBlending();
	renderer->restoreDepthTest();

	// ----------------- POST-PROCESS PRE-LIGHTING PASS ---------------- //

	renderables[Material::MaterialDomain::POST_PROCESS_PRE_LIGHTING]->execute();

	// ----------------- BACKGROUND, TRANSLUCENT AND DEBUG RENDER PASSES (FORWARD SHADING) ----------------- //

	// Copy depth-stencil buffer from geometryFBO to lightingFBO to draw translucent object in correct places
	lightingFBO->copyBuffer(Framebuffer::AttachmentPoint::DEPTH_STENCIL, Framebuffer::AttachmentPoint::DEPTH_STENCIL, geometryFBO);
	
	// Re-bind lightingFBO (without clear buffers) because copyBuffer() change bindings
	lightingFBO->enable(/* clearBuffers */ false);

	// Render unlit, background, translucent and debug objects
	renderables[Material::MaterialDomain::SURFACE]->execute();		// SURFACE domain contain unlit objects only
	renderables[Material::MaterialDomain::BACKGROUND]->execute();
	renderables[Material::MaterialDomain::TRANSLUCENT]->execute();	// Translucent objects are incompatible with deferred shading
	renderables[Material::MaterialDomain::DEBUG]->execute();

	lightingFBO->disable();

	// ----------------- POST-PROCESSING RENDER PASSES ----------------- //

	renderables[Material::MaterialDomain::POST_PROCESS]->execute();
	renderables[Material::MaterialDomain::UI]->execute();

	// ----------------- RENDER TO DEFAULT FRAMEBUFFER ----------------- //

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderables[Material::MaterialDomain::RENDER_TO_SCREEN]->execute();
}
