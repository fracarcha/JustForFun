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

#include <memory>

namespace JFF
{
	class GameObject;

	class Model
	{
	public:
		Model() {}
		virtual ~Model() {}

		// Copy ctor and copy assignment
		Model(const Model& other) = delete;
		Model& operator=(const Model& other) = delete;

		// Move ctor and assignment
		Model(Model&& other) = delete;
		Model operator=(Model&& other) = delete;

		// Model interface
		virtual std::weak_ptr<GameObject> getGameObject() const = 0;
	};
}