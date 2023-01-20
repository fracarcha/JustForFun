/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

//#include <Windows.h>

//#define GLEW_STATIC // Used when linked against GLEW static library
//#include "GL/glew.h"
//#include "GLFW/glfw3.h"

//#include "GLM/glm.hpp"
//#include "GLM/gtc/matrix_transform.hpp"
//#include "GLM/gtc/type_ptr.hpp"

//#include "GLShader.h"
//#include "GLTexture.h"
//#include "GLCamera2.h"
//#include "File.h"
//#include "Time.h"

//#include <iostream>

#include "Engine.h"

// TODO: testing
//#include "Vec.h"
//#include "Mat.h"
//#include "Log.h"
//#include "DFSAlgorithm.h"
//#include "Node.h"
//#include "DirectedNode.h"
//#include "SimpleGraph.h"
//#include "TreeGraph.h"
//#include "Scene.h"
#include "GameObject.h"
#include "MeshRenderComponent.h"
#include "PostProcessRenderComponent.h"
#include "TESTComponent.h"
#include "TESTComponent2.h"
#include "TESTCam.h"
#include "CameraComponent.h"
#include "FlyCamInputComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ReflectionProbeComponent.h"

#include "ScenarioSwitcherComponent.h"

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);
extern std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const char* name, const char* assetFilePath);

// Recurrent tasks:
// TODO: Review exceptions
// TODO: Review constness
// TODO: Review inlines
// TODO: Review redundant copy-constructor calls
// TODO: Review used containers (vector, map, ...) and check if they are suitable for their task
// TODO: Review memory leaks
// TODO: Review explicit constructors
// TODO: Remove unused includes
// TODO: Check '#pragma once' and '#define *_H' macros per interface. The last one is important for targeting the engine on non Windows platforms
// TODO: Relocate includes to avoid ill-formed std classes (like typeid)
// TODO: Review virtual functions (specially destructors)
// TODO: Add copyright notice to all source files
// TODO: Test code against integrated GPUs
// TODO: Common shader mistakes: 
//		* Not normalizing directions 
//		* Not operate on the same space (model, world, view, clip)
//		* Directions having w component other than zero
//		* Multiply a direction vector by model matrix or other matrix that scales or translates
//		* Projection matrix's fovy is passed as degrees instead of radians
// 
// Concrete tasks:
// TODO: Deferred shading + PBR
// TODO: Visual artifact detected in irradiance cubemap (probably due to TBN matrix??)
// TODO: Test PBR SPECULAR workflow
int main()
{
	// TODO: Real engine 
	// =================================================================

	using namespace JFF;

	// Create engine
	Engine engine;

	// TODO: Attach subsystems here
	//engine.attachSubsystem<JFF::Time>(std::make_shared<JFF::TimeSTD>());

	// Init essential subsystems
	engine.initBasicSubsystems();

	// Execute post load phase
	engine.postLoadSubsystems();

	// TODO: Configure subsystems here
	auto logic = engine.logic.lock();
	logic->loadEmptyScene("Main scene");
	
	// --------------------------------- PRE-BUILT OBJECTS ---------------------------------  //

	//std::weak_ptr<GameObject> floor = logic->spawnGameObject("Floor", Vec3(0.0f, -3.0f, 0.0f), Vec3::ZERO, Vec3(50.0f, 1.0f, 50.0f));
	//{
	//	auto floorHandler = floor.lock();
	//	floorHandler->addComponent<MeshComponent>("Mesh", true, MeshObject::BasicMesh::CUBE);
	//	floorHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Concrete.mat.ini");
	//}

	//std::weak_ptr<GameObject> cubeBrick = logic->spawnGameObject("Obj 1", Vec3(-5.0f, 0.0f, -2.0f), Vec3(0.0f, 40.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));
	//{
	//	auto obj1Handler = cubeBrick.lock();
	//	obj1Handler->addComponent<MeshComponent>("Mesh", true, MeshObject::BasicMesh::CUBE);
	//	obj1Handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/BrickWall.mat.ini");
	//	//obj1Handler->addComponent<TESTComponent>("Test Component", true);
	//	//obj1Handler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Materials/Material_Debug.mat.ini");
	//}
	//std::weak_ptr<GameObject> cubeBrickBump = logic->spawnGameObject("Obj 1", Vec3(-3.0f, -1.0f, -4.0f), Vec3(0.0f, 40.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));
	//{
	//	auto obj1Handler = cubeBrickBump.lock();
	//	obj1Handler->addComponent<MeshComponent>("Mesh", true, MeshObject::BasicMesh::CUBE);
	//	obj1Handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/BrickWallBump.mat.ini");
	//	//obj1Handler->addComponent<TESTComponent>("Test Component", true);
	//	//obj1Handler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Materials/Material_Debug.mat.ini");
	//}
	//std::weak_ptr<GameObject> sphereBrick = logic->spawnGameObject("Sph 1", Vec3(1.0f, 3.0f, 1.0f));
	//{
	//	auto sphHandler = sphereBrick.lock();
	//	sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/BrickWall.mat.ini");
	//	sphHandler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Materials/Material_Debug.mat.ini");
	//}
	//std::weak_ptr<GameObject> glassSphere = logic->spawnGameObject("Sph 2", Vec3(1.0f, 1.0f, -5.0f));
	//{
	//	auto sphHandler = glassSphere.lock();
	//	sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Glass.mat.ini");
	//	//sphHandler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Materials/Material_Debug.mat.ini");
	//}
	//std::weak_ptr<GameObject> sphereGouraud = logic->spawnGameObject("Sphere Gouraud", Vec3(-2.0f, 1.0f, -4.0f));
	//{
	//	auto sphHandler = sphereGouraud.lock();
	//	sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Gouraud.mat.ini");
	//	//sphHandler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Materials/Material_Debug.mat.ini");
	//}
	//std::weak_ptr<GameObject> blinnPhongSphere = logic->spawnGameObject("Blinn Phong sphere", Vec3(-3.0f, 0.0f, 0.0f));
	//{
	//	auto sphHandler = blinnPhongSphere.lock();
	//	sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/SimpleBlinnPhong.mat.ini");
	//}
	//std::weak_ptr<GameObject> unlitSphere = logic->spawnGameObject("Unlit sphere", Vec3(3.0f, 0.0f, 0.0f));
	//{
	//	auto sphHandler = unlitSphere.lock();
	//	sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/SimpleUnlit.mat.ini");
	//}

	//std::weak_ptr<GameObject> sphereRustedMetalPBR = logic->spawnGameObject("Sphere rusted metal PBR", Vec3(0.0f, 0.0f, 1.0f));
	//{
	//	auto sphereHandler = sphereRustedMetalPBR.lock();
	//	sphereHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphereHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/PBR_RustedIron.mat.ini");
	//}

	//std::weak_ptr<GameObject> spherePBRGlossy = logic->spawnGameObject("Sphere PBR glossy", Vec3(-2.0f, 0.0f, -1.0f));
	//{
	//	auto sphereHandler = spherePBRGlossy.lock();
	//	sphereHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphereHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/PBR_simple_glossy.mat.ini");
	//}

	//std::weak_ptr<GameObject> spherePBRGlossySpec = logic->spawnGameObject("Sphere PBR glossy", Vec3(-2.0f, 0.0f, 1.0f));
	//{
	//	auto sphereHandler = spherePBRGlossySpec.lock();
	//	sphereHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphereHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/PBR_simple_glossy_specular_workflow.mat.ini");
	//}

	//std::weak_ptr<GameObject> spherePBRPlastic = logic->spawnGameObject("Sphere PBR plastic", Vec3(0.0f, 0.0f, -3.0f));
	//{
	//	auto sphereHandler = spherePBRPlastic.lock();
	//	sphereHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphereHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/PBR_simple_plastic.mat.ini");
	//}

	//std::weak_ptr<GameObject> spherePBRPolishMetal = logic->spawnGameObject("Sphere PBR polish metal", Vec3(-2.0f, -1.0f, -4.0f));
	//{
	//	auto sphereHandler = spherePBRPolishMetal.lock();
	//	sphereHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphereHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/PBR_simple_polish_metal.mat.ini");
	//}

	//std::weak_ptr<GameObject> spherePBRRoughMetal = logic->spawnGameObject("Sphere PBR rough metal", Vec3(0.0f, -1.0f, -4.0f));
	//{
	//	auto sphereHandler = spherePBRRoughMetal.lock();
	//	sphereHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	sphereHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/PBR_simple_rough_metal.mat.ini");
	//}

	// --------------------------------- SKYBOXES ---------------------------------  //

	// NOTE: Skyboxes are enabled in ScenarioSwitcherComponent
	bool skyboxEnabled = false;

	//std::weak_ptr<GameObject> skyBeach = logic->spawnGameObject("Skybox beach", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	//{
	//	auto handler = skyBeach.lock();
	//	handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Beach.mat.ini");
	//	handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Beach.cube.ini");
	//}
	std::weak_ptr<GameObject> skyFactory = logic->spawnGameObject("Skybox factory", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyFactory.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Factory.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Factory.cube.ini");
	}
	//std::weak_ptr<GameObject> skyHouse = logic->spawnGameObject("Skybox house", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	//{
	//	auto handler = skyHouse.lock();
	//	handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
	//	handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_House.mat.ini");
	//	//handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/House.cube.ini");
	//}
	std::weak_ptr<GameObject> skyPure = logic->spawnGameObject("Skybox pure", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyPure.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_PureSky.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/PureSky.cube.ini");
	}
	/*std::weak_ptr<GameObject> skyForest = logic->spawnGameObject("Skybox forest", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyForest.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Rainforest.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Rainforest.cube.ini");
	}
	std::weak_ptr<GameObject> skyChristmas = logic->spawnGameObject("Skybox christmas", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyChristmas.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Christmas.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Christmas.cube.ini");
	}
	std::weak_ptr<GameObject> skyChristmas2 = logic->spawnGameObject("Skybox christmas2", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyChristmas2.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Christmas2.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Christmas2.cube.ini");
	}
	std::weak_ptr<GameObject> skyBlaubeuren = logic->spawnGameObject("Skybox blaubeuren", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyBlaubeuren.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Blaubeuren.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Blaubeuren.cube.ini");
	}
	std::weak_ptr<GameObject> skyFireplace = logic->spawnGameObject("Skybox fireplace", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyFireplace.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Fireplace.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Fireplace.cube.ini");
	}
	std::weak_ptr<GameObject> skyMilkyway = logic->spawnGameObject("Skybox milkyway", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f), skyboxEnabled);
	{
		auto handler = skyMilkyway.lock();
		handler->addComponent<MeshComponent>("Sphere mesh", true, MeshObject::BasicMesh::SPHERE);
		handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Materials/Skybox_Milkyway.mat.ini");
		handler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Skyboxes/Milkyway.cube.ini");
	}*/

	// --------------------------------- 3D MODELS ---------------------------------  //

	//std::shared_ptr<Model> backpackModel = engine.io.lock()->loadModel("Models/Backpack/backpack.3d.ini");
	//std::weak_ptr<GameObject> backpack = backpackModel->getGameObject();
	//{
	//	auto backpackHandler = backpack.lock();
	//	backpackHandler->setEnabled(true);
	//	backpackHandler->addComponent<TESTComponent>("Test Component", true);
	//}

	std::shared_ptr<Model> rifleModel = engine.io.lock()->loadModel("Models/Rifle/rifle.3d.ini");
	std::weak_ptr<GameObject> rifle = rifleModel->getGameObject();
	{
		auto handler = rifle.lock();
		handler->setName("rifle");
		handler->transform.setLocalYaw(90.0f);
		handler->transform.setLocalScale(0.8f, 0.8f, 0.8f);
		handler->transform.setLocalPos(0.5f, 0.0f, 0.0f);
		//handler->setEnabled(true); // Enable/disable is controller by ScenarioSwitcherComponent
		//cameraHandler->addComponent<TESTComponent>("Test Component", true);
	}

	//std::shared_ptr<Model> kasatkaModel = engine.io.lock()->loadModel("Models/Kasatka/kasatka.3d.ini");
	//std::weak_ptr<GameObject> kasatka = kasatkaModel->getGameObject();
	//{
	//	auto handler = kasatka.lock();
	//	handler->setName("kasatka");
	//	handler->transform.setLocalScale(1.9f, 1.9f, 1.9f);
	//	//handler->setEnabled(true); // Enable/disable is controller by ScenarioSwitcherComponent
	//	//cameraHandler->addComponent<TESTComponent>("Test Component", true);
	//}

	std::shared_ptr<Model> cartoonCarModel = engine.io.lock()->loadModel("Models/CartoonCar/cartoon_car.3d.ini");
	std::weak_ptr<GameObject> cartoonCar = cartoonCarModel->getGameObject();
	{
		auto handler = cartoonCar.lock();
		handler->setName("cartoon car");
		handler->transform.setLocalScale(0.007f, 0.007f, 0.007f);
		handler->transform.setLocalPos(0.0f, -1.0f, 0.0f);
		//handler->setEnabled(true); // Enable/disable is controller by ScenarioSwitcherComponent
		//cameraHandler->addComponent<TESTComponent>("Test Component", true);
	}

	//std::shared_ptr<Model> spartanModel = engine.io.lock()->loadModel("Models/Spartan/spartan.3d.ini");
	//std::weak_ptr<GameObject> spartan = spartanModel->getGameObject();
	//{
	//	auto handler = spartan.lock();
	//	handler->setName("spartan");
	//	handler->transform.setLocalScale(100.0f, 100.0f, 100.0f); // Tiny figure!
	//	handler->transform.setLocalPos(0.0f, -6.5f, 0.0f);
	//	//handler->setEnabled(true); // Enable/disable is controller by ScenarioSwitcherComponent
	//	//cameraHandler->addComponent<TESTComponent>("Test Component", true);
	//}

	std::shared_ptr<Model> hoverCarModel = engine.io.lock()->loadModel("Models/HoverCar/hover_car.3d.ini");
	std::weak_ptr<GameObject> hoverCar = hoverCarModel->getGameObject();
	{
		auto handler = hoverCar.lock();
		handler->setName("hover car");
		handler->transform.setLocalScale(0.3f, 0.3f, 0.3f);
		//handler->setEnabled(true); // Enable/disable is controller by ScenarioSwitcherComponent
		//cameraHandler->addComponent<TESTComponent>("Test Component", true);
	}

	// --------------------------------- CAMERAS ---------------------------------  //

	//std::weak_ptr<GameObject> camBase = logic->spawnGameObject("cam Base", Vec3::ZERO, Vec3(0.0f, 0.0f, 0.0f));
	//{
	//	auto camBaseHandler = camBase.lock();
	//	camBaseHandler->addComponent<TESTCam>("Test cam", true);
	//}
	std::weak_ptr<GameObject> cam = logic->spawnGameObject("Cam", Vec3(0.0f, 0.0f, 5.0f));
	{
		auto camHandler = cam.lock();
		auto context = engine.context.lock();

		// Camera component
		auto camComp = camHandler->addComponent<CameraComponent>("Cam component", true, true);

		int fbSizeWidth, fbSizeHeight;
		context->getFramebufferSizeInPixels(fbSizeWidth, fbSizeHeight);
		float aspectRatio = (float)fbSizeWidth / (float)fbSizeHeight;

		camComp.lock()->setPerspectiveProjection(45.0f, aspectRatio, 0.1f, 1000.0f);
		context->addOnFramebufferSizeChangedListener([&camComp](int width, int height) // TODO: Remember to remove this listener from Context
			{
				if (width == 0 || height == 0) // Ignore request to invalid sizes
					return;

				camComp.lock()->setPerspectiveProjection(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
			});
		//camComp.lock()->setOrthographicProjection(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 20.0f);

		// Camera input
		//auto input = engine.input.lock();
		//input->setEnabled(true); // Enable input
		//input->setCursorMode(JFF::Input::CursorMode::DISABLED);
		camHandler->addComponent<FlyCamInputComponent>("Input cam", true);

		// Component to switch between skyboxes and models
		camHandler->addComponent<ScenarioSwitcherComponent>("Scenario switcher", true);
	}

	// --------------------------------- LIGHTS ---------------------------------  //

	//std::weak_ptr<GameObject> dirLight1 = logic->spawnGameObject("Dir light 1", Vec3(0.0f, 6.0f, 0.0f), Vec3(40.0f, 0.0f, 0.0f));
	//{
	//	auto dirLightHandler = dirLight1.lock();

	//	DirectionalLightComponent::Params params;
	//	params.intensity = 3.0f;
	//	//params.color = Vec3(0.0f, 1.0f, 0.0f);
	//	auto dirLightComp = dirLightHandler->addComponent<DirectionalLightComponent>("Dir light comp", true, params);
	//}

	//for (int i = 0; i < 10; ++i)
	//{
	//	float randX = ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f; // Random in range [-1,1]
	//	float randY = ((float)std::rand() / RAND_MAX) + 1.0f;
	//	float randZ = ((float)std::rand() / RAND_MAX) * 2.0f - 1.0f;

	//	float randR = ((float)std::rand() / RAND_MAX) * 0.7f;
	//	float randG = ((float)std::rand() / RAND_MAX) * 0.7f;
	//	float randB = ((float)std::rand() / RAND_MAX) * 0.7f;

	//	std::weak_ptr<GameObject> pointLight1 = logic->spawnGameObject("Point light 1", Vec3(randX * 5.0f, randY * 5.0f, randZ * 5.0f));
	//	{
	//		auto pointLightHandler = pointLight1.lock();

	//		PointLightComponent::Params params;
	//		params.color = Vec3(randR, randG, randB);
	//		params.castShadows = false;
	//		//params.intensity = 10.0f;
	//		auto pointLightComp = pointLightHandler->addComponent<PointLightComponent>("Point light comp", true, params);
	//	}
	//}

	//std::weak_ptr<GameObject> pointLight1 = logic->spawnGameObject("Point light 1", Vec3(-3.0f, 1.0f, -2.0f));
	//{
	//	auto pointLightHandler = pointLight1.lock();

	//	PointLightComponent::Params params;
	//	params.color = Vec3::GREEN;
	//	params.intensity = 10.0f;
	//	params.castShadows = true;
	//	auto pointLightComp = pointLightHandler->addComponent<PointLightComponent>("Point light comp", true, params);
	//}

	//std::weak_ptr<GameObject> pointLight2 = logic->spawnGameObject("Point light 2", Vec3(3.0f, 1.0f, -2.0f));
	//{
	//	auto pointLightHandler = pointLight2.lock();

	//	PointLightComponent::Params params;
	//	params.color = Vec3::BLUE;
	//	params.intensity = 10.0f;
	//	params.castShadows = false;
	//	auto pointLightComp = pointLightHandler->addComponent<PointLightComponent>("Point light comp", true, params);
	//}

	//std::weak_ptr<GameObject> pointLight3 = logic->spawnGameObject("Point light 3", Vec3(0.0f, 1.0f, 2.0f));
	//{
	//	auto pointLightHandler = pointLight3.lock();

	//	PointLightComponent::Params params;
	//	params.color = Vec3::RED;
	//	params.intensity = 2.0f;
	//	params.castShadows = false;
	//	auto pointLightComp = pointLightHandler->addComponent<PointLightComponent>("Point light comp", true, params);
	//}

	//std::weak_ptr<GameObject> spotLight1 = logic->spawnGameObject("Spot light 1", Vec3(0.0f, 4.0f, 4.0f), Vec3(40.0f, 0.0f, 0.0f));
	//{
	//	auto spotLightHandler = spotLight1.lock();

	//	SpotLightComponent::Params params;
	//	//params.intensity = 10.0f;
	//	params.intensity = 10.0f;
	//	//params.color = Vec3(1.0f, 0.5f, 0.2f);
	//	params.color = Vec3(1.0f);
	//	params.linearAttenuationFactor = 0.045f;
	//	params.quadraticAttenuationFactor = 0.0075f;
	//	params.innerHalfAngleDegrees = 20.0f;
	//	params.outerHalfAngleDegrees = 40.0f;
	//	auto spotLightComp = spotLightHandler->addComponent<SpotLightComponent>("Spot light comp", true, params);
	//}

	// Execute main loop
	engine.mainLoop();

	return 0;
}