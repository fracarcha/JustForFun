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

#include "IO.h"

namespace JFF
{
	// Standard implementation of Input/Output subsystem
	class IOSTD : public IO
	{
	public:
		// Ctor & Dtor
		IOSTD();
		virtual ~IOSTD();

		// Copy ctor and copy assignment
		IOSTD(const IOSTD& other) = delete;
		IOSTD& operator=(const IOSTD& other) = delete;

		// Move ctor and assignment
		IOSTD(IOSTD&& other) = delete;
		IOSTD operator=(IOSTD&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// --------------------- IO interface --------------------- //

		// Raw plaint text loading
		[[nodiscard]] virtual std::shared_ptr<File> loadRawFile(const char* filename) const override;
		
		// INI file loading
		[[nodiscard]] virtual std::shared_ptr<INIFile> loadINIFile(const char* filename) const override;

		// Load images
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filename, 
			bool flipVertically = true, bool HDRImage = false, bool bgra = false) const override;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filename,
			const unsigned char* imgBuffer, int bufferSizeBytes, 
			bool flipVertically = true, bool HDRImage = false, bool bgra = false) const override;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filepath,
			int width, int height, int numChannels, const std::vector<float>& rawData, bool bgra = false) const override;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filepath,
			int width, int height, int numChannels, const std::vector<unsigned char>& rawData, bool bgra = false) const override;

		// Load models
		[[nodiscard]] virtual std::shared_ptr<Model> loadModel(const char* assetFilepath,
			const std::weak_ptr<JFF::GameObject>& parentGameObject = std::weak_ptr<JFF::GameObject>()) const override;

	protected:
		Engine* engine;
	};
}