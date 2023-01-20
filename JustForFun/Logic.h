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

#include "ExecutableSubsystem.h"
#include "GameObject.h"

namespace JFF
{
	class Logic : public ExecutableSubsystem
	{
	public:
		// Ctor & Dtor
		Logic() {}
		virtual ~Logic() {}

		// Copy ctor and copy assignment
		Logic(const Logic& other) = delete;
		Logic& operator=(const Logic& other) = delete;

		// Move ctor and assignment
		Logic(Logic&& other) = delete;
		Logic operator=(Logic&& other) = delete;

		// ------------------------------------ Logic interface ------------------------------------ //
		
		/*
		* Creates a new empty scene.
		* This unloads any previously loaded scene.
		* The new scene is created when the next frame begins
		*/ 
		virtual void loadEmptyScene(const char* sceneName) = 0;
		// TODO: load scene from file

		/*
		* Creates an empty GameObject and adds it to scene.
		* The attachment will happen at the begining of the next frame
		*/
		virtual std::weak_ptr<GameObject> spawnGameObject(
			const char* name,
			const Vec3& localPosition = Vec3::ZERO,
			const Vec3& localRotation = Vec3::ZERO,
			const Vec3& localScale = Vec3::ONE,
			bool initiallyEnabled = true) = 0;

		/*
		* Creates an empty GameObject and attaches it to a given parent.
		* The parent must be part of the scene when the attachment happens.
		* The attachment will happen at the beginning of the next frame
		*/
		virtual std::weak_ptr<GameObject> spawnGameObject(
			const char* name,
			const std::weak_ptr<GameObject>& parent,
			const Vec3& localPosition = Vec3::ZERO,
			const Vec3& localRotation = Vec3::ZERO,
			const Vec3& localScale = Vec3::ONE,
			bool initiallyEnabled = true) = 0;
		
		/*
		* Change the GameObject state.
		* The state will effectively change when the next frame begins.
		* If you want to change the state immediately, use GameObject.setState() directly.
		* Set apply recursively to enable/disable al children GameObjects and their Components
		*/
		virtual void setGameObjectEnabled(const std::weak_ptr<GameObject>& obj, bool enabled, bool applyRecursively) = 0;

		/*
		* Find all objects that have a specified name.
		* The search will include disabled GameObjects.
		* If no GameObject was found, the list will be empty
		**/
		virtual std::vector<std::weak_ptr<GameObject>> findGameObjectsByName(const std::string& objName) const = 0;
	};
}