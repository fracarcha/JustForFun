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

#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "MaterialFunctionCodeBuilder.h"
#include "Texture.h"
#include "Vec.h"

#include <string>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiString;
enum aiTextureType;
enum aiTextureMapping;
enum aiTextureOp;
enum aiTextureMapMode;

namespace JFF
{
	class Engine;
	class INIFile;

	class ModelAssimp : public Model
	{
	public:
		// Ctor & Dtor
		explicit ModelAssimp(const char* assetFilePath, Engine* const engine, const std::weak_ptr<GameObject>& parentGameObject);
		explicit ModelAssimp(const char* assetFilePath, Engine* const engine);
		virtual ~ModelAssimp();

		// Copy ctor and copy assignment
		ModelAssimp(const ModelAssimp& other) = delete;
		ModelAssimp& operator=(const ModelAssimp& other) = delete;

		// Move ctor and assignment
		ModelAssimp(ModelAssimp&& other) = delete;
		ModelAssimp operator=(ModelAssimp&& other) = delete;

		// Model interface
		virtual std::weak_ptr<GameObject> getGameObject() const override;

	private:
		inline std::string extractModelRelativePathFromFile(const std::shared_ptr<INIFile>& iniFile) const;
		inline std::string extractFolder(const std::string& fullPath) const;
		inline int extractModelConfigLoadOptionsFromFile(const std::shared_ptr<INIFile>& iniFile) const;
		inline void extractModelConfigUseNormalMapFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void extractModelConfigUseParallaxMapFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void extractModelConfigTranslucentFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void extractModelConfigRenderBackFacesFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void extractModelConfigLightModelFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void extractModelConfigPBRWorkflowFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void loadTexturesFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void extractModelDebugMaterialFromFile(const std::shared_ptr<INIFile>& iniFile);
		inline void extractModelMaterialOverrideFunctionFromFile(const std::shared_ptr<INIFile>& iniFile);

		inline void extractLocalTransform(aiNode* node, Vec3& localPos, Vec3& localRot, Vec3& localScale);
		inline void processRootNode(aiNode* node, const aiScene* scene);
		void processNode(aiNode* node, const aiScene* scene, const std::weak_ptr<GameObject>& parentGameObject); // Not inline because it's recursive
		inline void processMesh(aiMesh* mesh, const aiScene* scene, const std::weak_ptr<GameObject>& parentGameObject);

		inline std::shared_ptr<Mesh> generateMesh(aiMesh* mesh, const std::string& meshName) const;
		inline std::shared_ptr<Material> generateMaterial(const aiScene* scene, aiMesh* mesh, const std::string& meshName);

		inline bool fillMaterialWithTexturesOfType(aiTextureType texType, aiMesh* mesh, aiMaterial* mat, const aiScene* scene,
			std::shared_ptr<Material>& material, std::shared_ptr<MaterialFunctionCodeBuilder>& materialFunctionCodeBuilder);
		inline void fillMaterialWithConstantsOfType(aiTextureType texType, aiMaterial* mat, 
			std::shared_ptr<MaterialFunctionCodeBuilder>& materialFunctionCodeBuilder);

		inline void adaptTexture(const aiString& texPath, const aiScene* scene, const aiTextureMapMode (&mapMode)[3], 
			const aiTextureType& texType, std::shared_ptr<Texture>& texture) const;
		inline void adaptApplication(const aiTextureType& texType, MaterialFunctionCodeBuilder::Aplication& outApplication) const;
		inline void adaptTextureMapping(const aiTextureMapping& mapping, MaterialFunctionCodeBuilder::TextureMapping& outMapping) const;
		inline void adaptTextureUVUsed(unsigned int uvIndex, aiMesh* mesh, std::string& uvVariableNameUsed) const;
		inline void adaptTextureBlendFactor(float blend, float& blendFactor) const;
		inline void adaptTextureOp(const aiTextureOp& texOp, MaterialFunctionCodeBuilder::TextureOp& textureOp) const;
		inline void adaptTextureCoordsWrapMode(const aiTextureMapMode (&mapMode)[3], Texture::CoordsWrapMode& wrapMode) const;

	protected:
		Engine* engine;

		std::string modelName;
		std::string modelFolder;
		std::weak_ptr<GameObject> parentObj;
		std::weak_ptr<GameObject> loadedModel;

		bool translucentMaterial;
		bool normalMapInMaterialNormalChannel;
		bool normalMapInMaterialNormalCameraChannel;

		enum class NormalMapUsage : char
		{
			USE,
			USE_WITH_CAUTION,
			DONT_USE,
		} useNormalMap;
		bool useParallaxMap;
		bool enableTranslucency;
		bool renderBackfaces;
		bool isPBR;
		bool PBRMetallicWorkflow; // True: Metallic workflow. False: Specular workflow
		std::vector<std::shared_ptr<Texture>> externalTextures;
		std::string materialOverrideFunction;
		std::string debugMaterialName;
	};
}