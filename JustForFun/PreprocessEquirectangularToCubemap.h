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

#include "Image.h"
#include "Material.h"
#include "MeshObject.h"

#include <memory>

namespace JFF
{
	class Engine;

	class PreprocessEquirectangularToCubemap : public Preprocess
	{
	public:
		// Ctor & Dtor
		PreprocessEquirectangularToCubemap(Engine* const engine, const std::weak_ptr<Image>& img, unsigned int cubemapWidth);
		virtual ~PreprocessEquirectangularToCubemap();

		// Copy ctor and copy assignment
		PreprocessEquirectangularToCubemap(const PreprocessEquirectangularToCubemap& other) = delete;
		PreprocessEquirectangularToCubemap& operator=(const PreprocessEquirectangularToCubemap& other) = delete;

		// Move ctor and assignment
		PreprocessEquirectangularToCubemap(PreprocessEquirectangularToCubemap&& other) = delete;
		PreprocessEquirectangularToCubemap operator=(PreprocessEquirectangularToCubemap&& other) = delete;

		// ---------------------- PREPROCESS INTERFACE ---------------------- //

		// Execute the preprocessing effect
		virtual void execute() override;

	protected:
		Engine* engine;

		std::string imageFilePath;
		unsigned int cubemapWidth; // Cubemaps are squared, so height isn't needed

		// Matrices
		Mat4 projectionMatrix;
		Mat4 viewMatrixRight, viewMatrixLeft, viewMatrixTop, viewMatrixBottom, viewMatrixFront, viewMatrixBack;

		// Materials
		std::shared_ptr<Material> equirectangularToCubemapMaterial;

		// Framebuffers
		std::shared_ptr<Framebuffer> fbo;

		// Mesh object
		std::shared_ptr<MeshObject> mesh;
	};
}