/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Cubemap.h"

#include <sstream>

std::string JFF::Cubemap::generateCacheName(const char* assetFilepath)
{
	std::ostringstream ss;
	ss << "Cubemap://";
	ss << assetFilepath;

	return ss.str();
}

std::string JFF::Cubemap::generateCacheName(
	const char* imageRightPath, const char* imageLeftPath, const char* imageTopPath, 
	const char* imageBottomPath, const char* imageBackPath, const char* imageFrontPath, 
	const CoordsWrapMode& wrapMode, const FilterMode& filterMode, int numColorChannels, 
	SpecialFormat specialFormat, int numMipmapsGenerated)
{
	std::ostringstream ss;
	ss << "Cubemap://";
	ss << imageRightPath << imageLeftPath << imageTopPath << imageBottomPath << imageBackPath << imageFrontPath;

	Wrap u = wrapMode.u;
	switch (u)
	{
	case JFF::Cubemap::Wrap::MIRRORED_REPEAT:
		ss << 'M';
		break;
	case JFF::Cubemap::Wrap::CLAMP_TO_EDGE:
		ss << 'E';
		break;
	case JFF::Cubemap::Wrap::CLAMP_TO_BORDER:
		ss << 'B';
		break;
	case JFF::Cubemap::Wrap::REPEAT:
	default:
		ss << 'R';
		break;
	}

	Wrap v = wrapMode.v;
	switch (v)
	{
	case JFF::Cubemap::Wrap::MIRRORED_REPEAT:
		ss << 'M';
		break;
	case JFF::Cubemap::Wrap::CLAMP_TO_EDGE:
		ss << 'E';
		break;
	case JFF::Cubemap::Wrap::CLAMP_TO_BORDER:
		ss << 'B';
		break;
	case JFF::Cubemap::Wrap::REPEAT:
	default:
		ss << 'R';
		break;
	}

	Wrap w = wrapMode.w;
	switch (w)
	{
	case JFF::Cubemap::Wrap::MIRRORED_REPEAT:
		ss << 'M';
		break;
	case JFF::Cubemap::Wrap::CLAMP_TO_EDGE:
		ss << 'E';
		break;
	case JFF::Cubemap::Wrap::CLAMP_TO_BORDER:
		ss << 'B';
		break;
	case JFF::Cubemap::Wrap::REPEAT:
	default:
		ss << 'R';
		break;
	}

	MinificationFilter minFilter = filterMode.minFilter;
	switch (minFilter)
	{
	case JFF::Cubemap::MinificationFilter::NEAREST:
		ss << 'N';
		break;
	case JFF::Cubemap::MinificationFilter::LINEAR:
		ss << 'L';
		break;
	case JFF::Cubemap::MinificationFilter::NEAREST_NEAREST_MIP:
		ss << "nnM";
		break;
	case JFF::Cubemap::MinificationFilter::LINEAR_NEAREST_MIP:
		ss << "lnM";
		break;
	case JFF::Cubemap::MinificationFilter::NEAREST_LINEAR_MIP:
		ss << "nlM";
		break;
	case JFF::Cubemap::MinificationFilter::LINEAR_LINEAR_MIP:
	default:
		ss << "llM";
		break;
	}

	MagnificationFilter magFilter = filterMode.magFilter;
	switch (magFilter)
	{
	case JFF::Cubemap::MagnificationFilter::NEAREST:
		ss << 'N';
		break;
	case JFF::Cubemap::MagnificationFilter::LINEAR:
	default:
		ss << 'L';
		break;
	}

	ss << numColorChannels;

	switch (specialFormat)
	{
	case JFF::Cubemap::SpecialFormat::SRGB:
		ss << 'S';
		break;
	case JFF::Cubemap::SpecialFormat::HDR:
		ss << 'H';
		break;
	case JFF::Cubemap::SpecialFormat::NONE:
	default:
		ss << 'N';
		break;
	}

	ss << "mip" << numMipmapsGenerated;

	return ss.str();
}
