/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "TESTCam.h"

#include "Log.h"
#include "Engine.h"

JFF::TESTCam::TESTCam(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
	Component(gameObject, name, initiallyEnabled)
{
	JFF_LOG_INFO("Ctor TESTCam")
}

JFF::TESTCam::~TESTCam()
{
	JFF_LOG_INFO("Dtor TESTCam")
}

void JFF::TESTCam::onStart()
{
}

void JFF::TESTCam::onUpdate()
{
	float deltaTime = (float)gameObject->engine->time.lock()->deltaTime();
	gameObject->transform.addToLocalYaw(deltaTime * 60.0f);
}
