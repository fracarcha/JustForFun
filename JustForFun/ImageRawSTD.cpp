/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ImageRawSTD.h"

#include "Log.h"
#include "FileSystemSetup.h"

#include <sstream>
#include <vector>

JFF::ImageRawSTD::ImageRawSTD(const char* filepath, int width, int height, int numChannels, const std::vector<float>& rawData, bool bgra) :
	cacheName(),
	imgData(),

	rawData(),
	rawDataF(rawData)
{
	JFF_LOG_INFO("Creating image")

	imgData.filepath				= filepath;
	extractPath();
	imgData.imgChannelType			= Image::ImageChannelType::FLOAT;
	imgData.rawDataF				= rawDataF.data();
	imgData.width					= width;
	imgData.height					= height;
	imgData.originalNumChannels		= numChannels;
	imgData.desiredNumChannels		= numChannels;
	imgData.bgra					= bgra;

	// Generate cache name
	cacheName = generateCacheName(filepath);
}

JFF::ImageRawSTD::ImageRawSTD(const char* filepath, int width, int height, int numChannels, const std::vector<unsigned char>& rawData, bool bgra) :
	cacheName(),
	imgData(),

	rawData(rawData),
	rawDataF()
{
	JFF_LOG_INFO("Creating image")

	imgData.filepath				= filepath;
	extractPath();
	imgData.imgChannelType			= Image::ImageChannelType::UNSIGNED_BYTE;
	imgData.rawData					= this->rawData.data();
	imgData.width					= width;
	imgData.height					= height;
	imgData.originalNumChannels		= numChannels;
	imgData.desiredNumChannels		= numChannels;
	imgData.bgra					= bgra;

	// Generate cache name
	cacheName = generateCacheName(filepath);
}

JFF::ImageRawSTD::~ImageRawSTD()
{
	JFF_LOG_INFO("Deleting image")
}

std::string JFF::ImageRawSTD::getCacheName() const
{
	return cacheName;
}

const JFF::Image::Data& JFF::ImageRawSTD::data()
{
	return imgData;
}

inline void JFF::ImageRawSTD::extractPath()
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
