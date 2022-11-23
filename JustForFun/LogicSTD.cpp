#include "LogicSTD.h"

#include "Log.h"
#include "DFSAlgorithm.h"

JFF::LogicSTD::LogicSTD() : 
	engine(nullptr),
	activeScene(nullptr)
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: LogicSTD")

	auto updateGameObjects = [](const std::weak_ptr<GameObject>& gameObj) { gameObj.lock()->executeComponents(); };
	updateGameObjectsAlgorithm = std::make_shared<DFSAlgorithm<Scene, GameObject, EdgeBase<GameObject>>>(updateGameObjects);
}

JFF::LogicSTD::~LogicSTD()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: LogicSTD")
}

void JFF::LogicSTD::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: LogicSTD")
}

void JFF::LogicSTD::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: LogicSTD")
	this->engine = engine;
}

JFF::Subsystem::UnloadOrder JFF::LogicSTD::getUnloadOrder() const
{
	return UnloadOrder::LOGIC;
}

JFF::ExecutableSubsystem::ExecutionOrder JFF::LogicSTD::getExecutionOrder() const
{
	return ExecutableSubsystem::ExecutionOrder::LOGIC;
}

bool JFF::LogicSTD::execute()
{
	// TODO: Auto-load from file a predefined scene

	dispatchLoadSceneRequests();			// Accept requests to unload old scene and load a new scene
	autoLoadSceneIfEmpty();					// Auto-load an empty scene if it isn't loaded yet

	// TODO: Accept requests to unload GameObjects

	dispatchSpawnGameObjectRequests();		// Accept requests of spawn GameObjects
	dispatchSetGameObjectStateRequests();	// Accept requests of set GameObject state

	updateGameObjects();					// Update gameobjects

	return true;
}

void JFF::LogicSTD::loadEmptyScene(const char* sceneName)
{
	// Create a delay loaded function that will load a new scene at the begining of next call to execute()
	auto createSceneLambda = [this](const char* scName) 
	{
		// Create a new scene. This will delete old scene and all GameObjects attached to it
		activeScene = std::make_shared<Scene>(engine, scName);
	};
	auto createSceneFn = std::bind(createSceneLambda, sceneName);
	delayLoadedScenes.push_back(createSceneFn);
}

std::weak_ptr<JFF::GameObject> JFF::LogicSTD::spawnGameObject(
	const char* name, 
	const Vec3& localPosition, 
	const Vec3& localRotation,
	const Vec3& localScale,
	bool initiallyEnabled)
{
	// Create the GameObject
	std::shared_ptr<GameObject> obj = std::make_shared<GameObject>(engine, name, localPosition, localRotation, localScale, initiallyEnabled);

	// Add it to active scene
	auto spawnGameObjectLambda = [this](const std::shared_ptr<GameObject>& obj)
	{
		if (!activeScene.get())	// Ensure a scene is loaded
		{
			JFF_LOG_WARNING("Cannot spawn a GameObject. There isn't an active scene loaded")
			return;
		}

		activeScene->add(obj); // Adds the created object to the scene
	};
	auto spawnGameObjectFn = std::bind(spawnGameObjectLambda, obj);
	delayLoadedGameObjects.push_back(spawnGameObjectFn);

	// Return a weak ptr of created object
	return obj;
}

std::weak_ptr<JFF::GameObject> JFF::LogicSTD::spawnGameObject(
	const char* name, 
	const std::weak_ptr<GameObject>& parent,
	const Vec3& localPosition, 
	const Vec3& localRotation, 
	const Vec3& localScale,
	bool initiallyEnabled)
{
	// Create the GameObject
	std::shared_ptr<GameObject> obj = std::make_shared<GameObject>(engine, name, localPosition, localRotation, localScale, initiallyEnabled);

	// Attach it to provided parent in active scene
	auto spawnGameObjectLambda = [this](const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& obj)
	{
		if (!activeScene.get()) // Ensure a scene is loaded
		{
			JFF_LOG_WARNING("Cannot spawn a GameObject. There isn't an active scene loaded")
			return;
		}

		activeScene->attach(parent, obj); // Attaches the created object to the parent
	};
	auto spawnGameObjectFn = std::bind(spawnGameObjectLambda, parent.lock(), obj);
	delayLoadedGameObjects.push_back(spawnGameObjectFn);

	// Return a weak ptr of created object
	return obj;
}

void JFF::LogicSTD::setGameObjectEnabled(const std::weak_ptr<GameObject>& obj, bool enabled)
{
	auto setStateLambda = [](const std::shared_ptr<GameObject>& obj, bool enabled)
	{
		obj->setEnabled(enabled);
	};
	auto setStateFn = std::bind(setStateLambda, obj.lock(), enabled);
	delaySetStateGameObjects.push_back(setStateFn);
}

inline void JFF::LogicSTD::dispatchLoadSceneRequests()
{
	if (!delayLoadedScenes.empty())
	{
		delayLoadedScenes.back()(); // Load the last scene requested
		delayLoadedScenes.clear();
	}
}

inline void JFF::LogicSTD::autoLoadSceneIfEmpty()
{
	if (!activeScene.get())
	{
		JFF_LOG_IMPORTANT("Loading a default empty scene")
		activeScene = std::make_shared<Scene>(engine, "Default scene");
	}
}

inline void JFF::LogicSTD::dispatchSpawnGameObjectRequests()
{
	if (!delayLoadedGameObjects.empty())
	{
		std::for_each(delayLoadedGameObjects.begin(), delayLoadedGameObjects.end(), [](const std::function<void()>& fn) { fn(); });
		delayLoadedGameObjects.clear();
	}
}

inline void JFF::LogicSTD::dispatchSetGameObjectStateRequests()
{
	if (!delaySetStateGameObjects.empty())
	{
		std::for_each(delaySetStateGameObjects.begin(), delaySetStateGameObjects.end(), [](const std::function<void()>& fn) { fn(); });
		delaySetStateGameObjects.clear();
	}
}

inline void JFF::LogicSTD::updateGameObjects()
{
	activeScene->visitFromRoot<Scene>(updateGameObjectsAlgorithm);
}
