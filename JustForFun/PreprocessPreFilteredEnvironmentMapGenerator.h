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

#include "Preprocess.h"

#include "Cubemap.h"
#include "Material.h"
#include "MeshObject.h"

#include <memory>

namespace JFF
{
	class Engine;

	class PreprocessPreFilteredEnvironmentMapGenerator : public Preprocess
	{
	public:
		// Ctor & Dtor
		PreprocessPreFilteredEnvironmentMapGenerator(
			Engine* const engine, 
			const std::weak_ptr<Cubemap>& environmentMap, 
			unsigned int cubemapWidth,
			unsigned int numMipmaps);
		virtual ~PreprocessPreFilteredEnvironmentMapGenerator();

		// Copy ctor and copy assignment
		PreprocessPreFilteredEnvironmentMapGenerator(const PreprocessPreFilteredEnvironmentMapGenerator& other) = delete;
		PreprocessPreFilteredEnvironmentMapGenerator& operator=(const PreprocessPreFilteredEnvironmentMapGenerator& other) = delete;

		// Move ctor and assignment
		PreprocessPreFilteredEnvironmentMapGenerator(PreprocessPreFilteredEnvironmentMapGenerator&& other) = delete;
		PreprocessPreFilteredEnvironmentMapGenerator operator=(PreprocessPreFilteredEnvironmentMapGenerator&& other) = delete;

		// ---------------------- PREPROCESS INTERFACE ---------------------- //

		// Execute the preprocessing effect
		virtual void execute() override;

	protected:
		Engine* engine;

		std::shared_ptr<Cubemap> envMap;
		unsigned int cubemapWidth; // Cubemaps are squared, so height isn't needed
		unsigned int numMipmaps;

		// Matrices
		Mat4 projectionMatrix;
		Mat4 viewMatrixRight, viewMatrixLeft, viewMatrixTop, viewMatrixBottom, viewMatrixFront, viewMatrixBack;

		// Materials
		std::shared_ptr<Material> preFilteredEnvironmentMapGeneratorMaterial;

		// Framebuffers
		std::shared_ptr<Framebuffer> fbo;

		// Mesh object
		std::shared_ptr<MeshObject> mesh;
	};
}