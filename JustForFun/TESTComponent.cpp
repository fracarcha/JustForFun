/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "TESTComponent.h"

#include "Log.h"
#include "Engine.h"

JFF::TESTComponent::TESTComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) : 
	Component(gameObject, name, initiallyEnabled)
{
	JFF_LOG_INFO("Ctor TESTComponent")
}

JFF::TESTComponent::~TESTComponent()
{
	JFF_LOG_INFO("Dtor TESTComponent")
}

void JFF::TESTComponent::onStart()
{
}

void JFF::TESTComponent::onUpdate()
{
	float deltaAngle = (float) gameObject->engine->time.lock()->deltaTime();
	auto& tr = gameObject->transform;

	tr.addToLocalRotation(deltaAngle * 4.0f, deltaAngle * 3.0f, deltaAngle * 5.0f);
}

