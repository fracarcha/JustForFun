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

#include "TreeGraph.h"
#include "GameObject.h"

namespace JFF
{
	class Scene : public TreeGraph<GameObject, EdgeBase<GameObject>>
	{
	public:
		// Ctor & Dtor
		explicit Scene(Engine* const engine, const char* name);
		virtual ~Scene();

		// Copy ctor and copy assignment
		Scene(const Scene& other) = delete;
		Scene& operator=(const Scene& other) = delete;

		// Move ctor and assignment
		Scene(Scene&& other) = delete;
		Scene operator=(Scene&& other) = delete;

		// Attachs a new game object to root node
		virtual void add(const std::shared_ptr<GameObject>& newObject);
		
		/*
		* Adds a new game object to the scene and connects to parent. 
		* Parent game object must be part of the scene before this function call
		*/
		virtual void attach(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& newObject);

	protected:
		std::string name;
	};
}