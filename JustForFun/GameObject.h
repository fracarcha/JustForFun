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

#include "DirectedNodeBase.h"
#include "EdgeBase.h"
#include "TransformComponent.h" // Includes Component inside

#include <utility>

namespace JFF
{
	class Engine;

	class GameObject final : public DirectedNodeBase<EdgeBase<GameObject>>
	{
	public:
		// Ctor & Dtor
		explicit GameObject(
			Engine* const engine,
			const char* name, 
			const Vec3& localPosition = Vec3::ZERO, 
			const Vec3& localRotation = Vec3::ZERO, 
			const Vec3& localScale = Vec3::ONE,
			bool initiallyEnabled = true);
		virtual ~GameObject();

		// Copy ctor and copy assignment
		GameObject(const GameObject& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;

		// Move ctor and assignment
		GameObject(GameObject&& other) = delete;
		GameObject operator=(GameObject&& other) = delete;

		// Parent
		void findParent();

		// Name
		void setName(const std::string& name) { this->name = name; }
		std::string getName() const { return name; }

		// State machine
		void setEnabled(bool enabled, bool applyRecursively); // Can be applied recursively to child objects and their components
		bool isEnabled() const { return enabled; }
		void executeComponents();

		// Component management
		template<typename C, typename...Args> std::weak_ptr<C> addComponent(const char* componentName, bool initiallyEnabled, const Args&...args);
		template<typename C> std::weak_ptr<C> getComponent() const;
		template<typename C> std::weak_ptr<C> getComponent(const std::string& componentName);
		// TODO: Get all components of a class
		// TODO: Get components on children

	protected:
		inline void dispatchLoadComponents();
		inline void updateComponents();

	public:
		Engine* const engine;
		TransformComponent transform;		// Direct access to transform component
		std::weak_ptr<GameObject> parent;	// Direct access to parent GameObject

	protected:
		std::string name;
	
		// State machine attributes
		bool enabled;

		// List of components
		std::vector<std::pair<std::string, std::shared_ptr<Component>>> components;

		// Delay loaded lists
		std::vector<std::function<void()>> delayLoadedComponents;
	};
}

// Include inline definitions
#include "GameObject.inl"