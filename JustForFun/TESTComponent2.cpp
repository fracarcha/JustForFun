/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "TESTComponent2.h"

#include "Log.h"
#include "Engine.h"

JFF::TESTComponent2::TESTComponent2(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
	Component(gameObject, name, initiallyEnabled)
{
	JFF_LOG_INFO("Ctor TESTComponent 2")
}

JFF::TESTComponent2::~TESTComponent2()
{
	JFF_LOG_INFO("Dtor TESTComponent 2")
}

void JFF::TESTComponent2::onStart()
{
}

void JFF::TESTComponent2::onUpdate()
{
	float deltaAngle = (float)gameObject->engine->time.lock()->deltaTime();
	gameObject->transform.addToLocalPitch(deltaAngle * 50.0f);
}

