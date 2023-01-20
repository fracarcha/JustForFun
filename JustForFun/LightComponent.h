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

#include "RenderComponent.h"

namespace JFF
{
	class LightComponent : public Component
	{
	public:
		// Ctor & Dtor
		LightComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
			Component(gameObject, name, initiallyEnabled)
		{}
		virtual ~LightComponent() {}

		// Copy ctor and copy assignment
		LightComponent(const LightComponent& other) = delete;
		LightComponent& operator=(const LightComponent& other) = delete;

		// Move ctor and assignment
		LightComponent(LightComponent&& other) = delete;
		LightComponent operator=(LightComponent&& other) = delete;

		// ------------------------------- LIGHT COMPONENT INTERFACE ------------------------------- //

		// Send light parameters as uniforms to renderComponent's material
		virtual void sendLightParams(RenderComponent* const renderComponent, int lightIndex) = 0;

		// Send light parameters as uniforms to renderComponent's material
		virtual void sendLightParams(RenderComponent* const renderComponent) = 0;

		// Returns true if this light casts shadows
		virtual bool castShadows() const = 0;

		// Use this light's framebuffer object to be target of shadow rendering
		virtual void enableShadowMapFramebuffer() = 0;
		virtual void disableShadowMapFramebuffer() = 0;

		// gets the size of the internal shadow map buffer if this light component casts shadows. Otherwise, this function returns Vec2::ZERO
		virtual void getShadowMapSizePixels(unsigned int& outWidth, unsigned int& outHeight) const = 0;

		// Enables the internal shader
		virtual void useMaterial() = 0;

		/*
		* Send a 4x4 matrix to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendMat4(const char* variableName, const Mat4& matrix) = 0;

		/*
		* Send a vec3 to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendVec3(const char* variableName, const Vec3& vec) = 0;

		/*
		* Send a float to active material and attachs it to the variable name.
		* The variable name must be a valid uniform included in material's shader code
		*/
		virtual void sendFloat(const char* variableName, float f) = 0;

		// Gets the view matrix of this light.
		virtual Mat4 getViewMatrix() const = 0;

		// Gets the projection matrix of this light.
		virtual Mat4 getProjectionMatrix() const = 0;
	};
}