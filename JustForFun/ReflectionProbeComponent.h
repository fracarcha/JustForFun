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
		//virtual void onEnable() override;
		//virtual void onUpdate() override;
		//virtual void onDisable() noexcept override;
		virtual void onDestroy() noexcept override;

		// ------------------------------- ENVIRONMENT MAP COMPONENT OVERRIDES ------------------------------- //

		// Send environment map (cubemap) to active shader
		virtual void sendEnvironmentMap(RenderComponent* const renderComponent) override;

	protected:
		std::string assetFilepath;
		std::shared_ptr<Cubemap> envMap;
	};
}