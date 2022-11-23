#pragma once

#include "Cubemap.h"

#include "Image.h"
#include <memory>

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

namespace JFF
{
	class Engine;

	class CubemapGL : public Cubemap
	{
	public:
		// Ctor & Dtor
		CubemapGL(Engine* const engine, const char* name, const char* assetFilePath);
		virtual ~CubemapGL();

		// Copy ctor and copy assignment
		CubemapGL(const CubemapGL& other) = delete;
		CubemapGL& operator=(const CubemapGL& other) = delete;

		// Move ctor and assignment
		CubemapGL(CubemapGL&& other) = delete;
		CubemapGL operator=(CubemapGL&& other) = delete;

		// -------------------------------- CACHEABLE INTERFACE -------------------------------- //

		virtual std::string getCacheName() const override;

		// -------------------------------- CUBEMAP INTERFACE -------------------------------- //

		virtual void use(int textureUnit) override;
		virtual void destroy() override;
		virtual std::string getName() const override;

	private:
		inline GLenum extractImageFormat(int numChannels) const;

		inline GLint extractWrapOption(const std::string& option) const;
		inline GLint extractMinFilterOption(const std::string& option);
		inline GLint extractMagFilterOption(const std::string& option) const;
		inline GLint extractTextureFormatOption(const std::string& option) const;

		inline void generate(const std::shared_ptr<Image>& image,
			GLint wrapU, GLint wrapV, GLint wrapW,
			GLint minFilter, GLint magFilter,
			GLint textureFormat);
		inline void generate(const std::shared_ptr<Image>& imageLeft,
			const std::shared_ptr<Image>& imageRight,
			const std::shared_ptr<Image>& imageTop,
			const std::shared_ptr<Image>& imageBottom,
			const std::shared_ptr<Image>& imageFront,
			const std::shared_ptr<Image>& imageBack,
			GLint wrapU, GLint wrapV, GLint wrapW,
			GLint minFilter, GLint magFilter,
			GLint textureFormat);
		inline void loadSingleFace(GLenum facePosition, const Image::Data& img, GLint textureFormat);

	protected:
		Engine* engine;

		std::string cacheName;
		GLuint cube;
		std::string cubeName;

		bool mipmapsGenerated;
		bool isDestroyed;
	};
}