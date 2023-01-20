/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ScenarioSwitcherComponent.h"

#include "Engine.h"

JFF::ScenarioSwitcherComponent::ScenarioSwitcherComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
	InputComponent(gameObject, name, initiallyEnabled),

	skyboxes(),
	models(),

	activeSkyboxIdx(0u),
	activeModelIdx(0u)
{
	JFF_LOG_INFO("Ctor ScenarioSwitcherComponent")
}

JFF::ScenarioSwitcherComponent::~ScenarioSwitcherComponent()
{
	JFF_LOG_INFO("Dtor ScenarioSwitcherComponent")
}

void JFF::ScenarioSwitcherComponent::onStart()
{
	// ------------------------ FIND ALL OBJECTS NEEDED TO SWITCH BETWEEN THEM ------------------------ //

	auto logic = gameObject->engine->logic.lock();

	// Find and store models
	auto rifleList = logic->findGameObjectsByName("rifle");
	auto kasatkaList = logic->findGameObjectsByName("kasatka");
	auto cartoonCarList = logic->findGameObjectsByName("cartoon car");
	auto spartanList = logic->findGameObjectsByName("spartan");
	auto hoverCarList = logic->findGameObjectsByName("hover car");

	models.insert(models.end(), rifleList.begin(), rifleList.end());
	models.insert(models.end(), kasatkaList.begin(), kasatkaList.end());
	models.insert(models.end(), cartoonCarList.begin(), cartoonCarList.end());
	models.insert(models.end(), spartanList.begin(), spartanList.end());
	models.insert(models.end(), hoverCarList.begin(), hoverCarList.end());

	// Find and store skyboxes
	auto skyBeachList = logic->findGameObjectsByName("Skybox beach");
	auto skyFactoryList = logic->findGameObjectsByName("Skybox factory");
	auto skyPureList = logic->findGameObjectsByName("Skybox pure");
	auto skyForestList = logic->findGameObjectsByName("Skybox forest");
	auto skyChristmasList = logic->findGameObjectsByName("Skybox christmas");
	auto skyChristmas2List = logic->findGameObjectsByName("Skybox christmas2");
	auto skyBlaubeurenList = logic->findGameObjectsByName("Skybox blaubeuren");
	auto skyFireplaceList = logic->findGameObjectsByName("Skybox fireplace");
	auto skyMilkywayList = logic->findGameObjectsByName("Skybox milkyway");

	skyboxes.insert(skyboxes.end(), skyBeachList.begin(), skyBeachList.end());
	skyboxes.insert(skyboxes.end(), skyFactoryList.begin(), skyFactoryList.end());
	skyboxes.insert(skyboxes.end(), skyPureList.begin(), skyPureList.end());
	skyboxes.insert(skyboxes.end(), skyForestList.begin(), skyForestList.end());
	skyboxes.insert(skyboxes.end(), skyChristmasList.begin(), skyChristmasList.end());
	skyboxes.insert(skyboxes.end(), skyChristmas2List.begin(), skyChristmas2List.end());
	skyboxes.insert(skyboxes.end(), skyBlaubeurenList.begin(), skyBlaubeurenList.end());
	skyboxes.insert(skyboxes.end(), skyFireplaceList.begin(), skyFireplaceList.end());
	skyboxes.insert(skyboxes.end(), skyMilkywayList.begin(), skyMilkywayList.end());

	// Enable the first model and skybox
	logic->setGameObjectEnabled(models[activeModelIdx], true, /* apply recursively */ true);
	logic->setGameObjectEnabled(skyboxes[activeSkyboxIdx], true, /* apply recursively */ true);

	// -------------------------- SUBSCRIBE TO INPUT EVENTS -------------------------- //

	auto input = gameObject->engine->input.lock();

	Input::ButtonListener switchSkyboxListener = [this](const bool& value, Input::Hardware hw)
	{
		auto logic = gameObject->engine->logic.lock();

		// Disable the current model
		logic->setGameObjectEnabled(skyboxes[activeSkyboxIdx], false, /* apply recursively */ true);

		activeSkyboxIdx = ((size_t)activeSkyboxIdx + 1u) % skyboxes.size(); // The iterator loops when it reached the end of the list
		logic->setGameObjectEnabled(skyboxes[activeSkyboxIdx], true, /* apply recursively */ true);
	};
	input->addListener("default", "skyswitch", this, switchSkyboxListener);

	Input::ButtonListener switchModelListener = [this](const bool& value, Input::Hardware hw)
	{
		auto logic = gameObject->engine->logic.lock();

		// Disable the current model
		logic->setGameObjectEnabled(models[activeModelIdx], false, /* apply recursively */ true);

		// Enable the next model
		activeModelIdx = ((size_t)activeModelIdx + 1u) % models.size(); // The iterator loops when it reached the end of the list
		logic->setGameObjectEnabled(models[activeModelIdx], true, /* apply recursively */ true);
	};
	input->addListener("default", "modelswitch", this, switchModelListener);
}

void JFF::ScenarioSwitcherComponent::onDestroy() noexcept
{
	// Unsubscribe to input events
	auto input = gameObject->engine->input.lock();

	input->removeButtonListener("default", "skyswitch", this);
	input->removeButtonListener("default", "modelswitch", this);
}
