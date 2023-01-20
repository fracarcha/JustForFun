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

#include "Component.h"

namespace JFF
{
	class TESTComponent2 : public Component
	{
	public:
		// Ctor & Dtor
		TESTComponent2(GameObject* const gameObject, const char* name, bool initiallyEnabled);
		virtual ~TESTComponent2();

		// Copy ctor and copy assignment
		TESTComponent2(const TESTComponent2& other) = delete;
		TESTComponent2& operator=(const TESTComponent2& other) = delete;

		// Move ctor and assignment
		TESTComponent2(TESTComponent2&& other) = delete;
		TESTComponent2 operator=(TESTComponent2&& other) = delete;

		virtual void onStart() override;
		virtual void onUpdate() override;
	};
}