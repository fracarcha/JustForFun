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

#include "Material.h"
#include "MeshObject.h"

#include <memory>

namespace JFF
{
	class Engine;

	class PreprocessBRDFIntegrationMapGenerator : public Preprocess
	{
	public:
		// Ctor & Dtor
		PreprocessBRDFIntegrationMapGenerator(Engine* const engine, unsigned int textureWidth);
		virtual ~PreprocessBRDFIntegrationMapGenerator();

		// Copy ctor and copy assignment
		PreprocessBRDFIntegrationMapGenerator(const PreprocessBRDFIntegrationMapGenerator& other) = delete;
		PreprocessBRDFIntegrationMapGenerator& operator=(const PreprocessBRDFIntegrationMapGenerator& other) = delete;

		// Move ctor and assignment
		PreprocessBRDFIntegrationMapGenerator(PreprocessBRDFIntegrationMapGenerator&& other) = delete;
		PreprocessBRDFIntegrationMapGenerator operator=(PreprocessBRDFIntegrationMapGenerator&& other) = delete;

		// ---------------------- PREPROCESS INTERFACE ---------------------- //

		// Execute the preprocessing effect
		virtual void execute() override;

	protected:
		Engine* engine;

		unsigned int textureWidth; // Texture is squared, so height isn't needed

		// Materials
		std::shared_ptr<Material> BRDFIntegrationMapGeneratorMaterial;

		// Framebuffers
		std::shared_ptr<Framebuffer> fbo;

		// Mesh object
		std::shared_ptr<MeshObject> mesh;
	};
}