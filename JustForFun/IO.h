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

#include "Subsystem.h"

#include "File.h"
#include "INIFile.h"
#include "Image.h"
#include "Model.h"
#include "Material.h"

#include "Texture.h"
#include "Cubemap.h"

#include <memory>

namespace JFF
{
	class IO : public Subsystem
	{
	public:
		// Ctor & Dtor
		IO() {}
		virtual ~IO() {}

		// Copy ctor and copy assignment
		IO(const IO& other) = delete;
		IO& operator=(const IO& other) = delete;

		// Move ctor and assignment
		IO(IO&& other) = delete;
		IO operator=(IO&& other) = delete;

		// --------------------- IO interface --------------------- //

		// Raw plaint text loading
		[[nodiscard]] virtual std::shared_ptr<File> loadRawFile(const char* filename) const = 0;

		// INI file loading
		[[nodiscard]] virtual std::shared_ptr<INIFile> loadINIFile(const char* filename) const = 0;

		// Load images
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filename, 
			bool flipVertically = true, bool HDRImage = false, bool bgra = false) const = 0;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filename, 
			const unsigned char* imgBuffer, int bufferSizeBytes, 
			bool flipVertically = true, bool HDRImage = false, bool bgra = false) const = 0;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filepath,
			int width, int height, int numChannels, const std::vector<float>& rawData, bool bgra = false) const = 0;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filepath,
			int width, int height, int numChannels, const std::vector<unsigned char>& rawData, bool bgra = false) const = 0;

		// Load models
		[[nodiscard]] virtual std::shared_ptr<Model> loadModel(const char* assetFilepath,
			const std::weak_ptr<JFF::GameObject>& parentGameObject = std::weak_ptr<JFF::GameObject>()) const = 0;
	};
}