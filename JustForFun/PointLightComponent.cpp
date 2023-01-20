/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "PointLightComponent.h"

#include "Log.h"
#include "Engine.h"
#include "ShaderCodeBuilder.h"

#include <sstream>

extern std::shared_ptr<JFF::Framebuffer> createFramebuffer(JFF::Framebuffer::PrefabFramebuffer fboType,
	unsigned int width, unsigned int height, unsigned int samplesPerPixel = 0);

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);

JFF::PointLightComponent::PointLightComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, Params params) :
	LightComponent(gameObject, name, initiallyEnabled),
	engine(gameObject->engine),

	params(params),

	shadowProjectionMatrix(),
	viewMatrixRight(), 
	viewMatrixLeft(), 
	viewMatrixTop(), 
	viewMatrixBottom(), 
	viewMatrixNear(), 
	viewMatrixFar(),

	shadowCubemapFBO(),
	shadowCastMaterial()
{
	JFF_LOG_INFO("Ctor PointLightComponent")

	setPointLightImportanceVolume(params.zNear, params.zFar);
}

JFF::PointLightComponent::~PointLightComponent()
{
	JFF_LOG_INFO("Dtor PointLightComponent")
}

void JFF::PointLightComponent::onStart()
{
	// Create a shadowmap framebuffer if this light casts shadows
	if (params.castShadows)
	{
		shadowCubemapFBO = createFramebuffer(Framebuffer::PrefabFramebuffer::FBO_SHADOW_CUBEMAP, 
			params.shadowCubemapFaceWidth, params.shadowCubemapFaceHeight);
			
		shadowCastMaterial = createMaterial(engine, "Point light material");
		shadowCastMaterial->setDomain(Material::MaterialDomain::OMNIDIRECTIONAL_SHADOW_CAST);
		shadowCastMaterial->cook();
	}

	// Register the light in renderer
	gameObject->engine->renderer.lock()->addLight(this);
}

void JFF::PointLightComponent::onDestroy() noexcept
{
	// Unregister the light in Renderer
	gameObject->engine->renderer.lock()->removeLight(this);

	// Destroy framebuffer and material
	if (shadowCubemapFBO)
		shadowCubemapFBO->destroy();

	if (shadowCastMaterial)
		shadowCastMaterial->destroy();
}

void JFF::PointLightComponent::sendLightParams(RenderComponent* const renderComponent, int lightIndex)
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::POINT_LIGHT_POSITION;
	auto lightWorldPos = gameObject->transform.getModelMatrix() * Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightWorldPos.x, lightWorldPos.y, lightWorldPos.z));

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::POINT_LIGHT_COLOR;
	renderComponent->sendVec3(ss.str().c_str(), params.color);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::POINT_LIGHT_INTENSITY;
	renderComponent->sendFloat(ss.str().c_str(), params.intensity);
	
	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::POINT_LIGHT_LINEAR_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.linearAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::POINT_LIGHT_QUADRATIC_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.quadraticAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::POINT_LIGHT_CAST_SHADOWS;
	renderComponent->sendFloat(ss.str().c_str(), params.castShadows ? 1.0f : 0.0f);

	if (params.castShadows)
	{
		renderComponent->sendPointLightShadowCubemap(lightIndex, shadowCubemapFBO);

		ss.str(emptyString);
		ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << lightIndex << "]." << ShaderCodeBuilder::POINT_LIGHT_FAR_PLANE;
		renderComponent->sendFloat(ss.str().c_str(), params.zFar);
	}
	else
	{
		renderComponent->sendPointLightShadowCubemap(lightIndex); // Send empty shadow map
	}
}

void JFF::PointLightComponent::sendLightParams(RenderComponent* const renderComponent)
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_POSITION;
	auto lightWorldPos = gameObject->transform.getModelMatrix() * Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	renderComponent->sendVec3(ss.str().c_str(), Vec3(lightWorldPos.x, lightWorldPos.y, lightWorldPos.z));

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_COLOR;
	renderComponent->sendVec3(ss.str().c_str(), params.color);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_INTENSITY;
	renderComponent->sendFloat(ss.str().c_str(), params.intensity);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_LINEAR_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.linearAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_QUADRATIC_ATTENUATION_FACTOR;
	renderComponent->sendFloat(ss.str().c_str(), params.quadraticAttenuationFactor);

	ss.str(emptyString);
	ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_CAST_SHADOWS;
	renderComponent->sendFloat(ss.str().c_str(), params.castShadows ? 1.0f : 0.0f);

	if (params.castShadows)
	{
		renderComponent->sendPointLightShadowCubemap(0, shadowCubemapFBO);

		ss.str(emptyString);
		ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_FAR_PLANE;
		renderComponent->sendFloat(ss.str().c_str(), params.zFar);
	}
	else
	{
		renderComponent->sendPointLightShadowCubemap(0); // Send empty shadow map
	}
}

void JFF::PointLightComponent::enableShadowMapFramebuffer()
{
	shadowCubemapFBO->enable();
}

void JFF::PointLightComponent::disableShadowMapFramebuffer()
{
	shadowCubemapFBO->disable();
}

void JFF::PointLightComponent::getShadowMapSizePixels(unsigned int& outWidth, unsigned int& outHeight) const
{
	outWidth = params.shadowCubemapFaceWidth;
	outHeight = params.shadowCubemapFaceHeight;
}

void JFF::PointLightComponent::useMaterial()
{
	shadowCastMaterial->use();
}

void JFF::PointLightComponent::sendMat4(const char* variableName, const Mat4& matrix)
{
	shadowCastMaterial->sendMat4(variableName, matrix);
}

void JFF::PointLightComponent::sendVec3(const char* variableName, const Vec3& vec)
{
	shadowCastMaterial->sendVec3(variableName, vec);
}

void JFF::PointLightComponent::sendFloat(const char* variableName, float f)
{
	shadowCastMaterial->sendFloat(variableName, f);
}

JFF::Mat4 JFF::PointLightComponent::getViewMatrix() const
{
	JFF_LOG_WARNING("getViewMatrix() not implemented on PointLightComponent. Use sendCubemapViewMatrices() instead")
	return Mat4();
}

JFF::Mat4 JFF::PointLightComponent::getProjectionMatrix() const
{
	return shadowProjectionMatrix;
}

void JFF::PointLightComponent::setColor(Vec3 newColor)
{
	params.color = newColor;
}

void JFF::PointLightComponent::setColor(float red, float green, float blue)
{
	params.color.red = red;
	params.color.green = green;
	params.color.blue = blue;
}

void JFF::PointLightComponent::setIntensity(float newIntensity)
{
	params.intensity = newIntensity;
}

void JFF::PointLightComponent::setLinearAttenuationFactor(float newFactor)
{
	params.linearAttenuationFactor = newFactor;
}

void JFF::PointLightComponent::setQuadraticAttenuationFactor(float newFactor)
{
	params.quadraticAttenuationFactor = newFactor;
}

void JFF::PointLightComponent::setPointLightImportanceVolume(float zNear, float zFar)
{
	auto math = engine->math.lock();

	params.zNear = zNear;
	params.zFar = zFar;

	// --------------------------- BUILD VIEW MATRICES --------------------------- //

	/* 
	* NOTE: The UP vector (third parameter of lookAt()) may look strange since it doesn't look UP (0.0, 1.0, 0.0). This is due to the fact that 
	* positive Z face is what you whould look if you were inside the cubemap, that is, the back face of the cube.
	* For more info, check Cubemap class
	*/

	// Get world position of the light
	Vec3 lightPos = gameObject->transform.getWorldPos();

	viewMatrixRight		= math->lookAt(lightPos, lightPos + Vec3::RIGHT, Vec3::DOWN);
	viewMatrixLeft		= math->lookAt(lightPos, lightPos + Vec3::LEFT, Vec3::DOWN);
	viewMatrixTop		= math->lookAt(lightPos, lightPos + Vec3::UP, Vec3::BACKWARD);
	viewMatrixBottom	= math->lookAt(lightPos, lightPos + Vec3::DOWN, Vec3::FORWARD);
	viewMatrixNear		= math->lookAt(lightPos, lightPos + Vec3::BACKWARD, Vec3::DOWN);
	viewMatrixFar		= math->lookAt(lightPos, lightPos + Vec3::FORWARD, Vec3::DOWN);

	// --------------------------- BUILD PROJECTION MATRIX --------------------------- //

	float fovyRad = math->radians(90.0f); // 90 degrees takes exactly one face of the cubemap
	float aspect = (float)params.shadowCubemapFaceWidth / (float)params.shadowCubemapFaceHeight;

	shadowProjectionMatrix = math->perspective(fovyRad, aspect, zNear, zFar);
}

JFF::Vec3 JFF::PointLightComponent::getColor() const
{
	return params.color;
}

float JFF::PointLightComponent::getIntensity() const
{
	return params.intensity;
}

float JFF::PointLightComponent::getLinearAttenuationFactor() const
{
	return params.linearAttenuationFactor;
}

float JFF::PointLightComponent::getQuadraticAttenuationFactor() const
{
	return params.quadraticAttenuationFactor;
}

void JFF::PointLightComponent::getPointLightImportanceVolume(float& outZNear, float& outZFar) const
{
	outZNear = params.zNear;
	outZFar = params.zFar;
}

void JFF::PointLightComponent::sendCubemapViewMatrices()
{
	std::string emptyString;
	std::ostringstream ss(emptyString);

	// The order of layer:cubemap-face is: 0:right 1:left 2:top 3:bottom 4:near 5:far, so each layer must match its corresponding cubemap face
	Mat4 viewMatrices[] = { viewMatrixRight, viewMatrixLeft, viewMatrixTop, viewMatrixBottom, viewMatrixNear, viewMatrixFar };
	for (int layer = 0; layer < 6; ++layer)
	{
		ss.str(emptyString);
		ss << ShaderCodeBuilder::CUBEMAP_VIEW_MATRICES << "[" << layer << "]";
		sendMat4(ss.str().c_str(), viewMatrices[layer]);
	}
}
