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

#include "EnvironmentMapComponent.h"

#include "Cubemap.h"

namespace JFF
{
	class ReflectionProbeComponent : public EnvironmentMapComponent
	{
	public:
		// Ctor & Dtor
		ReflectionProbeComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, 
			const char* cubemapAssetFilepath);
		virtual ~ReflectionProbeComponent();

		// Copy ctor and copy assignment
		ReflectionProbeComponent(const ReflectionProbeComponent& other) = delete;
		ReflectionProbeComponent& operator=(const ReflectionProbeComponent& other) = delete;

		// Move ctor and assignment
		ReflectionProbeComponent(ReflectionProbeComponent&& other) = delete;
		ReflectionProbeComponent operator=(ReflectionProbeComponent&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() override;
		virtual void onEnable() override;
		//virtual void onUpdate() override;
		virtual void onDisable() noexcept override;
		virtual void onDestroy() noexcept override;

		// ------------------------------- ENVIRONMENT MAP COMPONENT OVERRIDES ------------------------------- //

		// Send environment map (cubemap) to active shader
		virtual void sendEnvironmentMap(RenderComponent* const renderComponent) override;

	private:
		inline void generateIrradianceMap();
		inline void generatePreFilteredEnvironmentMap();
		inline void generateBRDFIntegrationMap();

	protected:
		std::string assetFilepath;
		std::shared_ptr<Cubemap> envMap;
		
		const unsigned int irradianceMapFaceWidth;
		const unsigned int preFilteredMapFaceWidth;
		const unsigned int numPreFilteredMipmaps; // Check PBR shaders to ensure this variable matches with MAX_PREFILTERED_LOD_LEVELS
		const unsigned int BRDFIntegrationMapWidth;

		std::shared_ptr<Cubemap> irradianceMap;
		std::shared_ptr<Cubemap> preFilteredMap;
		std::shared_ptr<Texture> BRDFIntegrationMap;
	};
}