#pragma once

#include "Image.h"

namespace JFF
{
	class Engine;

	class ImageSTBI : public Image
	{
	public:
		// Ctor & Dtor
		explicit ImageSTBI(Engine* const engine, const char* filepath, bool flipVertically = true);
		virtual ~ImageSTBI();

		// Copy ctor and copy assignment
		ImageSTBI(const ImageSTBI& other) = delete;
		ImageSTBI& operator=(const ImageSTBI& other) = delete;

		// Move ctor and assignment
		ImageSTBI(ImageSTBI&& other) = delete;
		ImageSTBI operator=(ImageSTBI&& other) = delete;

		// -------------------------------- CACHEABLE INTERFACE -------------------------------- //

		virtual std::string getCacheName() const override;

		// -------------------------------- IMAGE INTERFACE -------------------------------- //

		virtual const Data& data() override;

	protected:
		Engine* engine;

		std::string cacheName;
		Data imgData;
	};
}