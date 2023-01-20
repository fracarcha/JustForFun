/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Component.h"

#include "Log.h"

JFF::Component::Component(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
	gameObject(gameObject),
	name(name),
	componentEnabledHint(initiallyEnabled),
	componentEnabled(false),
	executeFn(std::bind(&Component::initialize, this))
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor Component")
}

JFF::Component::~Component()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor Component")
}

void JFF::Component::setEnabled(bool enabled)
{
	componentEnabledHint = enabled;
}

bool JFF::Component::isEnabled() const
{
	return componentEnabled;
}

void JFF::Component::execute()
{
	executeFn();
}

void JFF::Component::destroy() noexcept
{
	if (componentEnabled)
		disable();

	onDestroy();
}

void JFF::Component::initialize()
{
	onStart();
	if (componentEnabledHint)
	{
		enable();
	}
	else
	{
		disable();
	}
}

void JFF::Component::enable()
{
	componentEnabled = true;
	onEnable();
	updateEnabled();
	executeFn = std::bind(&Component::updateEnabled, this);
}

void JFF::Component::updateEnabled()
{
	if (componentEnabledHint)
	{
		onUpdate();
	}
	else
	{
		disable();
	}
}

void JFF::Component::disable()
{
	componentEnabled = false;
	onDisable();
	executeFn = std::bind(&Component::updateDisabled, this);
}

void JFF::Component::updateDisabled()
{
	if (componentEnabledHint)
		enable();
}