#pragma once

#include "Cacheable.h"
#include <string>

namespace JFF
{
	class Cubemap : public Cacheable
	{
	public:
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

	public:
		static std::string generateCacheName(const char* assetFilepath);
	};
}