/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "MeshComponent.h"

#include "Log.h"
#include "GameObject.h"

extern std::shared_ptr<JFF::MeshObject> createMeshObject(JFF::Engine* const engine, const std::shared_ptr<JFF::Mesh>& mesh);
extern std::shared_ptr<JFF::MeshObject> createMeshObject(JFF::Engine* const engine, const JFF::MeshObject::BasicMesh& predefinedShape);

JFF::MeshComponent::MeshComponent(GameObject* const gameObject, 
	const char* name, 
	bool initiallyEnabled, 
	const std::shared_ptr<Mesh>& mesh) :
	Component(gameObject, name, initiallyEnabled),
	mesh()
{
	JFF_LOG_INFO("Ctor MeshComponent")

	this->mesh = createMeshObject(gameObject->engine, mesh);
}

JFF::MeshComponent::MeshComponent(GameObject* const gameObject, 
	const char* name, 
	bool initiallyEnabled, 
	const MeshObject::BasicMesh& predefinedShape) :
	Component(gameObject, name, initiallyEnabled),
	mesh()
{
	JFF_LOG_INFO("Ctor MeshComponent")

	mesh = createMeshObject(gameObject->engine, predefinedShape);
}

JFF::MeshComponent::~MeshComponent()
{
	JFF_LOG_INFO("Dtor MeshComponent")
}

void JFF::MeshComponent::onStart()
{
	mesh->cook();
}

void JFF::MeshComponent::onDestroy() noexcept
{
	mesh.reset();
}

void JFF::MeshComponent::draw()
{
	mesh->draw();
}
