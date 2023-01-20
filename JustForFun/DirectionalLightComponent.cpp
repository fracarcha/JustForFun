/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "DirectionalLightComponent.h"

#include "Log.h"
#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include <sstream>

extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(JFF::Framebuffer::PrefabFramebuffer fboType,
	unsigned int width, unsigned int height, unsigned int samplesPerPixel = 0);

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);

JFF::DirectionalLightComponent::DirectionalLightComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, 
	DirectionalLightComponent::Params params) :
	LightComponent(gameObject, name, initiallyEnabled),
	engine(gameObject->engine),
	
	params(params),

	shadowProjectionMatrix(),
	shadowMapFBO(),
	shadowCastMaterial()
{
	JFF_LOG_INFO("Ctor DirectionalLightComponent")

	setShadowImportanceVolume(params.left, params.right, params.bottom, params.top, params.zNear, params.zFar);
}

JFF::DirectionalLightComponent::~DirectionalLightComponent()
{
	JFF_LOG_INFO("Dtor DirectionalLightComponent")
}

void JFF::DirectionalLightComponent::onStart()
{
	// Create a shadowmap framebuffer if this light casts shadows
	if (params.castShadows)
	{
		shadowMapFBO = createFramebuffer(Framebuffer::PrefabFramebuffer::FBO_SHADOW_MAP, params.shadowMapWidth, params.shadowMapHeight);

		shadowCastMaterial = createMaterial(engine, "Directional light material");
		shadowCastMaterial->setDomain(Material::MaterialDomain::SHADOW_CAST);
		shadowCastMaterial->cook();
	}

	// Register the light in renderer
	gameObject->engine->renderer.lock()->addLight(this);
}

void JFF::DirectionalLightComponent::onDestroy() noexcept
{
	// Unregister the light in Renderer
	gameObject->engine->renderer.lock()->removeLight(this);
	
	// Destroy framebuffer and material
	if (shadowMapFBO)
		shadowMapFBO->destroy();

	if (shadowCastMaterial)
		shadowCastMaterial->destroy();
}

void JFF::DirectionalLightComponent::sendLightParams(RenderComponent* const renderComponent, int lightIndex)
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::DIR_LIGHT_DIRECTION;
	auto lightDir = gameObject->transform.getRotationMatrix() * Vec4::DOWN;
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightDir.x, lightDir.y, lightDir.z));
	
	ss.str(emptyString);
	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::DIR_LIGHT_COLOR;
	renderComponent->sendVec3(ss.str().c_str(), params.color);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::DIR_LIGHT_INTENSITY;
	renderComponent->sendFloat(ss.str().c_str(), params.intensity);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::DIR_LIGHT_CAST_SHADOWS;
	renderComponent->sendFloat(ss.str().c_str(), params.castShadows ? 1.0f : 0.0f);

	if (params.castShadows)
	{
		renderComponent->sendDirLightShadowMap(lightIndex, shadowMapFBO);

		ss.str(emptyString);
		ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_MATRICES << "[" << lightIndex << "]";
		renderComponent->sendMat4(ss.str().c_str(), getProjectionMatrix() * getViewMatrix());
	}
	else
	{
		renderComponent->sendDirLightShadowMap(lightIndex); // Send empty shadow map
	}
}

void JFF::DirectionalLightComponent::sendLightParams(RenderComponent* const renderComponent)
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT << "." << ShaderCodeBuilder::DIR_LIGHT_DIRECTION;
	auto lightDir = gameObject->transform.getRotationMatrix() * Vec4::DOWN;
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightDir.x, lightDir.y, lightDir.z));

	ss.str(emptyString);
	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT << "." << ShaderCodeBuilder::DIR_LIGHT_COLOR;
	renderComponent->sendVec3(ss.str().c_str(), params.color);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT << "." << ShaderCodeBuilder::DIR_LIGHT_INTENSITY;
	renderComponent->sendFloat(ss.str().c_str(), params.intensity);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT << "." << ShaderCodeBuilder::DIR_LIGHT_CAST_SHADOWS;
	renderComponent->sendFloat(ss.str().c_str(), params.castShadows ? 1.0f : 0.0f);

	if (params.castShadows)
	{
		renderComponent->sendDirLightShadowMap(0, shadowMapFBO);

		ss.str(emptyString);
		ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_MATRIX;
		renderComponent->sendMat4(ss.str().c_str(), getProjectionMatrix() * getViewMatrix());
	}
	else
	{
		renderComponent->sendDirLightShadowMap(0); // Send empty shadow map
	}
}

void JFF::DirectionalLightComponent::enableShadowMapFramebuffer()
{
	shadowMapFBO->enable();
}

void JFF::DirectionalLightComponent::disableShadowMapFramebuffer()
{
	shadowMapFBO->disable();
}

void JFF::DirectionalLightComponent::getShadowMapSizePixels(unsigned int& outWidth, unsigned int& outHeight) const
{
	outWidth = params.shadowMapWidth;
	outHeight = params.shadowMapHeight;
}

void JFF::DirectionalLightComponent::useMaterial()
{
	shadowCastMaterial->use();
}

void JFF::DirectionalLightComponent::sendMat4(const char* variableName, const Mat4& matrix)
{
	shadowCastMaterial->sendMat4(variableName, matrix);
}

void JFF::DirectionalLightComponent::sendVec3(const char* variableName, const Vec3& vec)
{
	shadowCastMaterial->sendVec3(variableName, vec);
}

void JFF::DirectionalLightComponent::sendFloat(const char* variableName, float f)
{
	shadowCastMaterial->sendFloat(variableName, f);
}

JFF::Mat4 JFF::DirectionalLightComponent::getViewMatrix() const
{
	// TODO: Don't calculate this every frame. Link this to transform's changes
	
	// Get world position of the light
	Vec3 lightPos = gameObject->transform.getWorldPos();

	// Get world rotation
	Vec4 lightDir4 = gameObject->transform.getRotationMatrix() * Vec4::DOWN;
	Vec3 lightDir(lightDir4.pitch, lightDir4.yaw, lightDir4.roll);

	return gameObject->engine->math.lock()->lookAt(lightPos, lightPos + lightDir, Vec3::UP);
}

JFF::Mat4 JFF::DirectionalLightComponent::getProjectionMatrix() const
{
	return shadowProjectionMatrix;
}

void JFF::DirectionalLightComponent::setColor(Vec3 newColor)
{
	params.color = newColor;
}

void JFF::DirectionalLightComponent::setColor(float red, float green, float blue)
{
	params.color.red = red;
	params.color.green = green;
	params.color.blue = blue;
}

void JFF::DirectionalLightComponent::setShadowImportanceVolume(float left, float right, float bottom, float top, float zNear, float zFar)
{
	params.left = left;
	params.right = right;
	params.bottom = bottom;
	params.top = top;
	params.zNear = zNear;
	params.zFar = zFar;

	// Selected ortho matrix because directional light rays are parallel
	shadowProjectionMatrix = gameObject->engine->math.lock()->ortho(left, right, bottom, top, zNear, zFar);
}

void JFF::DirectionalLightComponent::setIntensity(float newIntensity)
{
	params.intensity = newIntensity;
}

void JFF::DirectionalLightComponent::getShadowImportanceVolume(float& outLeft, float& outRight, float& outBottom, float& outTop, float& outZNear, float& outZFar) const
{
	outLeft = params.left;
	outRight = params.right;
	outBottom = params.bottom;
	outTop = params.top;
	outZNear = params.zNear;
	outZFar = params.zFar;
}

JFF::Vec3 JFF::DirectionalLightComponent::getColor() const
{
	return params.color;
}

float JFF::DirectionalLightComponent::getIntensity() const
{
	return params.intensity;
}
