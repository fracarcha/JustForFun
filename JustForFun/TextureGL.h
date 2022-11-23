#pragma once

#include "Texture.h"

#include <memory>

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

namespace JFF
{
	class Engine;
	class Image;

	class TextureGL : public Texture
	{
	public:
		// Ctor & Dtor
		TextureGL(Engine* const engine, const char* name, const char* assetFilePath);
		TextureGL(Engine* const engine, const char* name, const std::shared_ptr<Image>& image,
			const Texture::CoordsWrapMode& coordsWrapMode = Texture::DEFAULT_WRAP_MODE,
			const Texture::FilterMode& filterMode = Texture::DEFAULT_FILTER_MODE,
			bool isSRGBTexture = false);
		virtual ~TextureGL();

		// Copy ctor and copy assignment
		TextureGL(const TextureGL& other) = delete;
		TextureGL& operator=(const TextureGL& other) = delete;

		// Move ctor and assignment
		TextureGL(TextureGL&& other) = delete;
		TextureGL operator=(TextureGL&& other) = delete;

		// -------------------------------- CACHEABLE INTERFACE -------------------------------- //

		virtual std::string getCacheName() const override;

		// -------------------------------- TEXTURE INTERFACE -------------------------------- //

		virtual void use(int textureUnit) override;
		virtual void destroy() override;
		virtual std::string getName() const override;

	private:
		inline GLenum extractImageFormat(int numChannels) const;

		inline GLint extractWrapOption(const std::string& option) const;
		inline GLint extractWrapOption(Texture::Wrap option) const;

		inline GLint extractMinFilterOption(const std::string& option);
		inline GLint extractMinFilterOption(Texture::MinificationFilter option);

		inline GLint extractMagFilterOption(const std::string& option) const;
		inline GLint extractMagFilterOption(Texture::MagnificationFilter option) const;

		inline GLint extractTextureFormatOption(const std::string& option) const;
		inline GLint extractTextureFormatOption(bool isSRGBTexture) const;

		inline void generate(const std::shared_ptr<Image>& image, 
			GLint wrapU, GLint wrapV, GLint wrapW, 
			GLint minFilter, GLint magFilter, GLint textureFormat);

	protected:
		Engine* engine;

		std::string cacheName;
		GLuint tex;
		std::string texName;

		bool mipmapsGenerated;
		bool isDestroyed;
	};
}