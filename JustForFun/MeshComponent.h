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
#include "MeshObject.h"

namespace JFF
{
	class MeshComponent : public Component
	{
	public:
		// Ctor & Dtor
		MeshComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, const std::shared_ptr<Mesh>& mesh);
		MeshComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled, const MeshObject::BasicMesh& predefinedShape);

		MeshComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
			Component(gameObject, name, initiallyEnabled)
		{}
		virtual ~MeshComponent();

		// Copy ctor and copy assignment
		MeshComponent(const MeshComponent& other) = delete;
		MeshComponent& operator=(const MeshComponent& other) = delete;

		// Move ctor and assignment
		MeshComponent(MeshComponent&& other) = delete;
		MeshComponent operator=(MeshComponent&& other) = delete;

		// ----------------------------- COMPONENT OVERRIDES ----------------------------- //

		virtual void onStart() override;
		virtual void onDestroy() noexcept override;

		// ----------------------------- MESH FUNCTIONS ----------------------------- //
		
		// Enables the GPU buffer where the vertex data of this mesh is stored and execute a draw call
		virtual void draw();

	protected:
		std::shared_ptr<JFF::MeshObject> mesh;
	};
}