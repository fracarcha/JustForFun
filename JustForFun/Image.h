#pragma once

#include "Cacheable.h"
#include <string>

namespace JFF
{
	class Image : public Cacheable
	{
	public:
		struct Data
		{
			Data() :
				rawData(nullptr),
				width(0),
				height(0),
				originalNumChannels(0),
				desiredNumChannels(0)
			{}
			~Data() {} // rawData should be deleted by parent class Image

			// Copy ctor and copy assignment
			Data(const Data& other) = delete;
			Data& operator=(const Data& other) = delete;

			// Move ctor and assignment
			Data(Data&& other) = delete;
			Data operator=(Data&& other) = delete;

			std::string filepath;
			unsigned char* rawData;
			int width, height, originalNumChannels, desiredNumChannels;
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