/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Texture.h"

#include <sstream>

// Define Texture struct constants
const JFF::Texture::CoordsWrapMode JFF::Texture::DEFAULT_WRAP_MODE{ Wrap::REPEAT, Wrap::REPEAT , Wrap::REPEAT };
const JFF::Texture::FilterMode JFF::Texture::DEFAULT_FILTER_MODE{ MinificationFilter::LINEAR_LINEAR_MIP, MagnificationFilter::LINEAR };

std::string JFF::Texture::generateCacheName(const char* imageFilepath, 
	const CoordsWrapMode& wrapMode, const FilterMode& filterMode, int numColorChannels, SpecialFormat specialFormat)
{
	std::ostringstream ss;
	ss << "Texture://";
	ss << imageFilepath;
	
	Wrap u = wrapMode.u;
	switch (u)
	{
	case JFF::Texture::Wrap::MIRRORED_REPEAT:
		ss << 'M';
		break;
	case JFF::Texture::Wrap::CLAMP_TO_EDGE:
		ss << 'E';
		break;
	case JFF::Texture::Wrap::CLAMP_TO_BORDER:
		ss << 'B';
		break;
	case JFF::Texture::Wrap::REPEAT:
	default:
		ss << 'R';
		break;
	}

	Wrap v = wrapMode.v;
	switch (v)
	{
	case JFF::Texture::Wrap::MIRRORED_REPEAT:
		ss << 'M';
		break;
	case JFF::Texture::Wrap::CLAMP_TO_EDGE:
		ss << 'E';
		break;
	case JFF::Texture::Wrap::CLAMP_TO_BORDER:
		ss << 'B';
		break;
	case JFF::Texture::Wrap::REPEAT:
	default:
		ss << 'R';
		break;
	}

	Wrap w = wrapMode.w;
	switch (w)
	{
	case JFF::Texture::Wrap::MIRRORED_REPEAT:
		ss << 'M';
		break;
	case JFF::Texture::Wrap::CLAMP_TO_EDGE:
		ss << 'E';
		break;
	case JFF::Texture::Wrap::CLAMP_TO_BORDER:
		ss << 'B';
		break;
	case JFF::Texture::Wrap::REPEAT:
	default:
		ss << 'R';
		break;
	}

	MinificationFilter minFilter = filterMode.minFilter;
	switch (minFilter)
	{
	case JFF::Texture::MinificationFilter::NEAREST:
		ss << 'N';
		break;
	case JFF::Texture::MinificationFilter::LINEAR:
		ss << 'L';
		break;
	case JFF::Texture::MinificationFilter::NEAREST_NEAREST_MIP:
		ss << "nnM";
		break;
	case JFF::Texture::MinificationFilter::LINEAR_NEAREST_MIP:
		ss << "lnM";
		break;
	case JFF::Texture::MinificationFilter::NEAREST_LINEAR_MIP:
		ss << "nlM";
		break;
	case JFF::Texture::MinificationFilter::LINEAR_LINEAR_MIP:
	default:
		ss << "llM";
		break;
	}

	MagnificationFilter magFilter = filterMode.magFilter;
	switch (magFilter)
	{
	case JFF::Texture::MagnificationFilter::NEAREST:
		ss << 'N';
		break;
	case JFF::Texture::MagnificationFilter::LINEAR:
	default:
		ss << 'L';
		break;
	}

	ss << numColorChannels;

	switch (specialFormat)
	{
	case JFF::Texture::SpecialFormat::SRGB:
		ss << 'S';
		break;
	case JFF::Texture::SpecialFormat::HDR:
		ss << 'H';
		break;
	case JFF::Texture::SpecialFormat::NONE:
	default:
		ss << 'N';
		break;
	}

	return ss.str();
}

std::string JFF::Texture::generateCacheName(const char* assetFilepath)
{
	std::ostringstream ss;
	ss << "Texture://";
	ss << assetFilepath;

	return ss.str();
}
