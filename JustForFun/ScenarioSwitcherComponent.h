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

#include "InputComponent.h"

#include <vector>
#include <memory>

namespace JFF
{
	class ScenarioSwitcherComponent : public InputComponent
	{
	public:
		// Ctor & Dtor
		ScenarioSwitcherComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled);
		virtual ~ScenarioSwitcherComponent();

		// Copy ctor and copy assignment
		ScenarioSwitcherComponent(const ScenarioSwitcherComponent& other) = delete;
		ScenarioSwitcherComponent& operator=(const ScenarioSwitcherComponent& other) = delete;

		// Move ctor and assignment
		ScenarioSwitcherComponent(ScenarioSwitcherComponent&& other) = delete;
		ScenarioSwitcherComponent operator=(ScenarioSwitcherComponent&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() override;
		//virtual void onEnable() override;
		//virtual void onUpdate() override;
		//virtual void onDisable() noexcept override;
		virtual void onDestroy() noexcept override;

	protected:
		std::vector<std::weak_ptr<GameObject>> skyboxes;
		std::vector<std::weak_ptr<GameObject>> models;

		unsigned int activeSkyboxIdx;
		unsigned int activeModelIdx;
	};
}