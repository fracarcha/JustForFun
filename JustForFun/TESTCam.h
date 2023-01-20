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
	class TESTCam : public Component
	{
	public:
		// Ctor & Dtor
		TESTCam(GameObject* const gameObject, const char* name, bool initiallyEnabled);
		virtual ~TESTCam();

		// Copy ctor and copy assignment
		TESTCam(const TESTCam& other) = delete;
		TESTCam& operator=(const TESTCam& other) = delete;

		// Move ctor and assignment
		TESTCam(TESTCam&& other) = delete;
		TESTCam operator=(TESTCam&& other) = delete;

		// ------------------------------- COMPONENT OVERRIDES ------------------------------- //

		virtual void onStart() override;
		//virtual void onEnable() override;
		virtual void onUpdate() override;
		//virtual void onDisable() noexcept override;
		//virtual void onDestroy() noexcept override;

		// ------------------------------- TESTCam OVERRIDES ------------------------------- //

		// TODO: add your functions here

	protected:
		// TODO: add your member variables and types here
	};
}