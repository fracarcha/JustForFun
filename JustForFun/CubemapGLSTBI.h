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

#include "Cubemap.h"

#include "Image.h"
#include <memory>

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

namespace JFF
{
	class Engine;

	class CubemapGLSTBI : public Cubemap
	{
	public:
		// Ctor & Dtor
		CubemapGLSTBI(Engine* const engine, const char* name, const char* assetFilePath);
		CubemapGLSTBI(Engine* const engine, const Params& params);
		virtual ~CubemapGLSTBI();

		// Copy ctor and copy assignment
		CubemapGLSTBI(const CubemapGLSTBI& other) = delete;
		CubemapGLSTBI& operator=(const CubemapGLSTBI& other) = delete;

		// Move ctor and assignment
		CubemapGLSTBI(CubemapGLSTBI&& other) = delete;
		CubemapGLSTBI operator=(CubemapGLSTBI&& other) = delete;

		// -------------------------------- CACHEABLE INTERFACE -------------------------------- //

		virtual std::string getCacheName() const override;

		// -------------------------------- SAVEABLE INTERFACE -------------------------------- //

		virtual void writeToFile(const char* newFilename, bool storeInGeneratedSubfolder = true) override;

		// -------------------------------- CUBEMAP INTERFACE -------------------------------- //

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
		inline GLint extractWrapOption(Cubemap::Wrap option) const;

		inline GLint extractMinFilterOption(const std::string& option);
		inline GLint extractMinFilterOption(Cubemap::MinificationFilter option);

		inline GLint extractMagFilterOption(const std::string& option) const;
		inline GLint extractMagFilterOption(Cubemap::MagnificationFilter option) const;

		inline GLint extractTextureFormatOption(int numColorChannels, const std::string& specialFormatOption) const;
		inline GLint extractTextureFormatOption(int numColorChannels, Cubemap::SpecialFormat specialFormat) const;

		inline int extractMipmapOption(const std::string& option) const;

		inline void generate(const std::shared_ptr<Image>& imageLeft,
			const std::shared_ptr<Image>& imageRight,
			const std::shared_ptr<Image>& imageTop,
			const std::shared_ptr<Image>& imageBottom,
			const std::shared_ptr<Image>& imageFront,
			const std::shared_ptr<Image>& imageBack,
			GLint wrapU, GLint wrapV, GLint wrapW,
			GLint minFilter, GLint magFilter,
			GLint textureFormat);
		inline void loadSingleFace(GLenum facePosition, const std::shared_ptr<Image>& image, GLint textureFormat, GLint mipmapLevel);

	protected:
		Engine* engine;

		std::string cacheName;
		GLuint cube;
		ImageInfo imgInfo;

		bool isDestroyed;
	};
}