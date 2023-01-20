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

#include <string>
#include <functional>

namespace JFF
{
	class GameObject;

	class Component
	{
	public:
		// Ctor & dtor
		explicit Component(GameObject* const gameObject, const char* name, bool initiallyEnabled);
		virtual ~Component();

		// Copy ctor and copy assignment
		Component(const Component& other) = delete;
		Component& operator=(const Component& other) = delete;

		// Move ctor and assignment
		Component(Component&& other) = delete;
		Component operator=(Component&& other) = delete;

		// ----------------------------- NON OVERRIDABLE FUNCTIONS ----------------------------- //

		void setEnabled(bool enabled);
		bool isEnabled() const;

		void setName(const char* name) { this->name = name; }
		std::string getName() const { return name; }

		void execute();
		void destroy() noexcept;

		// ----------------------------- OVERRIDABLE FUNCTIONS ----------------------------- //

		virtual void onStart() = 0;			 // Programmable on children (Mandatory)
		virtual void onEnable() {}			 // Programmable on children (Optional)
		virtual void onUpdate() {}			 // Programmable on children (Optional)
		virtual void onDisable() noexcept {} // Programmable on children (Optional)
		virtual void onDestroy() noexcept {} // Programmable on children (Optional)

	private: // State functions
		void initialize();
		void enable();
		void updateEnabled();
		void disable();
		void updateDisabled();

	public:
		GameObject* const gameObject;

	protected:
		std::string name;

		// State member
		bool componentEnabledHint; // This is only a hint. The real state is stored in 'componentEnabled' member
		bool componentEnabled;
		std::function<void()> executeFn;
	};
}