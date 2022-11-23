#pragma once

#include "Cacheable.h"
#include <string>

namespace JFF
{
	class Texture : public Cacheable
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

		struct CoordsWrapMode
		{
			Wrap u, v, w;
		};

		struct FilterMode
		{
			MinificationFilter minFilter;
			MagnificationFilter magFilter;
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

	public:
		static std::string generateCacheName(const char* imageFilepath, const CoordsWrapMode& wrapMode, const FilterMode& filterMode);
		static std::string generateCacheName(const char* assetFilepath);

		static const CoordsWrapMode DEFAULT_WRAP_MODE;
		static const FilterMode DEFAULT_FILTER_MODE;
	};
}