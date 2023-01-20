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
	class Texture : public Cacheable, public Saveable
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
			std::string folder;

			std::string shaderVariableName;
			std::shared_ptr<Image> img;
			CoordsWrapMode coordsWrapMode;
			FilterMode filterMode;
			int numColorChannels; // This is texture's num channels, not image's
			SpecialFormat specialFormat;
		};

		struct ImageInfo
		{
			std::string folder;

			int width, height;
			int numChannels;
			bool HDR;
			int mipmapLevel;
			bool bgra;
		};

		// Ctor & Dtor
		Texture() {}
		virtual ~Texture() {}

		// Copy ctor and copy assignment
		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;

		// Move ctor and assignment
		Texture(Texture&& other) = delete;
		Texture operator=(Texture&& other) = delete;

		// -------------------------------- TEXTURE INTERFACE -------------------------------- //

		// Make the texture available to the material sampler on the selected texture unit
		virtual void use(int textureUnit) = 0;

		// Free memory that contains this texture and makes it unavailable
		virtual void destroy() = 0;

		// Gets the texture name. This name will match the name of the shader's sampler
		virtual std::string getName() const = 0;

		// Gets info about the internal image this texture is holding
		virtual ImageInfo getImageInfo() const = 0;

	public:
		static std::string generateCacheName(const char* imageFilepath, 
			const CoordsWrapMode& wrapMode, const FilterMode& filterMode, int numColorChannels, SpecialFormat specialFormat);
		static std::string generateCacheName(const char* assetFilepath);

		static const CoordsWrapMode DEFAULT_WRAP_MODE;
		static const FilterMode DEFAULT_FILTER_MODE;
	};
}