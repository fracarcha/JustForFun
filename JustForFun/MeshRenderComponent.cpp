/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "MeshRenderComponent.h"

#include "Engine.h"
#include "FileSystemSetup.h"
#include <regex>

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name, const char* assetFilePath);

JFF::MeshRenderComponent::MeshRenderComponent(
	GameObject* const gameObject,
	const char* name, 
	bool initiallyEnabled, 
	const char* materialAssetFilepath) :
	RenderComponent(gameObject, name, initiallyEnabled),
	materialAssetFilepath(materialAssetFilepath),
	material(),
	mesh()
{
	JFF_LOG_INFO("Ctor MeshRenderComponent")
}

JFF::MeshRenderComponent::MeshRenderComponent(
	GameObject* const gameObject, 
	const char* name, 
	bool initiallyEnabled, 
	const std::shared_ptr<Material>& material) :
	RenderComponent(gameObject, name, initiallyEnabled),
	materialAssetFilepath(),
	material(material),
	mesh()
{
	JFF_LOG_INFO("Ctor MeshRenderComponent")
}

JFF::MeshRenderComponent::~MeshRenderComponent()
{
	JFF_LOG_INFO("Dtor MeshRenderComponent")
}

void JFF::MeshRenderComponent::onStart()
{
	// Load material from file if it's null
	if (!material)
	{
		std::string assetFullPath = std::regex_replace(materialAssetFilepath, std::regex(R"raw(/)raw"), JFF_SLASH_STRING);
		material = createMaterial(gameObject->engine, materialAssetFilepath.c_str(), assetFullPath.c_str());
	}

	// Find a MeshComponent on this gameObject
	mesh = gameObject->getComponent<MeshComponent>();
	if (mesh.expired()) // If this GameObject doesn't contain a MeshComponent or there is another error
	{
		JFF_LOG_ERROR("A MeshRenderComponent needs a MeshComponent attached to the gameObject in order to work")
		return;
	}

	// Send this RenderComponent to Renderer
	gameObject->engine->renderer.lock()->addRenderable(this);
}

void JFF::MeshRenderComponent::onDestroy() noexcept
{
	// Destroy the associated material
	material->destroy();

	// Remove this RenderComponent from Renderer
	gameObject->engine->renderer.lock()->removeRenderable(this);
}

JFF::Material::MaterialDomain JFF::MeshRenderComponent::getMaterialDomain() const
{
	return material->getDomain();
}

JFF::Material::LightModel JFF::MeshRenderComponent::getLightModel() const
{
	return material->getLightModel();
}

JFF::Material::Side JFF::MeshRenderComponent::getMaterialSide() const
{
	return material->getSide();
}

JFF::Material::DebugDisplay JFF::MeshRenderComponent::getDebugDisplay() const
{
	return material->getDebugDisplay();
}

void JFF::MeshRenderComponent::useMaterial()
{
	material->use();
}

void JFF::MeshRenderComponent::sendMat4(const char* variableName, const Mat4& matrix)
{
	material->sendMat4(variableName, matrix);
}

void JFF::MeshRenderComponent::sendMat3(const char* variableName, const Mat3& matrix)
{
	material->sendMat3(variableName, matrix);
}

void JFF::MeshRenderComponent::sendVec3(const char* variableName, const Vec3& vec)
{
	material->sendVec3(variableName, vec);
}

void JFF::MeshRenderComponent::sendVec4(const char* variableName, const Vec4& vec)
{
	material->sendVec4(variableName, vec);
}

void JFF::MeshRenderComponent::sendFloat(const char* variableName, float f)
{
	material->sendFloat(variableName, f);
}

void JFF::MeshRenderComponent::sendEnvironmentMap(
	const std::shared_ptr<Cubemap>& envMap, 
	const std::shared_ptr<Cubemap>& irradianceMap, 
	const std::shared_ptr<Cubemap>& preFilteredMap, 
	const std::shared_ptr<Texture>& BRDFIntegrationMap)
{
	material->sendEnvironmentMap(envMap, irradianceMap, preFilteredMap, BRDFIntegrationMap);
}

void JFF::MeshRenderComponent::sendDirLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO)
{
	material->sendDirLightShadowMap(index, shadowMapFBO);
}

void JFF::MeshRenderComponent::sendSpotLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowMapFBO)
{
	material->sendSpotLightShadowMap(index, shadowMapFBO);
}

void JFF::MeshRenderComponent::sendPointLightShadowCubemap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowCubemapFBO)
{
	material->sendPointLightShadowCubemap(index, shadowCubemapFBO);
}

void JFF::MeshRenderComponent::sendPostProcessingTextures(const std::weak_ptr<Framebuffer>& ppFBO, const std::weak_ptr<Framebuffer>& ppFBO2)
{
	material->sendPostProcessingTextures(ppFBO, ppFBO2);
}

void JFF::MeshRenderComponent::draw()
{
	mesh.lock()->draw();
}
