/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "TextureGLSTBI.h"

#include "Log.h"
#include "Engine.h"

#include "stb_image_write.h"
#include "FileSystemSetup.h"

#include <sstream>
#include <regex>

JFF::TextureGLSTBI::TextureGLSTBI(JFF::Engine* const engine, const char* name, const char* assetFilePath) :
	engine(engine),

	cacheName(),
	tex(0u),
	texName(name),
	imgInfo(),

	mipmapsGenerated(false),
	isDestroyed(false)
{
	JFF_LOG_INFO("Ctor TextureGLSTBI")

	auto io = engine->io.lock();

	// Load the ini file that contains the image filename and texture options
	std::shared_ptr<INIFile> iniFile = io->loadINIFile(assetFilePath);

	// Load the image checking if INI file has enough loading info
	std::string imageFilePath = iniFile->getString("image", "path");
	if (imageFilePath.empty())
	{
		JFF_LOG_ERROR("Failed to load texture. Texture asset doesn't contain a valid image path")
		return;
	}

	// Extract other image loading parameters
	if (iniFile->has("image", "folder"))
		imgInfo.folder = std::regex_replace(iniFile->getString("image", "folder"), std::regex(R"raw(/)raw"), JFF_SLASH_STRING);

	bool flipVertically = true;
	if (iniFile->has("image", "flip-vertically"))
		flipVertically = iniFile->getString("image", "flip-vertically") == "true";
	
	bool HDRImage = false;
	if (iniFile->has("image", "img-hdr"))
		HDRImage = iniFile->getString("image", "img-hdr") == "true";

	// TODO: Load BGRA format from Asset file

	// Get the image data and check if it's valid
	std::string imageFullFilePath = imgInfo.folder.empty() ? imageFilePath : (imgInfo.folder + JFF_SLASH_STRING + imageFilePath);
	std::shared_ptr<Image> image = io->loadImage(imageFullFilePath.c_str(), flipVertically, HDRImage); 

	// Extract the rest of texture paramters from INI file
	GLint wrapU = extractWrapOption(iniFile->getString("texture", "wrapU"));
	GLint wrapV = extractWrapOption(iniFile->getString("texture", "wrapV"));
	GLint wrapW = extractWrapOption(iniFile->getString("texture", "wrapW"));
	GLint minFilter = extractMinFilterOption(iniFile->getString("texture", "filter-min"));
	GLint magFilter = extractMagFilterOption(iniFile->getString("texture", "filter-mag"));
	GLint textureFormat = extractTextureFormatOption(
		iniFile->getInt("texture", "tex-num-channels"), iniFile->getString("texture", "special-format"));

	// Generate the texture using OpenGL commands
	generate(image, wrapU, wrapV, wrapW, minFilter, magFilter, textureFormat);

	// Set cache name. Asset filepath can be an unique name for texture caching
	cacheName = generateCacheName(assetFilePath);
}

JFF::TextureGLSTBI::TextureGLSTBI(Engine* const engine, const Params& params) :
	engine(engine),

	cacheName(),
	tex(0u),
	texName(params.shaderVariableName),
	imgInfo(),

	mipmapsGenerated(false),
	isDestroyed(false)
{
	JFF_LOG_INFO("Ctor TextureGLSTBI")

	imgInfo.folder = params.folder;

	// Extract the rest of texture paramters from params
	GLint wrapU = extractWrapOption(params.coordsWrapMode.u);
	GLint wrapV = extractWrapOption(params.coordsWrapMode.v);
	GLint wrapW = extractWrapOption(params.coordsWrapMode.w);
	GLint minFilter = extractMinFilterOption(params.filterMode.minFilter);
	GLint magFilter = extractMagFilterOption(params.filterMode.magFilter);
	GLint textureFormat = extractTextureFormatOption(params.numColorChannels, params.specialFormat);

	// Generate the texture using OpenGL commands
	generate(params.img, wrapU, wrapV, wrapW, minFilter, magFilter, textureFormat);

	// Generate cache name
	cacheName = generateCacheName(params.img->data().filepath.c_str(), 
		params.coordsWrapMode, params.filterMode, params.numColorChannels, params.specialFormat);
}

JFF::TextureGLSTBI::~TextureGLSTBI()
{
	JFF_LOG_INFO("Dtor TextureGLSTBI")

	// Ensure the texture GPU memory is destroyed
	if (!isDestroyed)
	{
		JFF_LOG_WARNING("Texture GPU memory successfully destroyed on Texture's destructor. You should call destroy() before destructor is called")
		destroy();
	}
}

std::string JFF::TextureGLSTBI::getCacheName() const
{
	return cacheName;
}

void JFF::TextureGLSTBI::writeToFile(const char* newFilename, bool storeInGeneratedSubfolder)
{
	// Select this as target texture
	use(0); // Used texture unit 0 because it's not important here

	// TODO: write mipmap levels

	// Gather image info before writting to disk
	int mipmapLevel = imgInfo.mipmapLevel;
	int width		= imgInfo.width / (int)std::pow(2, mipmapLevel);
	int height		= imgInfo.height / (int)std::pow(2, mipmapLevel);
	int numChannels = imgInfo.numChannels;
	bool HDR		= imgInfo.HDR;
	bool bgra		= imgInfo.bgra;

	GLenum imgFormat = extractImageFormat(numChannels, bgra);
	GLenum imgType = extractImageType(HDR);

	// Full path
	std::ostringstream oss;
	oss << "Assets" << JFF_SLASH;
	if (storeInGeneratedSubfolder)
		oss << "Generated" << JFF_SLASH;
	oss << newFilename;

	// Read data from OpenGL and write it to disk
	if (HDR)
	{
		// HDR have .hdr extension
		oss << ".hdr";
		std::string fullPath = oss.str();

		float* pixels = new float[(size_t)width * height * numChannels]; // TODO: Keep in mind memory alignment (glPixelStorei())
		glGetTexImage(GL_TEXTURE_2D, mipmapLevel, imgFormat, imgType, pixels);
		stbi_write_hdr(fullPath.c_str(), width, height, numChannels, pixels);
		delete[] pixels;
	}
	else
	{
		// Non HDR have .png extension
		oss << ".png";
		std::string fullPath = oss.str();

		unsigned char* pixels = new unsigned char[(size_t)width * height * numChannels]; // TODO: Keep in mind memory alignment (glPixelStorei())
		glGetTexImage(GL_TEXTURE_2D, mipmapLevel, imgFormat, imgType, pixels);
		stbi_write_png(fullPath.c_str(), width, height, numChannels, pixels, /* Stride between rows */ 0);
		delete[] pixels;
	}
}

void JFF::TextureGLSTBI::use(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void JFF::TextureGLSTBI::destroy()
{
	glDeleteTextures(1, &tex);
	isDestroyed = true;
}

std::string JFF::TextureGLSTBI::getName() const
{
	return texName;
}

JFF::Texture::ImageInfo JFF::TextureGLSTBI::getImageInfo() const
{
	return imgInfo;
}

inline GLenum JFF::TextureGLSTBI::extractImageFormat(const std::shared_ptr<Image>& image) const
{
	const Image::Data& imgData = image->data();
	return extractImageFormat(imgData.desiredNumChannels, imgData.bgra);
}

inline GLenum JFF::TextureGLSTBI::extractImageFormat(int numChannels, bool bgra) const
{
	if (bgra)
	{
		if (numChannels == 3)
			return GL_BGR;
		else
			return GL_BGRA;
	}
	else
	{
		switch (numChannels)
		{
		case 1:
			return GL_RED;
		case 2:
			return GL_RG;
		case 3:
			return GL_RGB;
		case 4:
		default:
			return GL_RGBA;
		}
	}
}

inline GLenum JFF::TextureGLSTBI::extractImageType(const std::shared_ptr<Image>& image) const
{
	Image::ImageChannelType channelType = image->data().imgChannelType;

	switch (channelType)
	{
	case JFF::Image::ImageChannelType::FLOAT:
		return GL_FLOAT;
	case JFF::Image::ImageChannelType::UNSIGNED_BYTE:
	default:
		return GL_UNSIGNED_BYTE;
	}
}

inline GLenum JFF::TextureGLSTBI::extractImageType(bool isHDR) const
{
	return isHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;
}

inline const void* JFF::TextureGLSTBI::extractImagePixels(const std::shared_ptr<Image>& image) const
{
	const Image::Data& img = image->data();

	switch (img.imgChannelType)
	{
	case Image::ImageChannelType::FLOAT:
		return img.rawDataF;
	case Image::ImageChannelType::UNSIGNED_BYTE:
	default:
		return img.rawData;
	}
}

inline GLint JFF::TextureGLSTBI::extractWrapOption(const std::string& option) const
{
	if (option == "clamp")
	{
		return GL_CLAMP_TO_EDGE;
	}
	else if (option == "repeat")
	{
		return GL_REPEAT;
	}
	else if (option == "mirror")
	{
		return GL_MIRRORED_REPEAT;
	}

	return GL_REPEAT;
}

inline GLint JFF::TextureGLSTBI::extractWrapOption(Texture::Wrap option) const
{
	switch (option)
	{
	case JFF::Texture::Wrap::MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;
	case JFF::Texture::Wrap::CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
	case JFF::Texture::Wrap::CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;
	case JFF::Texture::Wrap::REPEAT:
	default:
		return GL_REPEAT;
	}
}

inline GLint JFF::TextureGLSTBI::extractMinFilterOption(const std::string& option) 
{
	if (option == "nearest")
	{
		return GL_NEAREST;
	}
	else if (option == "linear")
	{
		return GL_LINEAR;
	}
	else if (option == "nearest-nearestMip")
	{
		mipmapsGenerated = true;
		return GL_NEAREST_MIPMAP_NEAREST;
	}
	else if (option == "linear-nearestMip")
	{
		mipmapsGenerated = true;
		return GL_LINEAR_MIPMAP_NEAREST;
	}
	else if (option == "nearest-linearMip")
	{
		mipmapsGenerated = true;
		return GL_NEAREST_MIPMAP_LINEAR;
	}
	else if (option == "linear-linearMip")
	{
		mipmapsGenerated = true;
		return GL_LINEAR_MIPMAP_LINEAR;
	}

	return GL_LINEAR;
}

inline GLint JFF::TextureGLSTBI::extractMinFilterOption(Texture::MinificationFilter option)
{
	switch (option)
	{
	case JFF::Texture::MinificationFilter::NEAREST:
		return GL_NEAREST;
	case JFF::Texture::MinificationFilter::NEAREST_NEAREST_MIP:
		mipmapsGenerated = true;
		return GL_NEAREST_MIPMAP_NEAREST;
	case JFF::Texture::MinificationFilter::LINEAR_NEAREST_MIP:
		mipmapsGenerated = true;
		return GL_LINEAR_MIPMAP_NEAREST;
	case JFF::Texture::MinificationFilter::NEAREST_LINEAR_MIP:
		mipmapsGenerated = true;
		return GL_NEAREST_MIPMAP_LINEAR;
	case JFF::Texture::MinificationFilter::LINEAR_LINEAR_MIP:
		mipmapsGenerated = true;
		return GL_LINEAR_MIPMAP_LINEAR;
	case JFF::Texture::MinificationFilter::LINEAR:
	default:
		return GL_LINEAR;
	}
}

inline GLint JFF::TextureGLSTBI::extractMagFilterOption(const std::string& option) const
{
	if (option == "nearest")
	{
		return GL_NEAREST;
	}
	else if (option == "linear")
	{
		return GL_LINEAR;
	}

	return GL_LINEAR;
}

inline GLint JFF::TextureGLSTBI::extractMagFilterOption(Texture::MagnificationFilter option) const
{
	switch (option)
	{
	case JFF::Texture::MagnificationFilter::NEAREST:
		return GL_NEAREST;
	case JFF::Texture::MagnificationFilter::LINEAR:
	default:
		return GL_LINEAR;
	}
}

inline GLint JFF::TextureGLSTBI::extractTextureFormatOption(int numColorChannels, const std::string& specialFormatOption) const
{
	/*
	* NOTE: If the texture is in sRGB (the artist manually gamma corrected the colors working viewing a monitor),
	* we re-correct sRGB to work in linear space
	*/

	if (specialFormatOption == "NONE")
	{
		if (numColorChannels == 1)
			return GL_R8;
		else if (numColorChannels == 2)
			return GL_RG8;
		else if (numColorChannels == 3)
			return GL_RGB8;
		else
			return GL_RGBA8; // 4 byte color depth is preferred on Windows platform for aligment purposes // TODO: Check 
	}
	else if (specialFormatOption == "HDR")
	{
		// High precision unbound floating point channel types
		if (numColorChannels == 1)
			return GL_R16F;
		else if (numColorChannels == 2)
			return GL_RG16F;
		else if (numColorChannels == 3)
			return GL_RGB16F;
		else
			return GL_RGBA16F;
	}
	else if (specialFormatOption == "sRGB")
	{
		// Auto transform from sRGB to Linear color space
		if (numColorChannels == 3)
			return GL_SRGB8;
		else
			return GL_SRGB8_ALPHA8;
	}

	return GL_RGBA8;
}

inline GLint JFF::TextureGLSTBI::extractTextureFormatOption(int numColorChannels, SpecialFormat specialFormat) const
{
	switch (specialFormat)
	{
	case JFF::Texture::SpecialFormat::SRGB:
		if (numColorChannels == 3)
			return GL_SRGB8;
		else
			return GL_SRGB8_ALPHA8;
	case JFF::Texture::SpecialFormat::HDR:
		if (numColorChannels == 1)
			return GL_R16F;
		else if (numColorChannels == 2)
			return GL_RG16F;
		else if (numColorChannels == 3)
			return GL_RGB16F;
		else
			return GL_RGBA16F;
	case JFF::Texture::SpecialFormat::NONE:
	default:
		if (numColorChannels == 1)
			return GL_R8;
		else if (numColorChannels == 2)
			return GL_RG8;
		else if (numColorChannels == 3)
			return GL_RGB8;
		else
			return GL_RGBA8;
	}
}

inline void JFF::TextureGLSTBI::generate(const std::shared_ptr<Image>& image, 
	GLint wrapU, GLint wrapV, GLint wrapW, 
	GLint minFilter, GLint magFilter, GLint textureFormat)
{
	// Get the image data and check if it's valid
	const Image::Data& img = image->data();

	// Generate texture object and bind it to work with it
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	// Apply texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapU);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapV);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapW);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	// Fill the texture object with image data // TODO: May need to call glPixelStorei(GL_UNPACK_ALIGNMENT, #) ...
	GLint mipmapLevel	= 0; // Mipmap level (each mipmap can be loaded one by one or automatically, using glGenerateMipmap()) 
	GLsizei width		= img.width;
	GLsizei height		= img.height;
	GLint border		= 0; // Must be zero
	GLenum imageFormat	= extractImageFormat(image);
	GLenum imageType	= extractImageType(image); // The type of each image channel
	const void* pixels	= extractImagePixels(image);

	glTexImage2D(GL_TEXTURE_2D, mipmapLevel, textureFormat, width, height, border, imageFormat, imageType, pixels);

	// Generate mipmaps automatically for this texture
	if (mipmapsGenerated)
		glGenerateMipmap(GL_TEXTURE_2D);

	// Gather some image info
	imgInfo.width		= width;
	imgInfo.height		= height;
	imgInfo.numChannels = image->data().desiredNumChannels;
	imgInfo.HDR			= image->data().imgChannelType == Image::ImageChannelType::FLOAT;
	imgInfo.mipmapLevel = mipmapLevel;
	imgInfo.bgra		= image->data().bgra;
}
