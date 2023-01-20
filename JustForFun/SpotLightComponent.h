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

	class SpotLightComponent : public LightComponent
	{
	public:
		struct Params
		{
			Params() :
				color(Vec3::WHITE),
				intensity(1.0f),

				linearAttenuationFactor(0.09f),
				quadraticAttenuationFactor(0.032f),
				innerHalfAngleDegrees(10.0f),
				outerHalfAngleDegrees(20.0f),

				castShadows(true),
				shadowMapWidth(4096),
				shadowMapHeight(4096),

				zNear(0.01f),
				zFar(100.0f)
			{}
			~Params() {}

			// Light params
			Vec3 color;
			float intensity;

			// Light cone area
			float linearAttenuationFactor;
			float quadraticAttenuationFactor;
			float innerHalfAngleDegrees; 
			float outerHalfAngleDegrees;

			// Shadow casting
			bool castShadows;
			unsigned int shadowMapWidth, shadowMapHeight;

			// Shadow area (pyramidal frustum) of influence
			// NOTE: Amplitude (FOV) of frustum is determined by outerHalfAngleDegrees
			float zNear;
			float zFar;
		};

		// Ctor & Dtor
		SpotLightComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, Params params = Params());
		virtual ~SpotLightComponent();

		// Copy ctor and copy assignment
		SpotLightComponent(const SpotLightComponent& other) = delete;
		SpotLightComponent& operator=(const SpotLightComponent& other) = delete;

		// Move ctor and assignment
		SpotLightComponent(SpotLightComponent&& other) = delete;
		SpotLightComponent operator=(SpotLightComponent&& other) = delete;

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

		// ------------------------------- SPOT LIGHT COMPONENT OVERRIDES ------------------------------- //

		virtual void setColor(Vec3 newColor);
		virtual void setColor(float red, float green, float blue);
		virtual void setIntensity(float newIntensity);
		virtual void setLinearAttenuationFactor(float newFactor);
		virtual void setQuadraticAttenuationFactor(float newFactor);
		virtual void setSpotLightImportanceVolume(float innerHalfAngleDegrees, float outerHalfAngleDegrees, float zNear, float zFar);

		virtual Vec3 getColor() const;
		virtual float getIntensity() const;
		virtual float getLinearAttenuationFactor() const;
		virtual float getQuadraticAttenuationFactor() const;
		virtual void getSpotLightImportanceVolume(float& outInnerHalfAngleDegrees, float& outOuterHalfAngleDegrees, float& outZNear, float& outZFar) const;

	protected:
		Engine* engine;

		// NOTE: Position is given by GameObject's transform position.
		// NOTE: Direction is given by GameObject's transform rotation. By default, light direction points down
		Params params;

		float outerHalfAngleCutoff; // Cosine of th half angle of the spot light cone shape. We use cosine to compare this value with the result of dot products
		float innerHalfAngleCutoff;
		
		Mat4 shadowProjectionMatrix;
		std::shared_ptr<Framebuffer> shadowMapFBO;
		std::shared_ptr<Material> shadowCastMaterial;
	};
}