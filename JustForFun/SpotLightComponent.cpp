/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "SpotLightComponent.h"

#include "Log.h"
#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include <sstream>

extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(JFF::Framebuffer::PrefabFramebuffer fboType,
	unsigned int width, unsigned int height, unsigned int samplesPerPixel = 0);

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);

JFF::SpotLightComponent::SpotLightComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, 
	SpotLightComponent::Params params) :
	LightComponent(gameObject, name, initiallyEnabled),
	engine(gameObject->engine),

	params(params),

	innerHalfAngleCutoff(0.0f),
	outerHalfAngleCutoff(0.0f),

	shadowProjectionMatrix(),
	shadowMapFBO(),
	shadowCastMaterial()
{
	JFF_LOG_INFO("Ctor SpotLightComponent")

	setSpotLightImportanceVolume(params.innerHalfAngleDegrees, params.outerHalfAngleDegrees, params.zNear, params.zFar);
}

JFF::SpotLightComponent::~SpotLightComponent()
{
	JFF_LOG_INFO("Dtor SpotLightComponent")
}

void JFF::SpotLightComponent::onStart()
{
	// Create a shadowmap framebuffer if this light casts shadows
	if (params.castShadows)
	{
		shadowMapFBO = createFramebuffer(Framebuffer::PrefabFramebuffer::FBO_SHADOW_MAP, 
			params.shadowMapWidth, params.shadowMapHeight);

		shadowCastMaterial = createMaterial(engine, "Spot light material");
		shadowCastMaterial->setDomain(Material::MaterialDomain::SHADOW_CAST);
		shadowCastMaterial->cook();
	}

	// Register the light in renderer
	gameObject->engine->renderer.lock()->addLight(this);
}

void JFF::SpotLightComponent::onDestroy() noexcept
{
	// Unregister the light in Renderer
	gameObject->engine->renderer.lock()->removeLight(this);

	// Destroy framebuffer and material
	if (shadowMapFBO)
		shadowMapFBO->destroy();

	if (shadowCastMaterial)
		shadowCastMaterial->destroy();
}

void JFF::SpotLightComponent::sendLightParams(RenderComponent* const renderComponent, int lightIndex)
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_POSITION;
	auto lightWorldPos = gameObject->transform.getModelMatrix() * Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightWorldPos.x, lightWorldPos.y, lightWorldPos.z));

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_DIRECTION;
	auto lightDir = gameObject->transform.getRotationMatrix() * Vec4::DOWN;
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightDir.x, lightDir.y, lightDir.z));

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_COLOR;
	renderComponent->sendVec3(ss.str().c_str(), params.color);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_INTENSITY;
	renderComponent->sendFloat(ss.str().c_str(), params.intensity);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_LINEAR_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.linearAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_QUADRATIC_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.quadraticAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_INNER_HALF_ANGLE_CUTOFF;
	renderComponent->sendFloat(ss.str().c_str(), innerHalfAngleCutoff);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_OUTER_HALF_ANGLE_CUTOFF;
	renderComponent->sendFloat(ss.str().c_str(), outerHalfAngleCutoff);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::SPOT_LIGHT_CAST_SHADOWS;
	renderComponent->sendFloat(ss.str().c_str(), params.castShadows ? 1.0f : 0.0f);

	if (params.castShadows)
	{
		renderComponent->sendSpotLightShadowMap(lightIndex, shadowMapFBO);

		ss.str(emptyString);
		ss << ShaderCodeBuilder::SPOT_LIGHT_MATRICES << "[" << lightIndex << "]";
		renderComponent->sendMat4(ss.str().c_str(), getProjectionMatrix() * getViewMatrix());
	}
	else
	{
		renderComponent->sendSpotLightShadowMap(lightIndex); // Send empty shadow map
	}
}

void JFF::SpotLightComponent::sendLightParams(RenderComponent* const renderComponent)
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_POSITION;
	auto lightWorldPos = gameObject->transform.getModelMatrix() * Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightWorldPos.x, lightWorldPos.y, lightWorldPos.z));

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_DIRECTION;
	auto lightDir = gameObject->transform.getRotationMatrix() * Vec4::DOWN;
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightDir.x, lightDir.y, lightDir.z));

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_COLOR;
	renderComponent->sendVec3(ss.str().c_str(), params.color);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_INTENSITY;
	renderComponent->sendFloat(ss.str().c_str(), params.intensity);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_LINEAR_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.linearAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_QUADRATIC_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.quadraticAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_INNER_HALF_ANGLE_CUTOFF;
	renderComponent->sendFloat(ss.str().c_str(), innerHalfAngleCutoff);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_OUTER_HALF_ANGLE_CUTOFF;
	renderComponent->sendFloat(ss.str().c_str(), outerHalfAngleCutoff);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_CAST_SHADOWS;
	renderComponent->sendFloat(ss.str().c_str(), params.castShadows ? 1.0f : 0.0f);

	if (params.castShadows)
	{
		renderComponent->sendSpotLightShadowMap(0, shadowMapFBO);

		ss.str(emptyString);
		ss << ShaderCodeBuilder::SPOT_LIGHT_MATRIX;
		renderComponent->sendMat4(ss.str().c_str(), getProjectionMatrix() * getViewMatrix());
	}
	else
	{
		renderComponent->sendSpotLightShadowMap(0); // Send empty shadow map
	}
}

void JFF::SpotLightComponent::enableShadowMapFramebuffer()
{
	shadowMapFBO->enable();
}

void JFF::SpotLightComponent::disableShadowMapFramebuffer()
{
	shadowMapFBO->disable();
}

void JFF::SpotLightComponent::getShadowMapSizePixels(unsigned int& outWidth, unsigned int& outHeight) const
{
	outWidth = params.shadowMapWidth;
	outHeight = params.shadowMapHeight;
}

void JFF::SpotLightComponent::useMaterial()
{
	shadowCastMaterial->use();
}

void JFF::SpotLightComponent::sendMat4(const char* variableName, const Mat4& matrix)
{
	shadowCastMaterial->sendMat4(variableName, matrix);
}

void JFF::SpotLightComponent::sendVec3(const char* variableName, const Vec3& vec) 
{
	shadowCastMaterial->sendVec3(variableName, vec);
}

void JFF::SpotLightComponent::sendFloat(const char* variableName, float f)
{
	shadowCastMaterial->sendFloat(variableName, f);
}

JFF::Mat4 JFF::SpotLightComponent::getViewMatrix() const
{
	// TODO: Don't calculate this every frame. Link this to transform's changes

	// Get world position of the light
	Vec3 lightPos = gameObject->transform.getWorldPos();

	// Get world rotation
	Vec4 lightDir4 = gameObject->transform.getRotationMatrix() * Vec4::DOWN;
	Vec3 lightDir(lightDir4.pitch, lightDir4.yaw, lightDir4.roll);

	return gameObject->engine->math.lock()->lookAt(lightPos, lightPos + lightDir, Vec3::UP);
}

JFF::Mat4 JFF::SpotLightComponent::getProjectionMatrix() const
{
	return shadowProjectionMatrix;
}

void JFF::SpotLightComponent::setColor(Vec3 newColor)
{
	params.color = newColor;
}

void JFF::SpotLightComponent::setColor(float red, float green, float blue)
{
	params.color.red = red;
	params.color.green = green;
	params.color.blue = blue;
}

void JFF::SpotLightComponent::setIntensity(float newIntensity)
{
	params.intensity = newIntensity;
}

void JFF::SpotLightComponent::setLinearAttenuationFactor(float newFactor)
{
	params.linearAttenuationFactor = newFactor;
}

void JFF::SpotLightComponent::setQuadraticAttenuationFactor(float newFactor)
{
	params.quadraticAttenuationFactor = newFactor;
}

JFF::Vec3 JFF::SpotLightComponent::getColor() const
{
	return params.color;
}

float JFF::SpotLightComponent::getIntensity() const
{
	return params.intensity;
}

void JFF::SpotLightComponent::setSpotLightImportanceVolume(float innerHalfAngleDegrees, float outerHalfAngleDegrees, float zNear, float zFar)
{
	auto math = gameObject->engine->math.lock();

	params.innerHalfAngleDegrees = innerHalfAngleDegrees;
	params.outerHalfAngleDegrees = outerHalfAngleDegrees;
	params.zNear = zNear;
	params.zFar = zFar;

	outerHalfAngleCutoff = math->cos(math->radians(outerHalfAngleDegrees));
	innerHalfAngleCutoff = math->cos(math->radians(innerHalfAngleDegrees));

	float fovyRad = math->radians(params.outerHalfAngleDegrees * 2.0f); // Double the angle of outerHalfAngleDegrees
	float aspect = (float)params.shadowMapWidth / (float)params.shadowMapHeight;

	shadowProjectionMatrix = math->perspective(fovyRad, aspect, zNear, zFar);
}

float JFF::SpotLightComponent::getLinearAttenuationFactor() const
{
	return params.linearAttenuationFactor;
}

float JFF::SpotLightComponent::getQuadraticAttenuationFactor() const
{
	return params.quadraticAttenuationFactor;
}

void JFF::SpotLightComponent::getSpotLightImportanceVolume(float& outInnerHalfAngleDegrees, float& outOuterHalfAngleDegrees, float& outZNear, float& outZFar) const
{
	outInnerHalfAngleDegrees = params.innerHalfAngleDegrees;
	outOuterHalfAngleDegrees = params.outerHalfAngleDegrees;
	outZNear = params.zNear;
	outZFar = params.zFar;
}
