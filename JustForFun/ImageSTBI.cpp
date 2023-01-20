/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ImageSTBI.h"
#include "FileSystemSetup.h"

#include "Log.h"

#include "stb_image.h"
#include <sstream>
#include <vector>

JFF::ImageSTBI::ImageSTBI(const char* filepath, bool flipVertically, bool HDRImage, bool bgra) :
	cacheName(),
	imgData()
{
	JFF_LOG_INFO("Creating image")

	using ImgType = Image::ImageChannelType;

	std::string fullPath = std::string("Assets").append(1, JFF_SLASH).append(filepath);

	stbi_set_flip_vertically_on_load(flipVertically);
	imgData.filepath = filepath;
	extractPath();
	imgData.desiredNumChannels = 4;
	imgData.bgra = bgra;
	if (HDRImage)
	{
		imgData.imgChannelType = ImgType::FLOAT;
		imgData.rawDataF = stbi_loadf(fullPath.c_str(), &imgData.width, &imgData.height, &imgData.originalNumChannels, imgData.desiredNumChannels);
	}
	else
	{
		imgData.imgChannelType = ImgType::UNSIGNED_BYTE;
		imgData.rawData = stbi_load(fullPath.c_str(), &imgData.width, &imgData.height, &imgData.originalNumChannels, imgData.desiredNumChannels);
	}

	// Ensure that data loaded is correct
	if (imgData.rawData == NULL && imgData.rawDataF == NULL)
	{
		JFF_LOG_ERROR("Couldn't load image. Reason: " << stbi_failure_reason())
	}

	// Generate cache name
	cacheName = generateCacheName(filepath);
}

JFF::ImageSTBI::ImageSTBI(const char* filepath, const unsigned char* imgBuffer, int bufferSizeBytes, bool flipVertically, bool HDRImage, bool bgra) :
	cacheName(),
	imgData()
{
	JFF_LOG_INFO("Creating image")

	using ImgType = Image::ImageChannelType;

	stbi_set_flip_vertically_on_load(flipVertically);
	imgData.filepath = filepath;
	extractPath();
	imgData.desiredNumChannels = 4;
	imgData.bgra = bgra;
	if (HDRImage)
	{
		imgData.imgChannelType = ImgType::FLOAT;
		imgData.rawDataF = stbi_loadf_from_memory(imgBuffer, bufferSizeBytes, 
			&imgData.width, &imgData.height, &imgData.originalNumChannels, imgData.desiredNumChannels);
	}
	else
	{
		imgData.imgChannelType = ImgType::UNSIGNED_BYTE;
		imgData.rawData = stbi_load_from_memory(imgBuffer, bufferSizeBytes,
			&imgData.width, &imgData.height, &imgData.originalNumChannels, imgData.desiredNumChannels);
	}

	// Ensure that data loaded is correct
	if (imgData.rawData == NULL && imgData.rawDataF == NULL)
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
	switch (imgData.imgChannelType)
	{
	case Image::ImageChannelType::FLOAT:
		stbi_image_free(imgData.rawDataF);
		break;
	case Image::ImageChannelType::UNSIGNED_BYTE:
	default:
		stbi_image_free(imgData.rawData);
		break;
	}
}

std::string JFF::ImageSTBI::getCacheName() const
{
	return cacheName;
}

const JFF::Image::Data& JFF::ImageSTBI::data()
{
	return imgData;
}

inline void JFF::ImageSTBI::extractPath()
{
	std::stringstream ss(imgData.filepath);
	std::string segment;
	std::vector<std::string> tokens;

	while (std::getline(ss, segment, JFF_SLASH))
		tokens.push_back(segment);

	for (int i = 0; i < tokens.size() - 1; ++i)
	{
		imgData.folder += tokens[i];
		if (i != tokens.size() - 2)
			imgData.folder += JFF_SLASH_STRING;
	}
	
	imgData.filename = tokens.back();
}

