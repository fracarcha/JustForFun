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

#include "Texture.h"

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

namespace JFF
{
	class Engine;
	class Image;

	class TextureGLSTBI : public Texture
	{
	public:
		// Ctor & Dtor
		TextureGLSTBI(Engine* const engine, const char* name, const char* assetFilePath);
		TextureGLSTBI(Engine* const engine, const Params& params);
		virtual ~TextureGLSTBI();

		// Copy ctor and copy assignment
		TextureGLSTBI(const TextureGLSTBI& other) = delete;
		TextureGLSTBI& operator=(const TextureGLSTBI& other) = delete;

		// Move ctor and assignment
		TextureGLSTBI(TextureGLSTBI&& other) = delete;
		TextureGLSTBI operator=(TextureGLSTBI&& other) = delete;

		// -------------------------------- CACHEABLE INTERFACE -------------------------------- //

		virtual std::string getCacheName() const override;

		// -------------------------------- SAVEABLE INTERFACE -------------------------------- //

		virtual void writeToFile(const char* newFilename, bool storeInGeneratedSubfolder = true) override;

		// -------------------------------- TEXTURE INTERFACE -------------------------------- //

		virtual void use(int textureUnit) override;
		virtual void destroy() override;
		virtual std::string getName() const override;
		virtual ImageInfo getImageInfo() const override;

	private:
		inline GLenum extractImageFormat(const std::shared_ptr<Image>& image) const;
		inline GLenum extractImageFormat(int numChannels, bool bgra) const;
		inline GLenum extractImageType(const std::shared_ptr<Image>& image) const;
		inline GLenum extractImageType(bool isHDR) const;
		inline const void* extractImagePixels(const std::shared_ptr<Image>& image) const;

		inline GLint extractWrapOption(const std::string& option) const;
		inline GLint extractWrapOption(Texture::Wrap option) const;

		inline GLint extractMinFilterOption(const std::string& option);
		inline GLint extractMinFilterOption(Texture::MinificationFilter option);

		inline GLint extractMagFilterOption(const std::string& option) const;
		inline GLint extractMagFilterOption(Texture::MagnificationFilter option) const;

		inline GLint extractTextureFormatOption(int numColorChannels, const std::string& specialFormatOption) const;
		inline GLint extractTextureFormatOption(int numColorChannels, Texture::SpecialFormat specialFormat) const;

		inline void generate(const std::shared_ptr<Image>& image, 
			GLint wrapU, GLint wrapV, GLint wrapW, 
			GLint minFilter, GLint magFilter, GLint textureFormat);

	protected:
		Engine* engine;

		std::string cacheName;
		GLuint tex;
		std::string texName;
		ImageInfo imgInfo;

		bool mipmapsGenerated;
		bool isDestroyed;
	};
}