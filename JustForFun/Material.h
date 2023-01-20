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

#include "Mat.h"
#include <string>
#include <memory>

namespace JFF
{
	class Texture;
	class Cubemap;
	class Framebuffer;

	class Material
	{
	public:
		enum class MaterialDomain : char 
		{
			// Shadow cast materials
			SHADOW_CAST = 0,
			OMNIDIRECTIONAL_SHADOW_CAST,

			// Pre-processing
			BACKGROUND, 
			TRANSLUCENT,
			DEBUG,

			// Pre-process (Forward shading only)
			SURFACE,

			// Pre-process (Deferred shading only)
			GEOMETRY_DEFERRED,
			DIRECTIONAL_LIGHTING_DEFERRED,
			POINT_LIGHTING_DEFERRED,
			SPOT_LIGHTING_DEFERRED,
			ENVIRONMENT_LIGHTING_DEFERRED,
			EMISSIVE_LIGHTING_DEFERRED,

			// Post-process
			POST_PROCESS,
			POST_PROCESS_PRE_LIGHTING,
			UI,
			RENDER_TO_SCREEN, // Reserved for the last rendering pass

			// Post-process FX
			SSAO,

			// Helper shader domain
			GAUSSIAN_BLUR_HORIZONTAL,
			GAUSSIAN_BLUR_VERTICAL,
			HIGH_PASS_FILTER,
			COLOR_ADDITION,
			COLOR_COPY,
			EQUIRECTANGULAR_TO_CUBEMAP,
			IRRADIANCE_GENERATOR,
			PRE_FILTERED_ENVIRONMENT_MAP_GENERATOR,
			BRDF_INTEGRATION_MAP_GENERATOR,
		};

		enum class LightModel : char
		{
			GOURAUD, // Default and simplest shading
			PHONG,
			BLINN_PHONG,
			PBR,
			UNLIT,
		};

		enum class Side : char
		{
			FRONT, // Culls back faces
			BACK,
			TWO_SIDED,
		};

		enum class DebugDisplay : char
		{
			NO_DISPLAY,
			NORMALS,
			POLYGONS,
		};

		enum class PBRWorkflow : char
		{
			METALLIC,
			SPECULAR,
		};

		struct PostProcessParams
		{
			bool bloomEnabled;
			float bloomThreshold;
			float bloomIntensity;

			bool SSAOEnabled;
			float SSAOIntensity;
			unsigned int SSAONumSamples;
			float SSAOSampleHemisphereRadius;
			unsigned int SSAONumBlurSteps;

			// TODO: more post process params here
		};

		Material() {}
		virtual ~Material() {}

		// Copy ctor and copy assignment
		Material(const Material& other) = delete;
		Material& operator=(const Material& other) = delete;

		// Move ctor and assignment
		Material(Material&& other) = delete;
		Material operator=(Material&& other) = delete;

		// --------------------------- MATERIAL INTERFACE --------------------------- //
		
		// --------------- PRE-COOK FUNCTIONS --------------- //

		// Sets the name of this material. Name cannot be changed after this material is cooked
		virtual void setName(const char* name) = 0;

		// Gets the name of the material
		virtual std::string getName() const = 0;

		// Sets the material domain. Domain cannot be changed after this material is cooked
		virtual void setDomain(MaterialDomain domain) = 0;

		// Gets the material domain, which translates to the render pass that uses this material
		virtual MaterialDomain getDomain() const = 0;

		// Sets the material's light model. Light model cannot be changed after this material is cooked
		virtual void setLightModel(LightModel lightModel) = 0;

		// Gets material's light model
		virtual LightModel getLightModel() const = 0;

		// If light model is PBR, this function selects which PBR workflow will be used
		virtual void setPBRWorkflow(PBRWorkflow pbrWorkflow) = 0;

		// Gets material's PBR workflow
		virtual PBRWorkflow getPBRWorkflow() const = 0;

		/* Sets which faces of the model will be discarded and which will be drawn, regarding the winding order of the polygons.
		* Side cannot be changed after this material is cooked
		*/ 
		virtual void setSide(Side side) = 0;

		// Gets material's drawable side
		virtual Side getSide() const = 0;

		// Sets if this material will use normal maps for normal calculation or will use primitive (polygon) normals
		virtual void setUseNormalMap(bool useNormalMap) = 0;

		// Gets if this material will use normal maps for normal calculation or will use primitive (polygon) normals
		virtual bool getUseNormalMap() const = 0;

		// Gets debug display option if applicable
		virtual DebugDisplay getDebugDisplay() const = 0;

		// Gets post processing params
		virtual PostProcessParams getPostProcessParams() const = 0;

		/**
		* Adds an external pre-loaded texture to the shader.
		* Cannot add a new texture after this material is cooked
		*/ 
		virtual void addTexture(const std::shared_ptr<Texture>& texture) = 0;

		// TODO: Add cubemap ??

		/**
		* Compile and link shaders generated from provided info and make this material operative for rendering.
		* Optionally, external custom code can be added.
		* A Material can only be cooked once.
		*/
		virtual void cook(const std::string& externalCustomCode = "") = 0;

		// --------------- POST-COOK FUNCTIONS --------------- //

		// Enables the internal shader and its associated textures. Material must be cooked for this function to work
		virtual void use() = 0;

		/*
		* Send a 4x4 matrix to internal active shader and attachs it to the variable name.
		* The variable name must be a valid uniform included on internal shader code and 
		* material must be currently active with use()
		*/
		virtual void sendMat4(const char* variableName, const Mat4& matrix) = 0;

		/*
		* Send a 3x3 matrix to internal active shader and attachs it to the variable name.
		* The variable name must be a valid uniform included on internal shader code and
		* material must be currently active with use()
		*/
		virtual void sendMat3(const char* variableName, const Mat3& matrix) = 0;

		/*
		* Send a vec2 to active material and attach it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec2(const char* variableName, const Vec2& vec) = 0;

		/*
		* Send a vec3 to active material and attach it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec3(const char* variableName, const Vec3& vec) = 0;

		/*
		* Send a vec4 to active material and attach it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec4(const char* variableName, const Vec4& vec) = 0;

		/*
		* Send a float to active material and attach it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendFloat(const char* variableName, float f) = 0;

		/*
		* Send an int to active material and attach it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendInt(const char* variableName, int i) = 0;

		/*
		* Send an environment map to active material. 
		* If envMap is nullptr, send an empty environment map to material. This is important if no environment map is present,
		* because the cubemap sampler in shader must be specified.
		* There are other maps that can be sent as part of environment map, like irradiance map, used for PBR rendering.
		*/
		virtual void sendEnvironmentMap(const std::shared_ptr<Cubemap>& envMap = nullptr,
			const std::shared_ptr<Cubemap>& irradianceMap = nullptr,
			const std::shared_ptr<Cubemap>& preFilteredMap = nullptr,
			const std::shared_ptr<Texture>& BRDFIntegrationMap = nullptr) = 0;

		/* 
		* Send a directional light's shadowmap texture to this material located at given index. 
		* Index must be in range [0, Renderer::getMaxDirectionalLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important 
		* because all texture samplers in shader must be specified
		*/
		virtual void sendDirLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer> shadowMapFBO = std::weak_ptr<Framebuffer>()) = 0;

		/*
		* Send a point light's shadowmap cubemap to this material located at given index.
		* Index must be in range [0, Renderer::getMaxPointLights())
		* If @shadowCubemapFBO is invalid, this function will send the default bind cubemap. This is important
		* because all cubemap samplers in shader must be specified
		*/
		virtual void sendPointLightShadowCubemap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowCubemapFBO = std::weak_ptr<Framebuffer>()) = 0;

		/*
		* Send a spot light's shadowmap texture to this material located at given index.
		* Index must be in range [0, Renderer::getMaxSpotLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important
		* because all texture samplers in shader must be specified
		*/
		virtual void sendSpotLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer> shadowMapFBO = std::weak_ptr<Framebuffer>()) = 0;

		/*
		* Send all post-processing textures to this material
		*/
		virtual void sendPostProcessingTextures(const std::weak_ptr<Framebuffer>& ppFBO, const std::weak_ptr<Framebuffer>& ppFBO2 = std::weak_ptr<Framebuffer>()) = 0;

		// Releases GPU data and unbind internal shaders
		virtual void destroy() = 0;
	};
}