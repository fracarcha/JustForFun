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

#include <memory>
#include <utility>

#ifdef _WIN64 // Targeting Windows x64. TODO: Should add more macros here?

	// --------------------------- CORE SUBSYSTEM SETUP ------------------------------------- //
#	pragma region CORE SUBSYSTEM SETUP

#		pragma region Context and Input
#			if defined(JFF_GL) && defined(JFF_GLFW) // Use OpenGL for rendering and GLFW for Context creation
#				include "ContextGLFW.h"
				auto createContextSubsystem() { return std::make_shared<JFF::ContextGLFW>(); }
#			elif JFF_SDL // Use SDL for Context creation
				// auto createContextSubsystem() {...}
				// TODO: Define other contexts
#			else
#				error No Context factory is configured
#			endif
#		pragma endregion

#		pragma region Input
#			if defined(JFF_GL) && defined(JFF_GLFW) 
#				include "InputGLFW.h"
				auto createInputSubsystem() { return std::make_shared<JFF::InputGLFW>(); }
#			else
#				error No Input factory is configured
#			endif
#		pragma endregion

#		pragma region Maths
#			if defined(JFF_GL) && defined(JFF_GLM)
#				include "MathGLM.h"
				auto createMathSubsystem() { return std::make_shared<JFF::MathGLM>(); }
#			else
#				error No API defined for math
#			endif
#		pragma endregion

#		pragma region Rendering
#			ifdef JFF_GL
#				include "RendererGL.h"
				auto createRendererSubsystem() { return std::make_shared<JFF::RendererGL>(); }
#			else
#				error No API defined for rendering
#			endif
#		pragma endregion

#		pragma region Physics
#			ifdef JFF_BULLET
#				include "PhysicsBullet.h"
				auto createPhysicsSubsystem() { return std::make_shared<JFF::PhysicsBullet>(); } // TODO: Dummy physics subsystem
#			else	
#				error No API defined for physics
#			endif
#		pragma endregion

#		pragma region Logic
#			ifdef JFF_LOGIC_STD // Standard logic
#				include "LogicSTD.h"
				auto createLogicSubsystem() { return std::make_shared<JFF::LogicSTD>(); }
#			else	
#				error No API defined for logic
#			endif
#		pragma endregion

#		pragma region Time
#			ifdef JFF_TIME_STD
#				include "TimeSTD.h"
				auto createTimeSubsystem() { return std::make_shared<JFF::TimeSTD>(); }
#			else
#				error No API defined for time management
#			endif
#		pragma endregion

#		pragma region IO
#			ifdef JFF_IO_STD
#				include "IOSTD.h"
				auto createIOSubsystem() { return std::make_shared<JFF::IOSTD>(); }
#			else
#				error No API defined for IO
#			endif // JFF_IO_STD
#		pragma endregion

#		pragma region Camera
#			ifdef JFF_CAMERA_STD
#				include "CameraSTD.h"
				auto createCameraSubsystem() { return std::make_shared<JFF::CameraSTD>(); }
#			else
#				error No API defined for Camera
#			endif // JFF_CAMERA_STD
#		pragma endregion

#		pragma region Cache
#			ifdef JFF_CACHE_STD
#				include "CacheSTD.h"
				auto createCacheSubsystem() { return std::make_shared<JFF::CacheSTD>(); }
#			else
#				error No API defined for Cache
#			endif
#		pragma endregion

#	pragma endregion

	// --------------------------- IO SETUP ------------------------------------- //

#	pragma region IO SETUP

		// Text files
#		ifdef JFF_FILE_STD
#			include "FileSTD.h"
			std::shared_ptr<JFF::File> createFile(const char* filepath) { return std::make_shared<JFF::FileSTD>(filepath); }
#		else
#			error No API defined for File
#		endif // JFF_FILE_STD

		// INI files
#		ifdef JFF_INI_FILE_mINI
#			include "INIFileMINI.h"
			std::shared_ptr<JFF::INIFile> createINIFile(const char* filepath) { return std::make_shared<JFF::INIFileMINI>(filepath); }
#		else
#			error No API defined for INI File
#		endif // JFF_INI_FILE_mINI

		// Image files
#		ifdef JFF_STB_IMAGE
#			include "ImageSTBI.h"
			std::shared_ptr<JFF::Image> createImage(JFF::Engine* const engine, const char* filepath, 
				bool flipVertically = true, bool HDRImage = false, bool bgra = false)
			{
				std::shared_ptr<JFF::Image> outImg;
				auto cache = engine->cache.lock();

				std::string cacheName = JFF::Image::generateCacheName(filepath);
				std::shared_ptr<JFF::Cacheable> cacheableImage = cache->getCachedItem(cacheName);
				if (cacheableImage)
				{
					outImg = std::dynamic_pointer_cast<JFF::Image>(cacheableImage);
				}
				else
				{
					outImg = std::make_shared<JFF::ImageSTBI>(filepath, flipVertically, HDRImage, bgra);
					cache->addCacheItem(outImg);
				}

				return outImg;
			}
			std::shared_ptr<JFF::Image> createImage(JFF::Engine* const engine, const char* filepath,
				const unsigned char* imgBuffer, int bufferSizeBytes, 
				bool flipVertically = true, bool HDRImage = false, bool bgra = false)
			{
				std::shared_ptr<JFF::Image> outImg;
				auto cache = engine->cache.lock();

				std::string cacheName = JFF::Image::generateCacheName(filepath);
				std::shared_ptr<JFF::Cacheable> cacheableImage = cache->getCachedItem(cacheName);
				if (cacheableImage)
				{
					outImg = std::dynamic_pointer_cast<JFF::Image>(cacheableImage);
				}
				else
				{
					outImg = std::make_shared<JFF::ImageSTBI>(filepath, imgBuffer, bufferSizeBytes, flipVertically, HDRImage, bgra);
					cache->addCacheItem(outImg);
				}

				return outImg;
			}
#		else
#			error No API defined for image reading
#		endif // JFF_STB_IMAGE

		// Raw image files
#		ifdef JFF_RAW_IMAGE_STD
#			include "ImageRawSTD.h"
			std::shared_ptr<JFF::Image> createImage(JFF::Engine* const engine, const char* filepath,
				int width, int height, int numChannels, const std::vector<float>& rawData, bool bgra = false)
			{
				std::shared_ptr<JFF::Image> outImg;
				auto cache = engine->cache.lock();

				std::string cacheName = JFF::Image::generateCacheName(filepath);
				std::shared_ptr<JFF::Cacheable> cacheableImage = cache->getCachedItem(cacheName);
				if (cacheableImage)
				{
					outImg = std::dynamic_pointer_cast<JFF::Image>(cacheableImage);
				}
				else
				{
					outImg = std::make_shared<JFF::ImageRawSTD>(filepath, width, height, numChannels, rawData, bgra);
					cache->addCacheItem(outImg);
				}

				return outImg;
			}
			std::shared_ptr<JFF::Image> createImage(JFF::Engine* const engine, const char* filepath,
				int width, int height, int numChannels, const std::vector<unsigned char>& rawData, bool bgra = false)
			{
				std::shared_ptr<JFF::Image> outImg;
				auto cache = engine->cache.lock();

				std::string cacheName = JFF::Image::generateCacheName(filepath);
				std::shared_ptr<JFF::Cacheable> cacheableImage = cache->getCachedItem(cacheName);
				if (cacheableImage)
				{
					outImg = std::dynamic_pointer_cast<JFF::Image>(cacheableImage);
				}
				else
				{
					outImg = std::make_shared<JFF::ImageRawSTD>(filepath, width, height, numChannels, rawData, bgra);
					cache->addCacheItem(outImg);
				}

				return outImg;
			}
#		else
#			error No API defined for raw image creation
#		endif

		// Model files
#		ifdef JFF_MODEL_STD
#			include "ModelAssimp.h"
			std::shared_ptr<JFF::Model> createModel(const char* assetFilepath, JFF::Engine* const engine)
			{ 
				return std::make_shared<JFF::ModelAssimp>(assetFilepath, engine);
			}
			std::shared_ptr<JFF::Model> createModel(const char* assetFilepath, JFF::Engine* const engine, 
				const std::weak_ptr<JFF::GameObject>& parentGameObject)
			{
				return std::make_shared<JFF::ModelAssimp>(assetFilepath, engine, parentGameObject);
			}
#		else
#			error No API defined for model
#		endif // JFF_MODEL_STD

#	pragma endregion

	// --------------------------- RENDERER SETUP ------------------------------------- //

#	pragma region RENDERER SETUP 
#		ifdef JFF_GL
#			include "MeshObjectGL.h"
			std::shared_ptr<JFF::MeshObject> createMeshObject(JFF::Engine* const engine, const std::shared_ptr<JFF::Mesh>& mesh)
			{
				return std::make_shared<JFF::MeshObjectGL>(engine, mesh);
			}
			std::shared_ptr<JFF::MeshObject> createMeshObject(JFF::Engine* const engine, const JFF::MeshObject::BasicMesh& predefinedShape)
			{
				return std::make_shared<JFF::MeshObjectGL>(engine, predefinedShape);
			}

#			include "MaterialGL.h"
			std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name, const char* assetFilePath)
			{
				return std::make_shared<JFF::MaterialGL>(engine, name, assetFilePath);
			}
			std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name)
			{
				return std::make_shared<JFF::MaterialGL>(engine, name);
			}

#			ifdef JFF_STB_IMAGE
#				include "TextureGLSTBI.h"
				std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const char* name, const char* assetFilePath)
				{
					std::shared_ptr<JFF::Texture> outTex;
					auto cache = engine->cache.lock();

					std::string cacheName = JFF::Texture::generateCacheName(assetFilePath);
					std::shared_ptr<JFF::Cacheable> cacheableTexture = cache->getCachedItem(cacheName);
					if (cacheableTexture)
					{
						outTex = std::dynamic_pointer_cast<JFF::Texture>(cacheableTexture);
					}
					else
					{
						outTex = std::make_shared<JFF::TextureGLSTBI>(engine, name, assetFilePath);
						cache->addCacheItem(outTex);
					}

					return outTex;
				}
				std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const JFF::Texture::Params& params)
				{
					std::shared_ptr<JFF::Texture> outTex;
					auto cache = engine->cache.lock();

					std::string cacheName = JFF::Texture::generateCacheName(params.img->data().filepath.c_str(), 
						params.coordsWrapMode, params.filterMode, params.numColorChannels, params.specialFormat);
					std::shared_ptr<JFF::Cacheable> cacheableTexture = cache->getCachedItem(cacheName);
					if (cacheableTexture)
					{
						outTex = std::dynamic_pointer_cast<JFF::Texture>(cacheableTexture);
					}
					else
					{
						outTex = std::make_shared<JFF::TextureGLSTBI>(engine, params);
						cache->addCacheItem(outTex);
					}

					return outTex;
				}

#				include "CubemapGLSTBI.h"
				std::shared_ptr<JFF::Cubemap> createCubemap(JFF::Engine* const engine, const char* name, const char* assetFilePath)
				{
					std::shared_ptr<JFF::Cubemap> outCubemap;
					auto cache = engine->cache.lock();

					std::string cacheName = JFF::Cubemap::generateCacheName(assetFilePath);
					std::shared_ptr<JFF::Cacheable> cacheableCubemap = cache->getCachedItem(cacheName);
					if (cacheableCubemap)
					{
						outCubemap = std::dynamic_pointer_cast<JFF::Cubemap>(cacheableCubemap);
					}
					else 
					{
						outCubemap = std::make_shared<JFF::CubemapGLSTBI>(engine, name, assetFilePath);
						cache->addCacheItem(outCubemap);
					}

					return outCubemap;
				}
				std::shared_ptr<JFF::Cubemap> createCubemap(JFF::Engine* const engine, const JFF::Cubemap::Params& params)
				{
					std::shared_ptr<JFF::Cubemap> outCubemap;
					auto cache = engine->cache.lock();

					std::string imgRightPath	= params.imgRight->data().filepath;
					std::string imgLeftPath		= params.imgLeft->data().filepath;
					std::string imgTopPath		= params.imgTop->data().filepath;
					std::string imgBottomPath	= params.imgBottom->data().filepath;
					std::string imgBackPath		= params.imgBack->data().filepath;
					std::string imgFrontPath	= params.imgFront->data().filepath;

					std::string cacheName = JFF::Cubemap::generateCacheName(
						imgRightPath.c_str(), imgLeftPath.c_str(), imgTopPath.c_str(),
						imgBottomPath.c_str(), imgBackPath.c_str(), imgFrontPath.c_str(),
						params.coordsWrapMode, params.filterMode, params.numColorChannels, params.specialFormat, params.numMipmapsGenerated);
					std::shared_ptr<JFF::Cacheable> cacheableCubemap = cache->getCachedItem(cacheName);
					if (cacheableCubemap)
					{
						outCubemap = std::dynamic_pointer_cast<JFF::Cubemap>(cacheableCubemap);
					}
					else
					{
						outCubemap = std::make_shared<JFF::CubemapGLSTBI>(engine, params);
						cache->addCacheItem(outCubemap);
					}

					return outCubemap;
				}

#				include "FramebufferGLSTBI.h"
				std::shared_ptr<JFF::Framebuffer> createFramebuffer(JFF::Framebuffer::PrefabFramebuffer fboType,
					unsigned int width, unsigned int height, unsigned int samplesPerPixel = 0)
				{
					return std::make_shared<JFF::FramebufferGLSTBI>(fboType, width, height, samplesPerPixel);
				}
				std::shared_ptr<JFF::Framebuffer> createFramebuffer(const JFF::Framebuffer::Params& params)
				{
					return std::make_shared<JFF::FramebufferGLSTBI>(params);
				}
#			else
#				error No API defined for textures, cubemaps and framebuffers
#			endif // JFF_STB_IMAGE

#			include "ShaderCodeBuilderShadowCastGL.h"
#			include "ShaderCodeBuilderOmnidirectionalShadowCastGL.h"
#			include "ShaderCodeBuilderDebugGL.h"
#			include "ShaderCodeBuilderPostProcessGL.h"
#			include "ShaderCodeBuilderRenderToScreenGL.h"
#			include "ShaderCodeBuilderSSAOGL.h"
#			include "ShaderCodeBuilderGouraudGL.h"
#			include "ShaderCodeBuilderPBRGL.h"
#			include "ShaderCodeBuilderPhongGL.h"
#			include "ShaderCodeBuilderBlinnPhongGL.h"
#			include "ShaderCodeBuilderUnlitGL.h"
#			include "ShaderCodeBuilderGeometryDeferredBlinnPhongGL.h"
#			include "ShaderCodeBuilderDirectionalLightingDeferredBlinnPhongGL.h"
#			include "ShaderCodeBuilderPointLightingDeferredBlinnPhongGL.h"
#			include "ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL.h"
#			include "ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL.h"
#			include "ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL.h"
#			include "ShaderCodeBuilderBackgroundGL.h"
#			include "ShaderCodeBuilderUIGL.h"
#			include "ShaderCodeBuilderGaussianBlurHorizontalGL.h"
#			include "ShaderCodeBuilderGaussianBlurVerticalGL.h"
#			include "ShaderCodeBuilderHighPassFilterGL.h"
#			include "ShaderCodeBuilderColorAdditionGL.h"
#			include "ShaderCodeBuilderColorCopyGL.h"
#			include "ShaderCodeBuilderEquirectangularToCubemapGL.h"
#			include "ShaderCodeBuilderIrradianceGeneratorGL.h"
#			include "ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL.h"
#			include "ShaderCodeBuilderBRDFIntegrationMapGeneratorGL.h"
			std::shared_ptr<JFF::ShaderCodeBuilder> createShaderCodeBuilder(
				JFF::Renderer::RenderPath renderPath,
				JFF::Material::MaterialDomain domain, 
				JFF::Material::LightModel lightModel = JFF::Material::LightModel::GOURAUD) 
			{
				switch (domain)
				{
				case JFF::Material::MaterialDomain::SHADOW_CAST:
					return std::make_shared<JFF::ShaderCodeBuilderShadowCastGL>();
				case JFF::Material::MaterialDomain::OMNIDIRECTIONAL_SHADOW_CAST:
					return std::make_shared<JFF::ShaderCodeBuilderOmnidirectionalShadowCastGL>();
				case JFF::Material::MaterialDomain::SURFACE:
					switch (renderPath)
					{
					case JFF::Renderer::RenderPath::FORWARD:
						switch (lightModel)
						{
						case JFF::Material::LightModel::GOURAUD:
							return std::make_shared<JFF::ShaderCodeBuilderGouraudGL>();
						case JFF::Material::LightModel::PHONG:
							return std::make_shared<JFF::ShaderCodeBuilderPhongGL>();
						case JFF::Material::LightModel::BLINN_PHONG:
							return std::make_shared<JFF::ShaderCodeBuilderBlinnPhongGL>();
						case JFF::Material::LightModel::PBR:
							return std::make_shared<JFF::ShaderCodeBuilderPBRGL>();
						case JFF::Material::LightModel::UNLIT:
							return std::make_shared<JFF::ShaderCodeBuilderUnlitGL>();
						default:
							JFF_LOG_ERROR("Can't find a valid shader code builder")
							break;
						}
						break;
					case JFF::Renderer::RenderPath::DEFERRED:
						switch (lightModel)
						{
						case JFF::Material::LightModel::GOURAUD:
						case JFF::Material::LightModel::PHONG:
						case JFF::Material::LightModel::BLINN_PHONG:
							return std::make_shared<JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong>();
						case JFF::Material::LightModel::PBR:
							JFF_LOG_WARNING("Shader code builder not yet implemented")
							break;
						case JFF::Material::LightModel::UNLIT:
							return std::make_shared<JFF::ShaderCodeBuilderUnlitGL>();
						default:
							JFF_LOG_ERROR("Can't find a valid shader code builder")
							break;
						}
					default:
						JFF_LOG_ERROR("Can't find a valid shader code builder")
						break;
					}
					break;
				case JFF::Material::MaterialDomain::TRANSLUCENT:
					switch (lightModel)
					{
					case JFF::Material::LightModel::GOURAUD:
						return std::make_shared<JFF::ShaderCodeBuilderGouraudGL>();
					case JFF::Material::LightModel::PHONG:
						return std::make_shared<JFF::ShaderCodeBuilderPhongGL>();
					case JFF::Material::LightModel::BLINN_PHONG:
						return std::make_shared<JFF::ShaderCodeBuilderBlinnPhongGL>();
					case JFF::Material::LightModel::PBR:
						return std::make_shared<JFF::ShaderCodeBuilderPBRGL>();
					case JFF::Material::LightModel::UNLIT:
						return std::make_shared<JFF::ShaderCodeBuilderUnlitGL>();
					default:
						JFF_LOG_ERROR("Can't find a valid shader code builder")
						break;
					}
					break;
				case JFF::Material::MaterialDomain::GEOMETRY_DEFERRED:
					return std::make_shared<JFF::ShaderCodeBuilderGeometryDeferredBlinnPhong>();
				case JFF::Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED:
					return std::make_shared<JFF::ShaderCodeBuilderDirectionalLightingDeferredBlinnPhongGL>();
				case JFF::Material::MaterialDomain::POINT_LIGHTING_DEFERRED:
					return std::make_shared<JFF::ShaderCodeBuilderPointLightingDeferredBlinnPhongGL>();
				case JFF::Material::MaterialDomain::SPOT_LIGHTING_DEFERRED:
					return std::make_shared<JFF::ShaderCodeBuilderSpotLightingDeferredBlinnPhongGL>();
				case JFF::Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED:
					return std::make_shared<JFF::ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL>();
				case JFF::Material::MaterialDomain::EMISSIVE_LIGHTING_DEFERRED:
					return std::make_shared<JFF::ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL>();
				case JFF::Material::MaterialDomain::BACKGROUND:
					return std::make_shared<JFF::ShaderCodeBuilderBackgroundGL>();
				case JFF::Material::MaterialDomain::DEBUG:
					return std::make_shared<JFF::ShaderCodeBuilderDebugGL>();
				case JFF::Material::MaterialDomain::POST_PROCESS:
					return std::make_shared<JFF::ShaderCodeBuilderPostProcessGL>();
				case JFF::Material::MaterialDomain::POST_PROCESS_PRE_LIGHTING:
					JFF_LOG_WARNING("Cannot create a material of type POST_PROCESS_PRE_LIGHTING. This domain is used as render pass only")
					return std::shared_ptr<JFF::ShaderCodeBuilder>();
				case JFF::Material::MaterialDomain::UI:
					return std::make_shared<JFF::ShaderCodeBuilderUIGL>();
				case JFF::Material::MaterialDomain::RENDER_TO_SCREEN:
					return std::make_shared<JFF::ShaderCodeBuilderRenderToScreenGL>();
				case JFF::Material::MaterialDomain::SSAO:
					return std::make_shared<JFF::ShaderCodeBuilderSSAOGL>();
				case JFF::Material::MaterialDomain::GAUSSIAN_BLUR_HORIZONTAL:
					return std::make_shared<JFF::ShaderCodeBuilderGaussianBlurHorizontalGL>();
				case JFF::Material::MaterialDomain::GAUSSIAN_BLUR_VERTICAL:
					return std::make_shared<JFF::ShaderCodeBuilderGaussianBlurVerticalGL>();
				case JFF::Material::MaterialDomain::HIGH_PASS_FILTER:
					return std::make_shared<JFF::ShaderCodeBuilderHighPassFilterGL>();
				case JFF::Material::MaterialDomain::COLOR_ADDITION:
					return std::make_shared<JFF::ShaderCodeBuilderColorAdditionGL>();
				case JFF::Material::MaterialDomain::COLOR_COPY:
					return std::make_shared<JFF::ShaderCodeBuilderColorCopyGL>();
				case JFF::Material::MaterialDomain::EQUIRECTANGULAR_TO_CUBEMAP:
					return std::make_shared<JFF::ShaderCodeBuilderEquirectangularToCubemapGL>();
				case JFF::Material::MaterialDomain::IRRADIANCE_GENERATOR:
					return std::make_shared<JFF::ShaderCodeBuilderIrradianceGeneratorGL>();
				case JFF::Material::MaterialDomain::PRE_FILTERED_ENVIRONMENT_MAP_GENERATOR:
					return std::make_shared<JFF::ShaderCodeBuilderPreFilteredEnvironmentMapGeneratorGL>();
				case JFF::Material::MaterialDomain::BRDF_INTEGRATION_MAP_GENERATOR:
					return std::make_shared<JFF::ShaderCodeBuilderBRDFIntegrationMapGeneratorGL>();
				default:
					JFF_LOG_ERROR("Can't find a valid shader code builder")
					break;
				}

				return std::shared_ptr<JFF::ShaderCodeBuilder>();
			}

#			include "MaterialFunctionCodeBuilderGL.h"
			std::shared_ptr<JFF::MaterialFunctionCodeBuilder> createMaterialFunctionCodeBuilder()
			{
				return std::make_shared<JFF::MaterialFunctionCodeBuilderGL>();
			}

#		else
#			error No API defined for rendering
#		endif
#	pragma endregion

	// --------------------------- LOGIC SETUP ------------------------------------- //

#	pragma region LOGIC SETUP
#		ifdef JFF_GL
#			include "CameraComponentGL.h"
#			include "GameObject.h"

			std::shared_ptr<JFF::CameraComponent> createCameraComponent(
				JFF::GameObject* const gameObject,
				const char* name,
				bool initiallyEnabled,
				bool activeCameraOnStart = false)
			{
				return std::make_shared<JFF::CameraComponentGL>(gameObject, name, initiallyEnabled, activeCameraOnStart);
			}
#		else
#			error No API defined for rendering
#		endif
#	pragma endregion

	// --------------------------- INPUT SETUP ------------------------------------- //

#	pragma region INPUT SETUP
#		if defined(JFF_GL) && defined(JFF_GLFW)

#			include "InputBindingButtonGLFW.h"
#			include "InputBindingTriggerGLFW.h"
#			include "InputBindingAxesGLFW.h"

			std::shared_ptr<JFF::InputBindingButton> createInputBindingButton(const std::string& name, JFF::Engine* const engine,
				JFF::InputActionButton* const parentAction, JFF::Mapping inputMapping)
			{
				return std::make_shared<JFF::InputBindingButtonGLFW>(name, engine, parentAction, inputMapping);
			}
			std::shared_ptr<JFF::InputBindingTrigger> createInputBindingTrigger(const std::string& name, JFF::Engine* const engine,
				JFF::InputActionTrigger* const parentAction, JFF::Mapping inputMapping)
			{
				return std::make_shared<JFF::InputBindingTriggerGLFW>(name, engine, parentAction, inputMapping);
			}
			std::shared_ptr<JFF::InputBindingAxes> createInputBindingAxes(const std::string& name, JFF::Engine* const engine,
				JFF::InputActionAxes* const parentAction, JFF::Mapping inputMapping)
			{
				return std::make_shared<JFF::InputBindingAxesGLFW>(name, engine, parentAction, inputMapping);
			}

#		else
#			error No API defined for input
#		endif 
#	pragma endregion


#elif defined(_WIN32)
#	error This application isn't targeted for Windows x86 architecture
#else
#	error This application isn't targeted for this platform
#endif // _WIN64