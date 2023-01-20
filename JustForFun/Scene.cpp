/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Scene.h"

#include "Log.h"
#include "Engine.h"

#include "MeshRenderComponent.h"
#include "PostProcessRenderComponent.h"

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);

JFF::Scene::Scene(Engine* const engine, const char* name) :
	name(name)
{
	JFF_LOG_INFO("Ctor Scene")

	auto renderer = engine->renderer.lock();

	// Create the root node
	auto rootNodeObj = std::make_shared<GameObject>(engine, "root");
	
	// Add a plane mesh to be used as post process surface
	rootNodeObj->addComponent<MeshComponent>("Root render-to_screen plane mesh", true, MeshObject::BasicMesh::PLANE);

	// If render path is deferred shading, some lighting render components (used to render lights separately) are needed 
	if (renderer->getRenderPath() == Renderer::RenderPath::DEFERRED)
	{
		auto dirLightDeferredMaterial = createMaterial(engine, "Directional lighting deferred material");
		dirLightDeferredMaterial->setDomain(Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED);
		dirLightDeferredMaterial->cook();
		rootNodeObj->addComponent<MeshRenderComponent>("Directional lighting deferred render component", true, dirLightDeferredMaterial);

		auto pointLightDeferredMaterial = createMaterial(engine, "Point lighting deferred material");
		pointLightDeferredMaterial->setDomain(Material::MaterialDomain::POINT_LIGHTING_DEFERRED);
		pointLightDeferredMaterial->cook();
		rootNodeObj->addComponent<MeshRenderComponent>("Point lighting deferred render component", true, pointLightDeferredMaterial);

		auto spotLightDeferredMaterial = createMaterial(engine, "Spot lighting deferred material");
		spotLightDeferredMaterial->setDomain(Material::MaterialDomain::SPOT_LIGHTING_DEFERRED);
		spotLightDeferredMaterial->cook();
		rootNodeObj->addComponent<MeshRenderComponent>("Spot lighting deferred render component", true, spotLightDeferredMaterial);

		auto envLightDeferredMaterial = createMaterial(engine, "Environment lighting deferred material");
		envLightDeferredMaterial->setDomain(Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED);
		envLightDeferredMaterial->cook();
		rootNodeObj->addComponent<MeshRenderComponent>("Environment lighting deferred render component", true, envLightDeferredMaterial);
	
		auto emissiveDeferredMaterial = createMaterial(engine, "Emissive lighting deferred material");
		emissiveDeferredMaterial->setDomain(Material::MaterialDomain::EMISSIVE_LIGHTING_DEFERRED);
		emissiveDeferredMaterial->cook();
		rootNodeObj->addComponent<MeshRenderComponent>("Emissive lighting deferred render component", true, emissiveDeferredMaterial);
	}

	// Add post processing render component
	rootNodeObj->addComponent<PostProcessRenderComponent>("Post process render component", true, "Materials/PostProcess.mat.ini");

	// Add render-to-screen render component
	auto rootMaterial = createMaterial(engine, "Root Material");
	rootMaterial->setDomain(Material::MaterialDomain::RENDER_TO_SCREEN);
	rootMaterial->cook();
	rootNodeObj->addComponent<MeshRenderComponent>("Root render-to-screen mesh Renderer", true, rootMaterial);

	// Add the root node to this scene
	addNode(rootNodeObj);
}

JFF::Scene::~Scene()
{
	JFF_LOG_INFO("Dtor Scene")
}

void JFF::Scene::add(const std::shared_ptr<GameObject>& newObject)
{
	addNodeConnected(rootNode.lock(), newObject);
	newObject->findParent();
}

void JFF::Scene::attach(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& newObject)
{
	addNodeConnected(parent, newObject);
	newObject->findParent();
}
