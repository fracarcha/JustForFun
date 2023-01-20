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
#include "Saveable.h"

#include "Image.h"

#include <string>
#include <memory>

namespace JFF
{
	class Cubemap : public Cacheable, public Saveable
	{
	public:
		enum class Wrap : char
		{
			REPEAT,
			MIRRORED_REPEAT,
			CLAMP_TO_EDGE, // UVs outside [0,1] range will use the nearest texel
			CLAMP_TO_BORDER, // USs outside [0,1] range will be black (or whatever border color is defined)
		};

		enum class MagnificationFilter : char
		{
			NEAREST,
			LINEAR,
		};

		enum class MinificationFilter : char
		{
			NEAREST,
			LINEAR,
			NEAREST_NEAREST_MIP,
			LINEAR_NEAREST_MIP,
			NEAREST_LINEAR_MIP,
			LINEAR_LINEAR_MIP,
		};

		enum class SpecialFormat : char
		{
			NONE,
			SRGB,
			HDR,
		};

		struct CoordsWrapMode
		{
			Wrap u, v, w;
		};

		struct FilterMode
		{
			MinificationFilter minFilter;
			MagnificationFilter magFilter;
		};

		struct Params
		{
			std::string shaderVariableName;

			std::string folder;

			std::shared_ptr<Image> imgLeft;
			std::shared_ptr<Image> imgRight;
			std::shared_ptr<Image> imgTop;
			std::shared_ptr<Image> imgBottom;
			std::shared_ptr<Image> imgFront;
			std::shared_ptr<Image> imgBack;

			CoordsWrapMode coordsWrapMode;
			FilterMode filterMode;
			int numColorChannels; // This is cubemap's num channels, not image's
			SpecialFormat specialFormat;
			int numMipmapsGenerated; // -1: Auto generate mipmaps | 0: Don't generate mipmaps | >=1: Generate specific number of mipmaps
		};

		struct ImageInfo
		{
			std::string shaderVariableName;

			std::string folder;

			std::string imageRightFilename;
			std::string imageLeftFilename;
			std::string imageTopFilename;
			std::string imageBottomFilename;
			std::string imageBackFilename;
			std::string imageFrontFilename;

			int width, height;
			int numChannels;
			bool HDR;
			int numMipmapsGenerated; // -1: Auto generate mipmaps | 0: Don't generate mipmaps | >=1: Generate specific number of mipmaps
			bool bgra;
		};

		// Ctor & Dtor
		Cubemap() {}
		virtual ~Cubemap() {}

		// Copy ctor and copy assignment
		Cubemap(const Cubemap& other) = delete;
		Cubemap& operator=(const Cubemap& other) = delete;

		// Move ctor and assignment
		Cubemap(Cubemap&& other) = delete;
		Cubemap operator=(Cubemap&& other) = delete;

		// -------------------------------- CUBEMAP INTERFACE -------------------------------- //

		// Make the cubemap available to the material sampler on the selected texture unit
		virtual void use(int textureUnit) = 0;

		// Free memory that contains this cubemap and makes it unavailable
		virtual void destroy() = 0;

		// Gets the cubemap name. This name will match the name of the shader's sampler
		virtual std::string getName() const = 0;

		// Gets info about the internal images this texture is holding
		virtual ImageInfo getImageInfo() const = 0;

	public:
		static std::string generateCacheName(const char* assetFilepath);
		static std::string generateCacheName(
			const char* imageRightPath, const char* imageLeftPath, const char* imageTopPath,
			const char* imageBottomPath, const char* imageBackPath, const char* imageFrontPath,
			const CoordsWrapMode& wrapMode, const FilterMode& filterMode, int numColorChannels, 
			SpecialFormat specialFormat, int numMipmapsGenerated);
	};
}