#pragma once

#include "Logic.h"
#include "Scene.h"
#include "Algorithm.h"

namespace JFF
{
	// Standard implementation of Logic
	class LogicSTD : public Logic
	{
	public:
		// Ctor & Dtor
		LogicSTD();
		virtual ~LogicSTD();

		// Copy ctor and copy assignment
		LogicSTD(const LogicSTD& other) = delete;
		LogicSTD& operator=(const LogicSTD& other) = delete;

		// Move ctor and assignment
		LogicSTD(LogicSTD&& other) = delete;
		LogicSTD operator=(LogicSTD&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// ExecutableSubsystem impl
		virtual ExecutableSubsystem::ExecutionOrder getExecutionOrder() const override;
		virtual bool execute() override;

		// ------------------------------------ Logic interface ------------------------------------ //

		/*
		* Creates a new empty scene.
		* This unloads any previously loaded scene.
		* The new scene is created when the next frame begins
		*/
		virtual void loadEmptyScene(const char* sceneName) override;
		// TODO: load scene from file

		/*
		* Creates an empty GameObject and adds it to scene.
		* The attachment will happen at the beginning of the next frame
		*/
		virtual std::weak_ptr<GameObject> spawnGameObject(
			const char* name,
			const Vec3& localPosition = Vec3::ZERO,
			const Vec3& localRotation = Vec3::ZERO,
			const Vec3& localScale = Vec3::ONE,
			bool initiallyEnabled = true) override;

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
			bool initiallyEnabled = true) override;

		/*
		* Change the GameObject state.
		* The state will effectively change when the next frame begins.
		* If you want to change the state immediately, use GameObject.setState() directly
		*/
		virtual void setGameObjectEnabled(const std::weak_ptr<GameObject>& obj, bool enabled) override;

	protected: // Helper functions
		inline void dispatchLoadSceneRequests();
		inline void autoLoadSceneIfEmpty();
		inline void dispatchSpawnGameObjectRequests();
		inline void dispatchSetGameObjectStateRequests();
		inline void updateGameObjects();

	protected:
		Engine* engine;

		std::shared_ptr<Scene> activeScene;
		std::shared_ptr<Algorithm<Scene, GameObject, EdgeBase<GameObject>>> updateGameObjectsAlgorithm;

		// Delay loaded lists
		std::vector<std::function<void()>> delayLoadedScenes;
		std::vector<std::function<void()>> delayLoadedGameObjects;
		std::vector<std::function<void()>> delaySetStateGameObjects;

	};
}