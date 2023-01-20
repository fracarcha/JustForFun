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

#include "Image.h"
#include <vector>

namespace JFF
{
	class ImageRawSTD : public Image
	{
	public:
		// Ctor & Dtor
		explicit ImageRawSTD(const char* filepath, int width, int height, int numChannels, const std::vector<float>& rawData, bool bgra = false);
		explicit ImageRawSTD(const char* filepath, int width, int height, int numChannels, const std::vector<unsigned char>& rawData, bool bgra = false);
		virtual ~ImageRawSTD();

		// Copy ctor and copy assignment
		ImageRawSTD(const ImageRawSTD& other) = delete;
		ImageRawSTD& operator=(const ImageRawSTD& other) = delete;

		// Move ctor and assignment
		ImageRawSTD(ImageRawSTD&& other) = delete;
		ImageRawSTD operator=(ImageRawSTD&& other) = delete;

		// -------------------------------- CACHEABLE INTERFACE -------------------------------- //

		virtual std::string getCacheName() const override;

		// -------------------------------- IMAGE INTERFACE -------------------------------- //

		virtual const Data& data() override;

	protected:
		inline void extractPath();

	protected:
		std::string cacheName;
		Data imgData;

		std::vector<unsigned char> rawData;
		std::vector<float> rawDataF;
	};
}