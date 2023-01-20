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

#include "MeshComponent.h"
#include "CameraComponent.h"

#include <type_traits>
#include <typeinfo>

// ----------------------------------- GENERIC TEMPLATE DEFINITIONS ----------------------------------- //

template<typename C, typename ...Args>
inline std::weak_ptr<C> JFF::GameObject::addComponent(const char* componentName, bool initiallyEnabled, const Args & ...args)
{
	static_assert(std::is_base_of_v<Component, C>, "Added component must inherit from Component");

	// Create the Component
	auto comp = std::make_shared<C>(this, componentName, initiallyEnabled, args...);

	// Create a delay loaded function to add the Component to component list
	auto delayLoadedComponentLambda = [this](const std::shared_ptr<C>& comp)
	{
		std::string componentName = typeid(C).name();
		components.push_back(std::pair<std::string, std::shared_ptr<Component>>(componentName, comp));
	};
	auto delayLoadedComponentFn = std::bind(delayLoadedComponentLambda, comp);
	delayLoadedComponents.push_back(delayLoadedComponentFn);

	return comp;
}

template<typename C>
inline std::weak_ptr<C> JFF::GameObject::getComponent() const
{
	static_assert(std::is_base_of_v<Component, C>, "Requested component must inherit from Component");

	auto iter = std::find_if(components.begin(), components.end(), [](const auto& comp)
		{
			std::string componentName = typeid(C).name();
			return componentName == comp.first;
		});

	if (iter != components.end())
	{
		std::shared_ptr<C> component = std::dynamic_pointer_cast<C>(iter->second); // If cast doesn't succeed, the shared ptr is empty
		return component;
	}
	else
	{
		return std::weak_ptr<C>();
	}
}

template<typename C>
inline std::weak_ptr<C> JFF::GameObject::getComponent(const std::string& componentName)
{
	static_assert(std::is_base_of_v<Component, C>, "Requested component must inherit from Component");

	auto iter = std::find_if(components.begin(), components.end(), [&componentName](const auto& comp)
		{
			return componentName == comp.second->getName();
		});
	
	if (iter != components.end())
	{
		std::shared_ptr<C> component = std::dynamic_pointer_cast<C>(iter->second); // If cast doesn't succeed, the shared ptr is empty
		return component; 
 	}
	else
	{
		return std::weak_ptr<C>();
	}
}

// ----------------------------------- TEMPLATE SPECIALIZATIONS ----------------------------------- //

extern std::shared_ptr<JFF::CameraComponent> createCameraComponent(
	JFF::GameObject* const gameObject,
	const char* name,
	bool initiallyEnabled,
	bool activeCameraOnStart);

template<>
inline std::weak_ptr<JFF::CameraComponent> JFF::GameObject::addComponent(
	const char* componentName,
	bool initiallyEnabled,
	const bool& activeCameraOnStart)
{
	// Create the Component
	auto comp = createCameraComponent(this, componentName, initiallyEnabled, activeCameraOnStart);

	// Create a delay loaded function to add the Component to component list
	auto delayLoadedComponentLambda = [this](const std::shared_ptr<CameraComponent>& comp)
	{
		std::string componentName = typeid(CameraComponent).name();
		components.push_back(std::pair<std::string, std::shared_ptr<Component>>(componentName, comp));
	};
	auto delayLoadedComponentFn = std::bind(delayLoadedComponentLambda, comp);
	delayLoadedComponents.push_back(delayLoadedComponentFn);

	return comp;
}