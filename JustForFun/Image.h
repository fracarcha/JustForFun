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

#include "Cacheable.h"
#include <string>

namespace JFF
{
	class Image : public Cacheable
	{
	public:
		enum class ImageChannelType : char
		{
			UNSIGNED_BYTE,
			FLOAT,
		};

		struct Data
		{
			Data() :
				filepath(),
				filename(),
				folder(),
				imgChannelType(ImageChannelType::UNSIGNED_BYTE),
				rawData(nullptr),
				rawDataF(nullptr),
				width(0),
				height(0),
				originalNumChannels(0),
				desiredNumChannels(0),
				bgra(false)
			{}
			~Data() {} // rawData should be deleted by parent class Image

			// Copy ctor and copy assignment
			Data(const Data& other) = delete;
			Data& operator=(const Data& other) = delete;

			// Move ctor and assignment
			Data(Data&& other) = delete;
			Data operator=(Data&& other) = delete;

			std::string filepath;	// Full path from Assets folder
			std::string filename;	// Name of the file, without path
			std::string folder;		// Route to the file from Assets folder
			ImageChannelType imgChannelType;
			unsigned char* rawData;
			float* rawDataF;
			int width, height, originalNumChannels, desiredNumChannels;
			bool bgra;
		};

		// Ctor & Dtor
		Image() {}
		virtual ~Image() {}

		// Copy ctor and copy assignment
		Image(const Image& other) = delete;
		Image& operator=(const Image& other) = delete;

		// Move ctor and assignment
		Image(Image&& other) = delete;
		Image operator=(Image&& other) = delete;

		// Image interface
		virtual const Data& data() = 0;

	public:
		static std::string generateCacheName(const char* filepath);
	};
}