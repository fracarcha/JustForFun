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

	class PointLightComponent : public LightComponent
	{
	public:
		struct Params
		{
			Params() :
				color(Vec3::WHITE),
				intensity(1.0f),

				linearAttenuationFactor(0.09f),
				quadraticAttenuationFactor(0.032f),

				castShadows(true),
				shadowCubemapFaceWidth(4096),
				shadowCubemapFaceHeight(4096),

				zNear(1.0f),
				zFar(100.0f)
			{}
			~Params() {}

			// Light params
			Vec3 color;
			float intensity;

			// Light attenuation area
			float linearAttenuationFactor;
			float quadraticAttenuationFactor;

			// Shadow casting
			bool castShadows;
			unsigned int shadowCubemapFaceWidth, shadowCubemapFaceHeight; // Width and height of each cubemap face

			// Shadow area (pyramidal frustum) of influence per cubemap face
			// NOTE: Amplitude (FOV) of frustum is determined automatically by cubemap shape
			float zNear;
			float zFar;
		};

		// Ctor & Dtor
		PointLightComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, Params params = Params());
		virtual ~PointLightComponent();

		// Copy ctor and copy assignment
		PointLightComponent(const PointLightComponent& other) = delete;
		PointLightComponent& operator=(const PointLightComponent& other) = delete;

		// Move ctor and assignment
		PointLightComponent(PointLightComponent&& other) = delete;
		PointLightComponent operator=(PointLightComponent&& other) = delete;

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

		// ------------------------------- POINT LIGHT COMPONENT INTERFACE ------------------------------- //

		virtual void setColor(Vec3 newColor);
		virtual void setColor(float red, float green, float blue);
		virtual void setIntensity(float newIntensity);
		virtual void setLinearAttenuationFactor(float newFactor);
		virtual void setQuadraticAttenuationFactor(float newFactor);
		virtual void setPointLightImportanceVolume(float zNear, float zFar);

		virtual Vec3 getColor() const;
		virtual float getIntensity() const;
		virtual float getLinearAttenuationFactor() const;
		virtual float getQuadraticAttenuationFactor() const;
		virtual void getPointLightImportanceVolume(float& outZNear, float& outZFar) const;

		virtual void sendCubemapViewMatrices();

	protected:
		Engine* engine;

		// NOTE: Position is given by GameObject's transform position.
		Params params;

		Mat4 shadowProjectionMatrix;
		Mat4 viewMatrixRight, viewMatrixLeft, viewMatrixTop, viewMatrixBottom, viewMatrixNear, viewMatrixFar;

		std::shared_ptr<Framebuffer> shadowCubemapFBO;
		std::shared_ptr<Material> shadowCastMaterial;
	};
}