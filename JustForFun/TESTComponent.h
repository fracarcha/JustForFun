#pragma once

#include "Component.h"

namespace JFF
{
	class TESTComponent : public Component
	{
	public:
		// Ctor & Dtor
		TESTComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled);
		virtual ~TESTComponent();

		// Copy ctor and copy assignment
		TESTComponent(const TESTComponent& other) = delete;
		TESTComponent& operator=(const TESTComponent& other) = delete;

		// Move ctor and assignment
		TESTComponent(TESTComponent&& other) = delete;
		TESTComponent operator=(TESTComponent&& other) = delete;

		virtual void onStart() override;
		virtual void onUpdate() override;
	};
}