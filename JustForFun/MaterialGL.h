#pragma once

#include "Material.h"

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

#include "Texture.h"
#include "Cubemap.h"
#include "Framebuffer.h"

#include <vector>
#include <tuple>
#include <memory>
#include <sstream>

namespace JFF
{
	class Engine;
	class INIFile;

	class MaterialGL : public Material
	{
	public:
		// Ctor & Dtor
		explicit MaterialGL(JFF::Engine* const engine, const char* name, const char* assetFilePath);
		explicit MaterialGL(JFF::Engine* const engine, const char* name);
		virtual ~MaterialGL();

		// Copy ctor and copy assignment
		MaterialGL(const MaterialGL& other) = delete;
		MaterialGL& operator=(const MaterialGL& other) = delete;

		// Move ctor and assignment
		MaterialGL(MaterialGL&& other) = delete;
		MaterialGL operator=(MaterialGL&& other) = delete;

		// --------------------------- MATERIAL INTERFACE --------------------------- //

		// Sets the name of this material. Name cannot be changed after this material is cooked
		virtual void setName(const char* name) override;

		// Gets the name of the material
		virtual std::string getName() const override { return name; }

		// Sets the material domain. Domain cannot be changed after this material is cooked
		virtual void setDomain(MaterialDomain domain) override;

		// Gets the material domain, which translates to the render pass that uses this material
		virtual MaterialDomain getDomain() const override { return domain; }

		// Sets the material's light model. Light model cannot be changed after this material is cooked
		virtual void setLightModel(LightModel lightModel) override;

		// Gets material's light model
		virtual LightModel getLightModel() const override { return lightModel; }

		/* Sets whick faces of the model will be discardedand which will be drawn, regarding the winding order of the polygons.
		* Side cannot be changed after this material is cooked
		*/
		virtual void setSide(Side side) override;

		// Gets material's drawable side
		virtual Side getSide() const override { return side; }

		// Sets if this material will use normal maps for normal calculation or will use primitive (polygon) normals
		virtual void setUseNormalMap(bool useNormalMap) override;

		// Gets if this material will use normal maps for normal calculation or will use primitive (polygon) normals
		virtual bool getUseNormalMap() const override { return useNormalMap; }

		// Gets debug display option if applicable
		virtual DebugDisplay getDebugDisplay() const { return debugDisplay; }

		// Gets post processing params
		virtual PostProcessParams getPostProcessParams() const { return postProcessParams; }

		/**
		* Adds an external pre-loaded texture to the shader.
		* Cannot add a new texture after this material is cooked
		*/
		virtual void addTexture(const std::shared_ptr<Texture>& texture) override;

		/**
		* Compile and link shaders generated from provided infoand makes this material operative for rendering.
		* A Material can only be cooked once
		*/
		virtual void cook(const std::string& externalCustomCode = "") override;

		// Enables the internal shader and its associated textures
		virtual void use() override;

		/*
		* Send a 4x4 matrix to internal active shader and attachs it to the variable name.
		* The variable name must be a valid uniform included on internal shader code and
		* material must be currently active with use()
		*/
		virtual void sendMat4(const char* variableName, const Mat4& matrix) override;

		/*
		* Send a 3x3 matrix to internal active shader and attachs it to the variable name.
		* The variable name must be a valid uniform included on internal shader code and
		* material must be currently active with use()
		*/
		virtual void sendMat3(const char* variableName, const Mat3& matrix) override;

		/*
		* Send a vec2 to active material and attach it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec2(const char* variableName, const Vec2& vec) override;

		/*
		* Send a vec3 to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec3(const char* variableName, const Vec3& vec) override;

		/*
		* Send a vec4 to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec4(const char* variableName, const Vec4& vec) override;

		/*
		* Send a float to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendFloat(const char* variableName, float f) override;

		/*
		* Send an environment map to active material.
		* If envMap is nullptr, send an empty environment map to material. This is important if no environment map is present,
		* because the cubemap sampler in shader must be specified
		*/
		virtual void sendEnvironmentMap(const std::shared_ptr<Cubemap>& envMap = nullptr) override;

		/*
		* Send a directional light's shadowmap texture to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxDirectionalLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important
		* because all texture samplers in shader must be specified
		*/
		virtual void sendDirLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer> shadowMapFBO = std::weak_ptr<Framebuffer>()) override;

		/*
		* Send a point light's shadowmap cubemap to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxPointLights())
		* If @shadowCubemapFBO is invalid, this function will send the default bind cubemap. This is important
		* because all cubemap samplers in shader must be specified
		*/
		virtual void sendPointLightShadowCubemap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowCubemapFBO = std::weak_ptr<Framebuffer>()) override;

		/*
		* Send a spot light's shadowmap texture to this material located at given index.
		* Index must be in range [0, Renderer::getForwardShadingMaxSpotLights())
		* If @shadowMapFBO is invalid, this function will send the default bind texture. This is important
		* because all texture samplers in shader must be specified
		*/
		virtual void sendSpotLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer> shadowMapFBO = std::weak_ptr<Framebuffer>()) override;

		/*
		* Send all post-processing textures to this material
		*/
		virtual void sendPostProcessingTextures(const std::weak_ptr<Framebuffer>& ppFBO, const std::weak_ptr<Framebuffer>& ppFBO2 = std::weak_ptr<Framebuffer>()) override;

		// Releases GPU data and unbind internal shaders
		virtual void destroy() override;

	private: // Aux functions
		// Shader compilation status
		inline bool checkShaderCompilation(GLuint shader);
		inline bool checkProgramLinkStatus(GLuint program);

		// Internal command to send textures to shader code as uniforms
		void sendTexture(const char* variableName, int textureUnit);

		// Material options extraction from file
		inline void extractMaterialOptionsFromFile(const std::shared_ptr<INIFile>& iniFile);
			inline void extractMaterialDomain(const std::string& option);
			inline void extractLightingModel(const std::string& option);
			inline void extractSide(const std::string& option);
			inline void extractUseNormalMap(const std::string& option);

		// Material debug options extraction from file
		inline void extractMaterialDebugOptionsFromFile(const std::shared_ptr<INIFile>& iniFile);
			inline void extractDebugDisplay(const std::string& option);

		// Material post-process options extraction from file
		inline void extractPostProcessOptionsFromFile(const std::shared_ptr<INIFile>& iniFile);
			inline void extractBloomEnabled(const std::string& option);
			inline void extractBloomThreshold(const std::string& option);
			inline void extractBloomIntensity(const std::string& option);

		// Load textures associated with this material
		inline void loadTexturesFromFile(const std::shared_ptr<INIFile>& iniFile, Engine* const engine);
		inline void loadCubemapsFromFile(const std::shared_ptr<INIFile>& iniFile, Engine* const engine);
		inline void extractPostProcessingTextures();
		inline void extractEnvironmentMaps();
		inline void extractShadowMaps();

		// Extracts user code from ini file and stores it in a list
		inline void extractCustomCodeFromFile(const std::shared_ptr<INIFile>& iniFile);

	protected:
		Engine* engine;
		bool cooked;
		bool isDestroyed;

		std::string name;
		GLuint program;
		MaterialDomain domain;
		LightModel lightModel;
		Side side;
		bool useNormalMap;

		// Debug options
		DebugDisplay debugDisplay;

		// Post process options
		PostProcessParams postProcessParams;

		// 1�-> Texture unit 
		// 2�-> Shader sampler name 
		// 3�-> Texture object or texture attachment point inside a framebuffer
		// 4�-> Framebuffer used: 0-> ppFBO | 1-> ppFBO2
		std::vector<std::tuple<int, std::string>> environmentMaps;
		std::vector<std::tuple<int, std::string, std::shared_ptr<Texture>>> textures;
		std::vector<std::tuple<int, std::string, std::shared_ptr<Cubemap>>> cubemaps;
		std::vector<std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>> postProcessingTextures;
		std::vector<std::tuple<int, std::string, Framebuffer::AttachmentPoint>> directionalLightShadowMaps;
		std::vector<std::tuple<int, std::string, Framebuffer::AttachmentPoint>> pointLightShadowCubemaps;
		std::vector<std::tuple<int, std::string, Framebuffer::AttachmentPoint>> spotLightShadowMaps;
		std::ostringstream customCode;
		int textureUnit;
	};
}