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

namespace JFF
{
	class ImageSTBI : public Image
	{
	public:
		// Ctor & Dtor
		explicit ImageSTBI(const char* filepath, bool flipVertically = true, bool HDRImage = false, bool bgra = false);
		explicit ImageSTBI(const char* filepath, const unsigned char* imgBuffer, int bufferSizeBytes, 
			bool flipVertically = true, bool HDRImage = false, bool bgra = false);
		virtual ~ImageSTBI();

		// Copy ctor and copy assignment
		ImageSTBI(const ImageSTBI& other) = delete;
		ImageSTBI& operator=(const ImageSTBI& other) = delete;

		// Move ctor and assignment
		ImageSTBI(ImageSTBI&& other) = delete;
		ImageSTBI operator=(ImageSTBI&& other) = delete;

		// -------------------------------- CACHEABLE INTERFACE -------------------------------- //

		virtual std::string getCacheName() const override;

		// -------------------------------- IMAGE INTERFACE -------------------------------- //

		virtual const Data& data() override;

	protected:
		inline void extractPath();

	protected:
		std::string cacheName;
		Data imgData;
	};
}