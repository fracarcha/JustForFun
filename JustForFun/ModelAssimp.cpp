/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ModelAssimp.h"
#include "FileSystemSetup.h"

#include "Engine.h"
#include "Log.h"
#include "MeshRenderComponent.h"
#include "ShaderCodeBuilder.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype> // Used isalnum() function

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);
extern std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const char* name, const char* assetFilePath);
extern std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const JFF::Texture::Params& params);

extern std::shared_ptr<JFF::MaterialFunctionCodeBuilder> createMaterialFunctionCodeBuilder();

JFF::ModelAssimp::ModelAssimp(const char* assetFilePath, Engine* const engine, const std::weak_ptr<GameObject>& parentGameObject) :
	engine(engine),

	parentObj(parentGameObject),
	loadedModel(),

	translucentMaterial(false),
	normalMapInMaterialNormalChannel(false),
	normalMapInMaterialNormalCameraChannel(false),

	externalTextures()
{
	JFF_LOG_INFO("Ctor ModelAssimp")

	// Load INI file and extract all of its options
	std::shared_ptr<INIFile> iniFile = engine->io.lock()->loadINIFile(assetFilePath);
	std::string relativePath = extractModelRelativePathFromFile(iniFile);
	modelFolder = extractFolder(relativePath);
	int loadingFlags = extractModelConfigLoadOptionsFromFile(iniFile);

	extractModelConfigUseNormalMapFromFile(iniFile);
	extractModelConfigUseParallaxMapFromFile(iniFile);
	extractModelConfigTranslucentFromFile(iniFile);
	extractModelConfigRenderBackFacesFromFile(iniFile);
	extractModelConfigLightModelFromFile(iniFile);
	extractModelConfigPBRWorkflowFromFile(iniFile);

	loadTexturesFromFile(iniFile); // Loads textures from asset file that are not included as part of the model
	extractModelDebugMaterialFromFile(iniFile);
	extractModelMaterialOverrideFunctionFromFile(iniFile);

	// Load the model
	modelName = relativePath;

	std::ostringstream oss;
	oss << "Assets" << JFF_SLASH << relativePath;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(oss.str(), loadingFlags);

	// Check if model was successfully loaded
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		JFF_LOG_ERROR("Model with name " << assetFilePath << " Couldn't be loaded. Reason: " << importer.GetErrorString())
		return;
	}

	// Process nodes recursively, begining from root node
	processRootNode(scene->mRootNode, scene);
}

JFF::ModelAssimp::ModelAssimp(const char* assetFilePath, Engine* const engine) :
	ModelAssimp(assetFilePath, engine, std::weak_ptr<GameObject>())
{}


JFF::ModelAssimp::~ModelAssimp()
{
	JFF_LOG_INFO("Dtor ModelAssimp")
}

std::weak_ptr<JFF::GameObject> JFF::ModelAssimp::getGameObject() const
{
	return loadedModel;
}

inline std::string JFF::ModelAssimp::extractModelRelativePathFromFile(const std::shared_ptr<INIFile>& iniFile) const
{
	std::string path = std::regex_replace(iniFile->getString("model", "path"), std::regex(R"raw(/)raw"), JFF_SLASH_STRING);
	return path;
}

inline std::string JFF::ModelAssimp::extractFolder(const std::string& fullPath) const
{
	std::string folder;

	// Extract the path to the file without the filename
	std::stringstream ss(fullPath);
	std::string segment;
	std::vector<std::string> tokens;

	while (std::getline(ss, segment, JFF_SLASH))
		tokens.push_back(segment);

	for (int i = 0; i < tokens.size() - 1; ++i)
	{
		folder += tokens[i];
		if (i != tokens.size() - 2)
			folder += JFF_SLASH_STRING;
	}

	return folder;
}

inline int JFF::ModelAssimp::extractModelConfigLoadOptionsFromFile(const std::shared_ptr<INIFile>& iniFile) const
{
	int optionFlags = 0;

	if (iniFile->has("config", "triangulate"))
		optionFlags |= iniFile->getString("config", "triangulate") == "true" ? aiProcess_Triangulate : 0;
	
	if (iniFile->has("config", "flip-uv"))
		optionFlags |= iniFile->getString("config", "flip-uv") == "true" ? aiProcess_FlipUVs : 0;

	if (iniFile->has("config", "optimize"))
		optionFlags |= iniFile->getString("config", "optimize") == "true" ? aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes : 0;

	if (iniFile->has("config", "calculate-tangents"))
		optionFlags |= iniFile->getString("config", "calculate-tangents") == "true" ? aiProcess_CalcTangentSpace : 0;

	return optionFlags;
}

inline void JFF::ModelAssimp::extractModelConfigUseNormalMapFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	useNormalMap = NormalMapUsage::DONT_USE;
	if (iniFile->has("config", "use-normal-map"))
	{
		std::string option = iniFile->getString("config", "use-normal-map");
		if (option == "true")
			useNormalMap = NormalMapUsage::USE_WITH_CAUTION;
		else if (option == "false")
			useNormalMap = NormalMapUsage::DONT_USE;
		else if (option == "force")
			useNormalMap = NormalMapUsage::USE;
	}
}

inline void JFF::ModelAssimp::extractModelConfigUseParallaxMapFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	useParallaxMap = iniFile->has("config", "use-parallax-map") && iniFile->getString("config", "use-parallax-map") == "true";
}

inline void JFF::ModelAssimp::extractModelConfigTranslucentFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	enableTranslucency = iniFile->has("config", "enable-translucency") && iniFile->getString("config", "enable-translucency") == "true";
}

inline void JFF::ModelAssimp::extractModelConfigRenderBackFacesFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	renderBackfaces = iniFile->has("config", "render-back-faces") && iniFile->getString("config", "render-back-faces") == "true";
}

inline void JFF::ModelAssimp::extractModelConfigLightModelFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	isPBR = iniFile->has("config", "light-model") && iniFile->getString("config", "light-model") == "PBR";
}

inline void JFF::ModelAssimp::extractModelConfigPBRWorkflowFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	PBRMetallicWorkflow = iniFile->has("config", "pbr-workflow") && iniFile->getString("config", "pbr-workflow") == "METALLIC";
}

inline void JFF::ModelAssimp::loadTexturesFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	iniFile->visitKeyValuePairs("textures", [this](const std::pair<std::string, std::string>& pair)
		{
			std::string path = std::regex_replace(pair.second, std::regex(R"raw(/)raw"), JFF_SLASH_STRING);
			std::shared_ptr<Texture> texture = createTexture(engine, pair.first.c_str(), path.c_str());
			externalTextures.push_back(texture);
		});
}

inline void JFF::ModelAssimp::extractModelDebugMaterialFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	if (iniFile->has("debug", "debug-mat-path"))
		debugMaterialName = iniFile->getString("debug", "debug-mat-path");
}

inline void JFF::ModelAssimp::extractModelMaterialOverrideFunctionFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	std::ostringstream oss;

	// NOTE: a function called 'materialOverrides' must be present
	iniFile->visitKeyValuePairs("material", [&oss](const std::pair<std::string, std::string>& pair)
		{
			oss << pair.second;
		});

	materialOverrideFunction = oss.str();
}

void JFF::ModelAssimp::extractLocalTransform(aiNode* node, Vec3& localPos, Vec3& localRot, Vec3& localScale)
{
	// Extract Assimp's trasform components
	aiVector3D aiLocalPos, aiLocalRot, aiLocalScale;
	node->mTransformation.Decompose(aiLocalScale, aiLocalRot, aiLocalPos);

	// Set JFF transform components from extracted Assimp components
	// NOTE: Assimp uses radians in their rotations, but JFF unit is degrees
	auto math = engine->math.lock();
	localPos = Vec3(aiLocalPos.x, aiLocalPos.y, aiLocalPos.z);
	localRot = Vec3(math->degrees(aiLocalRot.x), math->degrees(aiLocalRot.y), math->degrees(aiLocalRot.z));
	localScale = Vec3(aiLocalScale.x, aiLocalScale.y, aiLocalScale.z);
}

void JFF::ModelAssimp::processRootNode(aiNode* node, const aiScene* scene)
{
	// Extract position, rotation and scale from this node
	Vec3 localPos, localRot, localScale;
	extractLocalTransform(node, localPos, localRot, localScale);

	// Spawn an initially disabled GameObject
	if (parentObj.expired()) // No parent defined
	{
		loadedModel = engine->logic.lock()->spawnGameObject(modelName.c_str(), localPos, localRot, localScale, false);
	}
	else
	{
		loadedModel = engine->logic.lock()->spawnGameObject(modelName.c_str(), parentObj, localPos, localRot, localScale, false);
	}

	// Load meshes
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		processMesh(scene->mMeshes[node->mMeshes[i]], scene, loadedModel);

	// Load nodes recursively
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene, loadedModel);
}

void JFF::ModelAssimp::processNode(aiNode* node, const aiScene* scene, const std::weak_ptr<GameObject>& parentGameObject)
{
	// Extract position, rotation and scale from this node
	Vec3 localPos, localRot, localScale;
	extractLocalTransform(node, localPos, localRot, localScale);
	
	// Create an empty GameObject node 
	std::string nodeObjName = parentGameObject.lock()->getName().append(".node-").append(node->mName.C_Str());
	auto nodeObj = engine->logic.lock()->spawnGameObject(nodeObjName.c_str(), parentGameObject, localPos, localRot, localScale);

	// Load meshes
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
		processMesh(scene->mMeshes[node->mMeshes[i]], scene, nodeObj);

	// Load nodes recursively
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene, nodeObj);
}

void JFF::ModelAssimp::processMesh(aiMesh* mesh, const aiScene* scene, const std::weak_ptr<GameObject>& parentGameObject)
{
	// Create a GameObject that will contain the mesh
	std::string meshObjName = parentGameObject.lock()->getName().append(".mesh-").append(mesh->mName.C_Str());
	auto meshObj = engine->logic.lock()->spawnGameObject(meshObjName.c_str(), parentGameObject); // Locate at local (0,0,0)
	auto meshObjHandler = meshObj.lock();

	// Create mesh component from mesh data
	std::string meshCompName = meshObjName + ".meshComp";
	std::shared_ptr<Mesh> finalMesh = generateMesh(mesh, meshObjName);
	meshObjHandler->addComponent<MeshComponent>(meshCompName.c_str(), true, finalMesh);

	// Create mesh render component from mesh' material data
	std::string meshRenderName = meshObjName + ".renderComp";
	std::shared_ptr<Material> material = generateMaterial(scene, mesh, meshObjName);
	meshObjHandler->addComponent<MeshRenderComponent>(meshRenderName.c_str(), true, material);

	// If valid, generate a MeshRenderComponent with debug info
	if (!debugMaterialName.empty())
	{
		std::string meshDebugRenderName = meshObjName + ".debugRenderComp";
		meshObjHandler->addComponent<MeshRenderComponent>(meshDebugRenderName.c_str(), true, debugMaterialName.c_str());
	}
}

inline std::shared_ptr<JFF::Mesh> JFF::ModelAssimp::generateMesh(aiMesh* mesh, const std::string& meshName) const
{
	std::shared_ptr<Mesh> finalMesh = std::make_shared<Mesh>();

	// Ensure this mesh has positions
	if (!mesh->HasPositions())
	{
		JFF_LOG_WARNING("Error processing mesh with name " << meshName << ". This mesh doesn't have vertices")
		return nullptr;
	}

	// Check if this mesh has more than triangle primitives
	if ((mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE) != mesh->mPrimitiveTypes)
	{
		JFF_LOG_WARNING("Mesh " << meshName << " has other primitives than triangles. Non triangle primitives will be omitted")
	}

	finalMesh->isDataCollapsed = false; // Data will be splitted to vertices, normals, uvs ...

	// TODO: remove 
	//finalMesh->componentsPerVertex = 3; // Assimp always assumes this as 3
	//finalMesh->componentsPerNormal = 3; // Assimp always assumes this as 3
	//finalMesh->componentsPerUV = mesh->mNumUVComponents[0]; // TODO: Assimp can have more than one texcoord (uv) per mesh. Check this!

	finalMesh->useNormals = mesh->HasNormals(); // False if there aren't normals present in model
	finalMesh->useTangents = finalMesh->useBitangents = mesh->HasTangentsAndBitangents(); // False if there aren't tangents and bitangents present in model
	finalMesh->useUV = mesh->HasTextureCoords(0); // False if there aren't UVs present in the model. // TODO: Assimp can have more than one texcoord (uv) per mesh. Check this!
	finalMesh->useFaces = mesh->HasFaces(); // Check if this model use faces (Indices). Assimp normally use faces
	if (!finalMesh->useFaces)
		finalMesh->primitiveAssemblyMethod = Mesh::PrimitiveAssemblyMethod::TRIANGLES; // TODO: Used triangles because we call aiProcess_Triangulate when loading the model

	// Copy vertex data (positions, normals, uv, etc.) from Assimp to Mesh structures
	finalMesh->reserve(mesh->mNumVertices);
	memcpy(finalMesh->vertices, mesh->mVertices, sizeof(float) * finalMesh->verticesSize);

	if (finalMesh->useNormals)
		memcpy(finalMesh->normals, mesh->mNormals, sizeof(float) * finalMesh->normalsSize);

	if (finalMesh->useTangents)
		memcpy(finalMesh->tangents, mesh->mTangents, sizeof(float) * finalMesh->tangentsSize);

	if (finalMesh->useBitangents)
		memcpy(finalMesh->bitangents, mesh->mBitangents, sizeof(float) * finalMesh->bitangentsSize);

	if (finalMesh->useUV)
		memcpy(finalMesh->uv, mesh->mTextureCoords[0], sizeof(float) * finalMesh->uvSize); // TODO: copy all UV channels

	// Iterate on indices
	if (finalMesh->useFaces)
	{
		std::pair<Mesh::PrimitiveAssemblyMethod, std::vector<unsigned int>> triangleIndices;
		triangleIndices.first = Mesh::PrimitiveAssemblyMethod::TRIANGLES;
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) // TODO: Extremely slow solution!! Use memcpy or something similar
		{
			aiFace face = mesh->mFaces[i];
			if (face.mNumIndices == 3) // As promised before, primitives other than triangles will be omitted
			{
				triangleIndices.second.push_back(face.mIndices[0]);
				triangleIndices.second.push_back(face.mIndices[1]);
				triangleIndices.second.push_back(face.mIndices[2]);
			}
		}
		finalMesh->faces.insert(triangleIndices);
	}

	return finalMesh;
}

inline std::shared_ptr<JFF::Material> JFF::ModelAssimp::generateMaterial(const aiScene* scene, aiMesh* mesh, const std::string& meshName)
{
	// Ensure this mesh has a material
	if (mesh->mMaterialIndex < 0)
	{
		JFF_LOG_WARNING("Mesh with name " << meshName << " doesn't have material. Mesh discarded")
		return nullptr;
	}

	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
	std::shared_ptr<Material> material = createMaterial(engine, mat->GetName().C_Str());

	// Create a helper class to build the material() function
	std::shared_ptr<MaterialFunctionCodeBuilder> matFuncBuilder = createMaterialFunctionCodeBuilder();

	// Load material textures included in model and add them to shader code
	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_HEIGHT, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_HEIGHT, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_DISPLACEMENT, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_DISPLACEMENT, mat, matFuncBuilder);

	if (!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_DIFFUSE, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_DIFFUSE, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_SPECULAR, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_SPECULAR, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_AMBIENT, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_AMBIENT, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_EMISSIVE, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_EMISSIVE, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_NORMALS, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_NORMALS, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_SHININESS, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_SHININESS, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_OPACITY, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_OPACITY, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_LIGHTMAP, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_LIGHTMAP, mat, matFuncBuilder);

	if(!fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_REFLECTION, mesh, mat, scene, material, matFuncBuilder))
		fillMaterialWithConstantsOfType(aiTextureType::aiTextureType_REFLECTION, mat, matFuncBuilder);

	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_BASE_COLOR, mesh, mat, scene, material, matFuncBuilder);
	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_NORMAL_CAMERA, mesh, mat, scene, material, matFuncBuilder);
	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_EMISSION_COLOR, mesh, mat, scene, material, matFuncBuilder);
	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_METALNESS, mesh, mat, scene, material, matFuncBuilder);
	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, mesh, mat, scene, material, matFuncBuilder);
	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_AMBIENT_OCCLUSION, mesh, mat, scene, material, matFuncBuilder);

	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_SHEEN, mesh, mat, scene, material, matFuncBuilder);
	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_CLEARCOAT, mesh, mat, scene, material, matFuncBuilder);
	fillMaterialWithTexturesOfType(aiTextureType::aiTextureType_TRANSMISSION, mesh, mat, scene, material, matFuncBuilder);

	// TODO: PBR support here

	// TODO: There is room to extract way more parameters from material. Check Assimp's material.h line:944
	// More info: http://assimp.sourceforge.net/lib_html/material_8h.html

	// Fill material with external textures given from asset file
	for (auto tex : externalTextures)
		material->addTexture(tex);

	// Generate material() and materialOverrides() function
	matFuncBuilder->addMaterialOverrideFunction(materialOverrideFunction);

	std::string materialFunctionCode;
	matFuncBuilder->generateCode(materialFunctionCode, useParallaxMap, isPBR);

	// Decide if normals from normal maps will be used
	bool finalUseNormalMap = false;
	switch (useNormalMap)
	{
	case JFF::ModelAssimp::NormalMapUsage::USE:
		finalUseNormalMap = true;
		break;
	case JFF::ModelAssimp::NormalMapUsage::USE_WITH_CAUTION:
		finalUseNormalMap = normalMapInMaterialNormalChannel || normalMapInMaterialNormalCameraChannel;
		break;
	case JFF::ModelAssimp::NormalMapUsage::DONT_USE:
	default:
		break;
	}

	// Cook (compile and link shaders) the material
	material->setLightModel(isPBR ? Material::LightModel::PBR : Material::LightModel::BLINN_PHONG);
	material->setPBRWorkflow(PBRMetallicWorkflow ? Material::PBRWorkflow::METALLIC : Material::PBRWorkflow::SPECULAR);
	material->setUseNormalMap(finalUseNormalMap);
	material->setDomain(translucentMaterial ? Material::MaterialDomain::TRANSLUCENT : Material::MaterialDomain::SURFACE);
	material->setSide(renderBackfaces ? Material::Side::TWO_SIDED : Material::Side::FRONT);

	material->cook(materialFunctionCode);
	return material;
}

inline bool JFF::ModelAssimp::fillMaterialWithTexturesOfType(aiTextureType texType, aiMesh* mesh, aiMaterial* mat, 
	const aiScene* scene, std::shared_ptr<Material>& material, std::shared_ptr<MaterialFunctionCodeBuilder>& materialFunctionCodeBuilder)
{
	unsigned int texCount = mat->GetTextureCount(texType);

	// In special case of Opacity channel, a check of texture presence is needed
	if (texType == aiTextureType::aiTextureType_OPACITY)
		translucentMaterial = texCount > 0 && enableTranslucency;

	// If there is a normal map in any of normal channels, use normal map in this material if option useNormalMap is set.
	// If there isn't a normal map on any of the channels, don't use normal map even if option useNormalMap is set
	if (texType == aiTextureType::aiTextureType_NORMALS)
		normalMapInMaterialNormalChannel = texCount > 0;
	if(texType == aiTextureType::aiTextureType_NORMAL_CAMERA)
		normalMapInMaterialNormalCameraChannel = texCount > 0;

	// Loop over textures of given type and add them to the corresponding material function line
	for (unsigned int i = 0; i < texCount; ++i)
	{
		// Extract individual texture params
		aiString path;
		aiTextureMapping mapping = aiTextureMapping_UV;
		unsigned int uvIndex = 0;
		ai_real blend = 1.0f;
		aiTextureOp texOp = aiTextureOp_Multiply;
		aiTextureMapMode mapMode[3]{ aiTextureMapMode_Wrap , aiTextureMapMode_Wrap ,aiTextureMapMode_Wrap };
		mat->GetTexture(texType, i, &path, &mapping, &uvIndex, &blend, &texOp, mapMode);

		// Texture parameters
		std::shared_ptr<Texture> texture;
		MaterialFunctionCodeBuilder::Aplication texApplication;
		MaterialFunctionCodeBuilder::TextureMapping textureMapping;
		std::string uvVariableNameUsed;
		float blendFactor;
		MaterialFunctionCodeBuilder::TextureOp textureOp;

		// Adapt assimp paramenters to Texture paramenters
		adaptTexture(path, scene, mapMode, texType, texture);
		adaptApplication(texType, texApplication);
		adaptTextureMapping(mapping, textureMapping);
		adaptTextureUVUsed(uvIndex, mesh, uvVariableNameUsed);
		adaptTextureBlendFactor(blend, blendFactor);
		adaptTextureOp(texOp, textureOp);

		// Add texture to material
		material->addTexture(texture);

		// Add texture to material() function
		materialFunctionCodeBuilder->addTextureLine(texture->getName(), texApplication, textureMapping, uvVariableNameUsed, blendFactor, textureOp);
	}

	return texCount;
}

inline void JFF::ModelAssimp::fillMaterialWithConstantsOfType(aiTextureType texType, aiMaterial* mat, 
	std::shared_ptr<MaterialFunctionCodeBuilder>& materialFunctionCodeBuilder)
{
	// TODO: Does it make sense to talk about constant values in PBR ?? Check
	Vec4 value;
	aiColor4D valueTemp;
	switch (texType)
	{
	case aiTextureType_DIFFUSE:
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, valueTemp);
		break;
	case aiTextureType_SPECULAR:
		mat->Get(AI_MATKEY_COLOR_SPECULAR, valueTemp);
		break;
	case aiTextureType_AMBIENT:
		mat->Get(AI_MATKEY_COLOR_AMBIENT, valueTemp);
		break;
	case aiTextureType_EMISSIVE:
		mat->Get(AI_MATKEY_COLOR_EMISSIVE, valueTemp);
		break;
	case aiTextureType_HEIGHT:
		valueTemp = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		break;
	case aiTextureType_NORMALS:
		valueTemp = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		break;
	case aiTextureType_SHININESS:
		mat->Get(AI_MATKEY_SHININESS, valueTemp);
		break;
	case aiTextureType_OPACITY:
		mat->Get(AI_MATKEY_OPACITY, valueTemp);
		translucentMaterial = valueTemp.r < 1.0f && enableTranslucency; // In special case of Opacity channel, a check of a value < 1.0 is needed
		break;
	case aiTextureType_DISPLACEMENT:
		valueTemp = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		break;
	case aiTextureType_LIGHTMAP:
		valueTemp = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		break;
	case aiTextureType_REFLECTION:
		mat->Get(AI_MATKEY_COLOR_REFLECTIVE, valueTemp);
		break;
	case aiTextureType_BASE_COLOR:
	case aiTextureType_NORMAL_CAMERA:
	case aiTextureType_EMISSION_COLOR:
	case aiTextureType_METALNESS:
	case aiTextureType_DIFFUSE_ROUGHNESS:
	case aiTextureType_AMBIENT_OCCLUSION:
	case aiTextureType_SHEEN:
	case aiTextureType_CLEARCOAT:
	case aiTextureType_TRANSMISSION:

	case aiTextureType_NONE:
	case aiTextureType_UNKNOWN:
	case _aiTextureType_Force32Bit:
	default:
		valueTemp = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		break;
	}

	// Adapt assimp parameters to texture parameters
	value.r = valueTemp.r;
	value.g = valueTemp.g;
	value.b = valueTemp.b;
	value.a = valueTemp.a;

	MaterialFunctionCodeBuilder::Aplication constantApplication;
	adaptApplication(texType, constantApplication);

	// Send value to material shader as a constant
	materialFunctionCodeBuilder->addConstantLine(value, constantApplication);
}

inline void JFF::ModelAssimp::adaptTexture(const aiString& texPath, 
	const aiScene* scene, 
	const aiTextureMapMode (&mapMode)[3], 
	const aiTextureType& texType,
	std::shared_ptr<Texture>& texture) const
{
	// Adapt assimp's texture map mode to JFF's Texture
	Texture::CoordsWrapMode wrapMode;
	adaptTextureCoordsWrapMode(mapMode, wrapMode);

	// Create an image from path
	std::shared_ptr<Image> img;
	const aiTexture* tex = scene->GetEmbeddedTexture(texPath.C_Str());
	if (tex) // If this is != nullptr, it means that texture is embedded
	{
		// Load embedded textures
		if (tex->mHeight == 0) // Load COMPRESSED embedded texture
		{
			// Reinterpret pcData, which is aiTexel* to a unsigned char*
			void* imgBufferPVoid = static_cast<void*>(tex->pcData);
			const unsigned char* imgBuffer = static_cast<unsigned char*>(imgBufferPVoid);

			std::string fullPath = modelFolder + JFF_SLASH_STRING + texPath.C_Str();
			img = engine->io.lock()->loadImage(fullPath.c_str(), imgBuffer, tex->mWidth);
		}
		else // Load UNCOMPRESSED embedded texture
		{
			// TODO: Load uncompressed embedded textures. NOTE: Check achFormatHint from Assimp to know the channel order (e.g. ARGB, BGRA, etc.)
			JFF_LOG_WARNING("Uncompressed embedded texture detected in model. Uncompressed embedded texture loading not yet implemented")
		}
	}
	else // Texture is external
	{
		// Load external texture
		std::string fullPath = modelFolder + JFF_SLASH_STRING + texPath.C_Str();
		img = engine->io.lock()->loadImage(fullPath.c_str());
	}

	// Create a texture name. Texture name will be used as shader uniform sampler
	std::ostringstream texNameSS;
	texNameSS << "tex" << texPath.C_Str() << img->data().width << "x" << img->data().height;
	std::string texName = texNameSS.str();

	// Ensure texName doesn't contain invalid characters to use in shader variables
	texName.erase(std::remove_if(texName.begin(), texName.end(), [](const char& ch) { return !(std::isalnum(ch)); }), texName.end());

	/*
	* Heuristic:
	* 
	* Textures used for coloring objects (like diffuse textures) are almost
	* always in sRGB space. Textures used for retrieving lighting parameters (like specular maps and
	* normal maps) are almost always in linear space
	*/
	bool isSRGBTexture = (texType == aiTextureType::aiTextureType_DIFFUSE || texType == aiTextureType::aiTextureType_BASE_COLOR) ? true : false;

	// Create a texture from image
	Texture::Params texParams;

	texParams.folder				= "";
	texParams.shaderVariableName	= texName;
	texParams.img					= img;
	texParams.coordsWrapMode		= wrapMode;
	texParams.filterMode			= Texture::DEFAULT_FILTER_MODE;
	texParams.numColorChannels		= 4;
	texParams.specialFormat			= isSRGBTexture ? Texture::SpecialFormat::SRGB : Texture::SpecialFormat::NONE;

	texture = createTexture(engine, texParams);
}

inline void JFF::ModelAssimp::adaptApplication(const aiTextureType& texType, MaterialFunctionCodeBuilder::Aplication& outApplication) const
{
	switch (texType)
	{
	case aiTextureType_DIFFUSE:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_DIFFUSE;
		break;
	case aiTextureType_SPECULAR:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_SPECULAR;
		break;
	case aiTextureType_AMBIENT:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_AMBIENT;
		break;
	case aiTextureType_EMISSIVE:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_EMISSIVE;
		break;
	case aiTextureType_HEIGHT:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_HEIGHT;
		break;
	case aiTextureType_NORMALS:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_NORMAL;
		break;
	case aiTextureType_SHININESS:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_SHININESS;
		break;
	case aiTextureType_OPACITY:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_OPACITY;
		break;
	case aiTextureType_DISPLACEMENT:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_DISPLACEMENT;
		break;
	case aiTextureType_LIGHTMAP:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_LIGHTMAP;
		break;
	case aiTextureType_REFLECTION:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PHONG_REFLECTION;
		break;
	case aiTextureType_BASE_COLOR:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_BASE_COLOR;
		break;
	case aiTextureType_NORMAL_CAMERA:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_NORMAL_CAMERA;
		break;
	case aiTextureType_EMISSION_COLOR:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_EMISSION_COLOR;
		break;
	case aiTextureType_METALNESS:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_METALNESS;
		break;
	case aiTextureType_DIFFUSE_ROUGHNESS:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_DIFFUSE_ROUGHNESS;
		break;
	case aiTextureType_AMBIENT_OCCLUSION:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_AMBIENT_OCCLUSION;
		break;
	case aiTextureType_SHEEN:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_SHEEN;
		break;
	case aiTextureType_CLEARCOAT:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_CLEARCOAT;
		break;
	case aiTextureType_TRANSMISSION:
		outApplication = MaterialFunctionCodeBuilder::Aplication::PBR_TRANSMISSION;
		break;
	case aiTextureType_NONE:
	case aiTextureType_UNKNOWN:
	case _aiTextureType_Force32Bit:
	default:
		outApplication = MaterialFunctionCodeBuilder::Aplication::NO_APPLICATION;
		break;
	}
}

inline void JFF::ModelAssimp::adaptTextureMapping(const aiTextureMapping& mapping, MaterialFunctionCodeBuilder::TextureMapping& outMapping) const
{
	switch (mapping)
	{
	case aiTextureMapping_SPHERE:
		outMapping = MaterialFunctionCodeBuilder::TextureMapping::SPHERE;
		break;
	case aiTextureMapping_CYLINDER:
		outMapping = MaterialFunctionCodeBuilder::TextureMapping::CYLINDER;
		break;
	case aiTextureMapping_BOX:
		outMapping = MaterialFunctionCodeBuilder::TextureMapping::BOX;
		break;
	case aiTextureMapping_PLANE:
		outMapping = MaterialFunctionCodeBuilder::TextureMapping::PLANE;
		break;
	case aiTextureMapping_UV:
	case aiTextureMapping_OTHER: // TODO: May need to raise a warning if this option is encountered
	case _aiTextureMapping_Force32Bit:
	default:
		outMapping = MaterialFunctionCodeBuilder::TextureMapping::UV;
		break;
	}
}

inline void JFF::ModelAssimp::adaptTextureUVUsed(unsigned int uvIndex, aiMesh* mesh, std::string& uvVariableNameUsed) const
{
	// TODO: I have serious doubts about this function. I could create my own uv1, uv2, etc and reference it using uvIndex
	// without having to extract the uv name here

	const aiString* coordsName = mesh->GetTextureCoordsName(uvIndex);
	if (coordsName)
	{
		uvVariableNameUsed = coordsName->C_Str();
	}
	else // Use a default uv name
	{
		uvVariableNameUsed = ShaderCodeBuilder::INPUT_UV_0; // TODO: harcoded
	}
}

inline void JFF::ModelAssimp::adaptTextureBlendFactor(float blend, float& blendFactor) const
{
	blendFactor = blend;
}

inline void JFF::ModelAssimp::adaptTextureOp(const aiTextureOp& texOp, MaterialFunctionCodeBuilder::TextureOp& textureOp) const
{
	switch (texOp)
	{
	case aiTextureOp_Multiply:
		textureOp = MaterialFunctionCodeBuilder::TextureOp::MULTIPLY;
		break;
	case aiTextureOp_Add:
		textureOp = MaterialFunctionCodeBuilder::TextureOp::ADD;
		break;
	case aiTextureOp_Subtract:
		textureOp = MaterialFunctionCodeBuilder::TextureOp::SUBTRACT;
		break;
	case aiTextureOp_Divide:
		textureOp = MaterialFunctionCodeBuilder::TextureOp::DIVIDE;
		break;
	case aiTextureOp_SmoothAdd:
		textureOp = MaterialFunctionCodeBuilder::TextureOp::SMOOTH_ADD;
		break;
	case aiTextureOp_SignedAdd:
		textureOp = MaterialFunctionCodeBuilder::TextureOp::SIGNED_ADD;
		break;
	case _aiTextureOp_Force32Bit:
	default:
		textureOp = MaterialFunctionCodeBuilder::TextureOp::NONE;
		break;
	}
}

inline void JFF::ModelAssimp::adaptTextureCoordsWrapMode(const aiTextureMapMode (&mapMode)[3], Texture::CoordsWrapMode& wrapMode) const
{
	// mapMode must point to an array of 3 aiTextureMapMode

	aiTextureMapMode mapModeU = mapMode[0];
	switch (mapModeU)
	{
	case aiTextureMapMode_Clamp:
		wrapMode.u = Texture::Wrap::CLAMP_TO_EDGE;
		break;
	case aiTextureMapMode_Decal:
		wrapMode.u = Texture::Wrap::CLAMP_TO_BORDER;
		break;
	case aiTextureMapMode_Mirror:
		wrapMode.u = Texture::Wrap::MIRRORED_REPEAT;
		break;
	case aiTextureMapMode_Wrap:
	case _aiTextureMapMode_Force32Bit:
	default:
		wrapMode.u = Texture::Wrap::REPEAT;
		break;
	}

	aiTextureMapMode mapModeV = mapMode[1];
	switch (mapModeV)
	{
	case aiTextureMapMode_Clamp:
		wrapMode.v = Texture::Wrap::CLAMP_TO_EDGE;
		break;
	case aiTextureMapMode_Decal:
		wrapMode.v = Texture::Wrap::CLAMP_TO_BORDER;
		break;
	case aiTextureMapMode_Mirror:
		wrapMode.v = Texture::Wrap::MIRRORED_REPEAT;
		break;
	case aiTextureMapMode_Wrap:
	case _aiTextureMapMode_Force32Bit:
	default:
		wrapMode.v = Texture::Wrap::REPEAT;
		break;
	}

	aiTextureMapMode mapModeW = mapMode[2];
	switch (mapModeW)
	{
	case aiTextureMapMode_Clamp:
		wrapMode.w = Texture::Wrap::CLAMP_TO_EDGE;
		break;
	case aiTextureMapMode_Decal:
		wrapMode.w = Texture::Wrap::CLAMP_TO_BORDER;
		break;
	case aiTextureMapMode_Mirror:
		wrapMode.w = Texture::Wrap::MIRRORED_REPEAT;
		break;
	case aiTextureMapMode_Wrap:
	case _aiTextureMapMode_Force32Bit:
	default:
		wrapMode.w = Texture::Wrap::REPEAT;
		break;
	}
	
}
