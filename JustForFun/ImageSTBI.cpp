#include "ImageSTBI.h"
#include "FileSystemSetup.h"

#include "Log.h"
#include "Engine.h"

#include "stb_image.h"

JFF::ImageSTBI::ImageSTBI(Engine* const engine, const char* filepath, bool flipVertically) :
	engine(engine),

	cacheName(),
	imgData()
{
	JFF_LOG_INFO("Creating image")

	std::string fullPath = std::string("Assets").append(1, JFF_SLASH).append(filepath);

	stbi_set_flip_vertically_on_load(flipVertically);
	imgData.filepath = filepath;
	imgData.desiredNumChannels = 4;
	imgData.rawData = stbi_load(fullPath.c_str(), &imgData.width, &imgData.height, &imgData.originalNumChannels, imgData.desiredNumChannels);
	if (imgData.rawData == NULL)
	{
		JFF_LOG_ERROR("Couldn't load image. Reason: " << stbi_failure_reason())
	}

	// Generate cache name
	cacheName = generateCacheName(filepath);
}

JFF::ImageSTBI::~ImageSTBI()
{
	JFF_LOG_INFO("Deleting image")

	// Free the image using the same library that created it
	stbi_image_free(imgData.rawData);
}

std::string JFF::ImageSTBI::getCacheName() const
{
	return cacheName;
}

const JFF::Image::Data& JFF::ImageSTBI::data()
{
	return imgData;
}

