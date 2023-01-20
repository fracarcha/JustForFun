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

#include "LightComponent.h"

#include "Framebuffer.h"

namespace JFF
{
	class Engine;

	class DirectionalLightComponent : public LightComponent
	{
	public:
		struct Params
		{
			Params() :
				color(Vec3::WHITE),
				intensity(1.0f),

				castShadows(true),
				shadowMapWidth(4096),
				shadowMapHeight(4096),

				left(-10.0f),
				right(10.0f),
				bottom(-10.0f),
				top(10.0f),
				zNear(1.0f),
				zFar(20.0f)
			{}
			~Params() {}

			// Light params
			Vec3 color;
			float intensity;

			// Shadow casting
			bool castShadows;
			unsigned int shadowMapWidth, shadowMapHeight;

			// Shadow area (cube) of influence
			float left;
			float right;
			float bottom;
			float top;
			float zNear;
			float zFar;
		};

		// Ctor & Dtor
		DirectionalLightComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, Params params = Params());
		virtual ~DirectionalLightComponent();

		// Copy ctor and copy assignment
		DirectionalLightComponent(const DirectionalLightComponent& other) = delete;
		DirectionalLightComponent& operator=(const DirectionalLightComponent& other) = delete;

		// Move ctor and assignment
		DirectionalLightComponent(DirectionalLightComponent&& other) = delete;
		DirectionalLightComponent operator=(DirectionalLightComponent&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() override;
		//virtual void onEnable() override;
		//virtual void onUpdate() override;
		//virtual void onDisable() noexcept override;
		virtual void onDestroy() noexcept override;

		// ------------------------------- LIGHT COMPONENT OVERRIDES ------------------------------- //

		virtual void sendLightParams(RenderComponent* const renderComponent, int lightIndex) override;
		virtual void sendLightParams(RenderComponent* const renderComponent) override;

		virtual bool castShadows() const override { return params.castShadows; }
		virtual void enableShadowMapFramebuffer() override;
		virtual void disableShadowMapFramebuffer() override;
		virtual void getShadowMapSizePixels(unsigned int& outWidth, unsigned int& outHeight) const override;
		virtual void useMaterial() override;
		virtual void sendMat4(const char* variableName, const Mat4& matrix) override;
		virtual void sendVec3(const char* variableName, const Vec3& vec) override;
		virtual void sendFloat(const char* variableName, float f) override;
		virtual Mat4 getViewMatrix() const override;
		virtual Mat4 getProjectionMatrix() const override;

		// ------------------------------- DIRECTIONAL LIGHT COMPONENT INTERFACE ------------------------------- //

		virtual void setColor(Vec3 newColor);
		virtual void setColor(float red, float green, float blue);
		virtual void setIntensity(float newIntensity);
		virtual void setShadowImportanceVolume(float left, float right, float bottom, float top, float zNear, float zFar);

		virtual Vec3 getColor() const;
		virtual float getIntensity() const;
		virtual void getShadowImportanceVolume(float& outLeft, float& outRight, float& outBottom, float& outTop, float& outZNear, float& outZFar) const;

	protected:
		Engine* engine;

		// NOTE: Direction is given by GameObject's transform rotation. By default, light direction points down
		Params params;

		Mat4 shadowProjectionMatrix;
		std::shared_ptr<Framebuffer> shadowMapFBO;
		std::shared_ptr<Material> shadowCastMaterial;
	};
}