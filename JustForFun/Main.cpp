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

#include "assimp/Importer.hpp"

extern std::shared_ptr<JFF::Material> createMaterial(JFF::Engine* const engine, const char* name);

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
// TODO: Common shader mistakes: 
//		* Not normalizing directions 
//		* Not operate on the same space (model, world, view, clip)
//		* Directions having w component other than zero
//		* Multiply a direction vector by model matrix or other matrix that scales or translates
//		* Projection matrix's fovy is passed as degrees instead of radians
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
	engine.renderer.lock()->setRenderPath(Renderer::RenderPath::DEFERRED); // TODO: remove ############
	engine.postLoadSubsystems();

	// TODO: Configure subsystems here
	auto logic = engine.logic.lock();
	logic->loadEmptyScene("Main scene");
	
	// --------------------------------- OBJECTS ---------------------------------  //

	std::weak_ptr<GameObject> floor = logic->spawnGameObject("Floor", Vec3(0.0f, -3.0f, 0.0f), Vec3::ZERO, Vec3(50.0f, 1.0f, 50.0f));
	{
		auto floorHandler = floor.lock();
		floorHandler->addComponent<MeshComponent>("Mesh", true, MeshComponent::BasicMesh::CUBE);
		floorHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Concrete.mat.ini");
	}

	std::weak_ptr<GameObject> cubeBrick = logic->spawnGameObject("Obj 1", Vec3(-5.0f, 0.0f, -2.0f), Vec3(0.0f, 40.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));
	{
		auto obj1Handler = cubeBrick.lock();
		obj1Handler->addComponent<MeshComponent>("Mesh", true, MeshComponent::BasicMesh::CUBE);
		obj1Handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "BrickWall.mat.ini");
		//obj1Handler->addComponent<TESTComponent>("Test Component", true);
		//obj1Handler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Material_Debug.mat.ini");
	}
	std::weak_ptr<GameObject> cubeBrickBump = logic->spawnGameObject("Obj 1", Vec3(-3.0f, -1.0f, -4.0f), Vec3(0.0f, 40.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f));
	{
		auto obj1Handler = cubeBrickBump.lock();
		obj1Handler->addComponent<MeshComponent>("Mesh", true, MeshComponent::BasicMesh::CUBE);
		obj1Handler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "BrickWallBump.mat.ini");
		//obj1Handler->addComponent<TESTComponent>("Test Component", true);
		//obj1Handler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Material_Debug.mat.ini");
	}
	std::weak_ptr<GameObject> sphereBrick = logic->spawnGameObject("Sph 1", Vec3(1.0f, 3.0f, 1.0f));
	{
		auto sphHandler = sphereBrick.lock();
		sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshComponent::BasicMesh::SPHERE);
		sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "BrickWall.mat.ini");
		sphHandler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Material_Debug.mat.ini");
	}
	std::weak_ptr<GameObject> glassSphere = logic->spawnGameObject("Sph 2", Vec3(1.0f, 1.0f, -5.0f));
	{
		auto sphHandler = glassSphere.lock();
		sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshComponent::BasicMesh::SPHERE);
		sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Glass.mat.ini");
		//sphHandler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Material_Debug.mat.ini");
	}
	std::weak_ptr<GameObject> sphereGouraud = logic->spawnGameObject("Sphere Gouraud", Vec3(-2.0f, 1.0f, -4.0f));
	{
		auto sphHandler = sphereGouraud.lock();
		sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshComponent::BasicMesh::SPHERE);
		sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Gouraud.mat.ini");
		//sphHandler->addComponent<MeshRenderComponent>("Debug Renderer", true, "Material_Debug.mat.ini");
	}
	std::weak_ptr<GameObject> blinnPhongSphere = logic->spawnGameObject("Blinn Phong sphere", Vec3(-3.0f, 0.0f, 0.0f));
	{
		auto sphHandler = blinnPhongSphere.lock();
		sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshComponent::BasicMesh::SPHERE);
		sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "SimpleBlinnPhong.mat.ini");
	}
	std::weak_ptr<GameObject> unlitSphere = logic->spawnGameObject("Unlit sphere", Vec3(3.0f, 0.0f, 0.0f));
	{
		auto sphHandler = unlitSphere.lock();
		sphHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshComponent::BasicMesh::SPHERE);
		sphHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "SimpleUnlit.mat.ini");
	}
	std::weak_ptr<GameObject> skySphere = logic->spawnGameObject("Sky", Vec3::ZERO, Vec3::ZERO, Vec3(2000.0f, 2000.0f, 2000.0f));
	{
		auto skySphereHandler = skySphere.lock();
		skySphereHandler->addComponent<MeshComponent>("Sphere mesh", true, MeshComponent::BasicMesh::SPHERE);
		skySphereHandler->addComponent<MeshRenderComponent>("Mesh Renderer", true, "Material_sky.mat.ini");
		skySphereHandler->addComponent<ReflectionProbeComponent>("Reflection probe cubemap", true, "Park.cube.ini");
	}

	std::shared_ptr<Model> backpackModel = engine.io.lock()->loadModel("backpack.3d.ini");
	std::weak_ptr<GameObject> backpack = backpackModel->getGameObject();
	{
		auto backpackHandler = backpack.lock();
		backpackHandler->setEnabled(true);
		backpackHandler->addComponent<TESTComponent>("Test Component", true);
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
				camComp.lock()->setPerspectiveProjection(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
			});
		//camComp.lock()->setOrthographicProjection(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 20.0f);

		// Camera input
		auto input = engine.input.lock();
		input->setEnabled(true); // Enable input
		input->setCursorMode(JFF::Input::CursorMode::DISABLED);
		camHandler->addComponent<FlyCamInputComponent>("Input cam", true);
	}

	// --------------------------------- LIGHTS ---------------------------------  //

	//std::weak_ptr<GameObject> dirLight1 = logic->spawnGameObject("Dir light 1", Vec3(0.0f, 6.0f, 0.0f), Vec3(40.0f, 0.0f, 0.0f));
	//{
	//	auto dirLightHandler = dirLight1.lock();

	//	DirectionalLightComponent::Params params;
	//	params.intensity = 1.0f;
	//	//params.color = Vec3(0.0f, 1.0f, 0.0f);
	//	auto dirLightComp = dirLightHandler->addComponent<DirectionalLightComponent>("Dir light comp", true, params);
	//}
	//std::weak_ptr<GameObject> dirLight2 = logic->spawnGameObject("Dir light 2", Vec3(4.0f, 6.0f, 0.0f), Vec3(40.0f, 40.0f, 0.0f));
	//{
	//	auto dirLightHandler = dirLight2.lock();
	//	auto dirLightComp = dirLightHandler->addComponent<DirectionalLightComponent>("Dir light comp", true, Vec3::BLUE, 1.0f, /* Cast shadows */ true, 4096, 4096);
	//}
	//std::weak_ptr<GameObject> dirLight3 = logic->spawnGameObject("Dir light 3", Vec3(-4.0f, 6.0f, 0.0f), Vec3(40.0f, -40.0f, 0.0f));
	//{
	//	auto dirLightHandler = dirLight3.lock();
	//	auto dirLightComp = dirLightHandler->addComponent<DirectionalLightComponent>("Dir light comp", true, Vec3::RED, 1.0f, /* Cast shadows */ true, 4096, 4096);
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
	//	params.castShadows = true;
	//	auto pointLightComp = pointLightHandler->addComponent<PointLightComponent>("Point light comp", true, params);
	//}

	//std::weak_ptr<GameObject> pointLight2 = logic->spawnGameObject("Point light 2", Vec3(3.0f, 1.0f, -2.0f));
	//{
	//	auto pointLightHandler = pointLight2.lock();

	//	PointLightComponent::Params params;
	//	params.color = Vec3::BLUE;
	//	params.castShadows = true;
	//	auto pointLightComp = pointLightHandler->addComponent<PointLightComponent>("Point light comp", true, params);
	//}

	//std::weak_ptr<GameObject> pointLight3 = logic->spawnGameObject("Point light 3", Vec3(0.0f, 1.0f, 2.0f));
	//{
	//	auto pointLightHandler = pointLight3.lock();

	//	PointLightComponent::Params params;
	//	params.color = Vec3::RED;
	//	params.castShadows = true;
	//	auto pointLightComp = pointLightHandler->addComponent<PointLightComponent>("Point light comp", true, params);
	//}

	std::weak_ptr<GameObject> spotLight1 = logic->spawnGameObject("Spot light 1", Vec3(0.0f, 3.0f, 3.0f), Vec3(40.0f, 0.0f, 0.0f));
	{
		auto spotLightHandler = spotLight1.lock();

		SpotLightComponent::Params params;
		params.intensity = 10.0f;
		params.color = Vec3(1.0f, 0.5f, 0.2f);
		params.linearAttenuationFactor = 0.045f;
		params.quadraticAttenuationFactor = 0.0075f;
		params.innerHalfAngleDegrees = 20.0f;
		params.outerHalfAngleDegrees = 40.0f;
		auto spotLightComp = spotLightHandler->addComponent<SpotLightComponent>("Spot light comp", true, params);
	}


	// Execute main loop
	engine.mainLoop();

	return 0;
}