/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "GameObject.h"

#include "Log.h"

#include <algorithm>

JFF::GameObject::GameObject(
	Engine* const engine,
	const char* name, 
	const Vec3& localPosition, 
	const Vec3& localRotation, 
	const Vec3& localScale,
	bool initiallyEnabled) :
	engine(engine),
	name(name),
	transform(this, "Transform", /* Initially enabled */ true, localPosition, localRotation, localScale),
	enabled(initiallyEnabled),
	components(),
	parent()
{
	JFF_LOG_INFO("Ctor GameObject")
}

JFF::GameObject::~GameObject()
{
	JFF_LOG_INFO("Dtor GameObject")

	// Calls onDestroy() on all components before destruction
	std::for_each(components.begin(), components.end(), [](const auto& pair) { pair.second->destroy(); });
}

void JFF::GameObject::findParent()
{
	if (incomingEdges.empty())
		return;
	
	parent = getIncomingEdge(0).lock()->getSrcNode();
}

void JFF::GameObject::setEnabled(bool enabled, bool applyRecursively)
{
	// Enable/Disable this GameObject
	this->enabled = enabled;

	if (applyRecursively)
	{
		// Apply to this GameObject's components
		for (const auto& pair : components)
		{
			pair.second->setEnabled(enabled);
		
			/* 
			* When disabling a Component and its parent GameObject at the same time, the component doesn't have the oportunity
			* to execute to pass to a 'disabled' state. Next line solves that
			*/
			if (!enabled)
				pair.second->execute();
		}

		// Apply to child GameObjects
		auto visitor = [enabled, applyRecursively](const std::weak_ptr<EdgeBase<GameObject>>& edge) 
		{
			edge.lock()->getDstNode().lock()->setEnabled(enabled, applyRecursively);
		};
		visitOutcomingEdges(visitor);
	}
}

void JFF::GameObject::executeComponents()
{
	dispatchLoadComponents();	// Load all delay loaded components
	updateComponents();	// Update Components
}

inline void JFF::GameObject::dispatchLoadComponents()
{
	if (!delayLoadedComponents.empty())
	{
		std::for_each(delayLoadedComponents.begin(), delayLoadedComponents.end(), [](const std::function<void()>& fn) { fn(); });
		delayLoadedComponents.clear();
	}
}

inline void JFF::GameObject::updateComponents()
{
	std::for_each(components.begin(), components.end(), [](const auto& pair) { pair.second->execute(); });
}
