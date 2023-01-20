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

	class PreprocessIrradianceGenerator : public Preprocess
	{
	public:
		// Ctor & Dtor
		PreprocessIrradianceGenerator(Engine* const engine, const std::weak_ptr<Cubemap>& environmentMap, unsigned int cubemapWidth);
		virtual ~PreprocessIrradianceGenerator();

		// Copy ctor and copy assignment
		PreprocessIrradianceGenerator(const PreprocessIrradianceGenerator& other) = delete;
		PreprocessIrradianceGenerator& operator=(const PreprocessIrradianceGenerator& other) = delete;

		// Move ctor and assignment
		PreprocessIrradianceGenerator(PreprocessIrradianceGenerator&& other) = delete;
		PreprocessIrradianceGenerator operator=(PreprocessIrradianceGenerator&& other) = delete;

		// ---------------------- PREPROCESS INTERFACE ---------------------- //

		// Execute the preprocessing effect
		virtual void execute() override;

	protected:
		Engine* engine;

		std::shared_ptr<Cubemap> envMap;
		unsigned int cubemapWidth; // Cubemaps are squared, so height isn't needed

		// Matrices
		Mat4 projectionMatrix;
		Mat4 viewMatrixRight, viewMatrixLeft, viewMatrixTop, viewMatrixBottom, viewMatrixFront, viewMatrixBack;

		// Materials
		std::shared_ptr<Material> irradianceGeneratorMaterial;

		// Framebuffers
		std::shared_ptr<Framebuffer> fbo;

		// Mesh object
		std::shared_ptr<MeshObject> mesh;
	};
}