/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "CubemapGLSTBI.h"

#include "Log.h"
#include "Engine.h"

#include "PreprocessEquirectangularToCubemap.h"

#include "stb_image_write.h"
#include "FileSystemSetup.h"

#include <sstream>
#include <regex>
#include <stdexcept>

JFF::CubemapGLSTBI::CubemapGLSTBI(Engine* const engine, const char* name, const char* assetFilePath) :
	engine(engine),

	cacheName(),
	cube(0u),
	imgInfo(),

	isDestroyed(false)
{
	JFF_LOG_INFO("Ctor CubemapGLSTBI")

	auto io = engine->io.lock();

	imgInfo.shaderVariableName = name;

	// Load the ini file that contains the image filename and cubemap options
	std::shared_ptr<INIFile> iniFile = io->loadINIFile(assetFilePath);

	// Extract cubemap paramters from INI file
	imgInfo.numMipmapsGenerated = extractMipmapOption(iniFile->getString("cubemap", "mipmaps"));
	GLint wrapU					= extractWrapOption(iniFile->getString("cubemap", "wrapU"));
	GLint wrapV					= extractWrapOption(iniFile->getString("cubemap", "wrapV"));
	GLint wrapW					= extractWrapOption(iniFile->getString("cubemap", "wrapW"));
	GLint minFilter				= extractMinFilterOption(iniFile->getString("cubemap", "filter-min"));
	GLint magFilter				= extractMagFilterOption(iniFile->getString("cubemap", "filter-mag"));
	GLint texFormat				= extractTextureFormatOption(iniFile->getInt("cubemap", "tex-num-channels"), iniFile->getString("cubemap", "special-format"));

	// Extract other image loading parameters
	std::string folder;
	if(iniFile->has("image", "folder"))
		folder = std::regex_replace(iniFile->getString("image", "folder"), std::regex(R"raw(/)raw"), JFF_SLASH_STRING);

	bool HDRImage = false;
	if (iniFile->has("image", "img-hdr"))
		HDRImage = iniFile->getString("image", "img-hdr") == "true";

	// TODO: Load BGRA format from Asset file

	// Loading images has two options: All cubemap info included in a single image or each cubemap face has a unique image
	if (iniFile->has("image", "filename")) // Load single image containing all texture faces
	{
		std::string imageFilePath = iniFile->getString("image", "filename");

		// Extract other image loading parameters
		bool flipVertically = true;
		if (iniFile->has("image", "flip-vertically"))
			flipVertically = iniFile->getString("image", "flip-vertically") == "true";

		int cubemapWidth = 512;
		if (iniFile->has("image", "equirectangular-width"))
			cubemapWidth = iniFile->getInt("image", "equirectangular-width");

		// TODO: If cubemap images were generated from equirectangular map previously, don't generate them again and load them directly

		// Get the equirectangular image data and check if it's valid
		std::string equirectangularFullPath = folder.empty() ? imageFilePath : (folder + JFF_SLASH_STRING + imageFilePath);
		std::shared_ptr<Image> equirectangularImg = io->loadImage(equirectangularFullPath.c_str(), flipVertically, HDRImage);

		// Transform equirectangular texture into 6 cubemap textures
		std::shared_ptr<Preprocess> equirectangularToCubemapPreprocessor 
			= std::make_shared<PreprocessEquirectangularToCubemap>(engine, equirectangularImg, cubemapWidth);
		equirectangularToCubemapPreprocessor->execute();

		// Load generated images
		flipVertically = false; // Don't flip vertically generated images
		imgInfo.imageRightFilename	 = std::regex_replace(imageFilePath, std::regex(R"raw(\.)raw"), "_posx.");
		imgInfo.imageLeftFilename	 = std::regex_replace(imageFilePath, std::regex(R"raw(\.)raw"), "_negx.");
		imgInfo.imageTopFilename	 = std::regex_replace(imageFilePath, std::regex(R"raw(\.)raw"), "_posy.");
		imgInfo.imageBottomFilename	 = std::regex_replace(imageFilePath, std::regex(R"raw(\.)raw"), "_negy.");
		imgInfo.imageBackFilename	 = std::regex_replace(imageFilePath, std::regex(R"raw(\.)raw"), "_posz.");
		imgInfo.imageFrontFilename	 = std::regex_replace(imageFilePath, std::regex(R"raw(\.)raw"), "_negz.");

		imgInfo.folder = "Generated";

		std::string imageRightFullPath	= imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageRightFilename;
		std::string imageLeftFullPath	= imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageLeftFilename;
		std::string imageTopFullPath	= imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageTopFilename;
		std::string imageBottomFullPath = imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageBottomFilename;
		std::string imageBackFullPath	= imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageBackFilename;
		std::string imageFrontFullPath	= imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageFrontFilename;

		// Get the image data and check if it's valid
		std::shared_ptr<Image> imageRight	= io->loadImage(imageRightFullPath.c_str(),  flipVertically, HDRImage);
		std::shared_ptr<Image> imageLeft	= io->loadImage(imageLeftFullPath.c_str(),	 flipVertically, HDRImage);
		std::shared_ptr<Image> imageTop		= io->loadImage(imageTopFullPath.c_str(),	 flipVertically, HDRImage);
		std::shared_ptr<Image> imageBottom	= io->loadImage(imageBottomFullPath.c_str(), flipVertically, HDRImage);
		std::shared_ptr<Image> imageBack	= io->loadImage(imageBackFullPath.c_str(),	 flipVertically, HDRImage);
		std::shared_ptr<Image> imageFront	= io->loadImage(imageFrontFullPath.c_str(),  flipVertically, HDRImage);

		// Generate the texture using OpenGL commands
		generate(imageLeft, imageRight, imageTop, imageBottom, imageFront, imageBack,
			wrapU, wrapV, wrapW, minFilter, magFilter, texFormat);
	}
	else // Load one image per cube face
	{
		imgInfo.imageRightFilename	= iniFile->getString("image", "filenameRight");
		imgInfo.imageLeftFilename	= iniFile->getString("image", "filenameLeft");
		imgInfo.imageTopFilename	= iniFile->getString("image", "filenameTop");
		imgInfo.imageBottomFilename	= iniFile->getString("image", "filenameBottom");
		imgInfo.imageBackFilename	= iniFile->getString("image", "filenameBack");
		imgInfo.imageFrontFilename	= iniFile->getString("image", "filenameFront");

		imgInfo.folder = folder;

		std::string imageRightFullPath	= imgInfo.folder.empty() ? imgInfo.imageRightFilename	: (imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageRightFilename);
		std::string imageLeftFullPath	= imgInfo.folder.empty() ? imgInfo.imageLeftFilename	: (imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageLeftFilename);
		std::string imageTopFullPath	= imgInfo.folder.empty() ? imgInfo.imageTopFilename		: (imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageTopFilename);
		std::string imageBottomFullPath = imgInfo.folder.empty() ? imgInfo.imageBottomFilename	: (imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageBottomFilename);
		std::string imageBackFullPath	= imgInfo.folder.empty() ? imgInfo.imageBackFilename	: (imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageBackFilename);
		std::string imageFrontFullPath	= imgInfo.folder.empty() ? imgInfo.imageFrontFilename	: (imgInfo.folder + JFF_SLASH_STRING + imgInfo.imageFrontFilename);

		// Extract other image loading parameters
		bool flipVerticallyRight = true;
		if (iniFile->has("image", "flip-vertically-right"))
			flipVerticallyRight = iniFile->getString("image", "flip-vertically-right") == "true";

		bool flipVerticallyLeft  = true;
		if (iniFile->has("image", "flip-vertically-left"))
			flipVerticallyLeft = iniFile->getString("image", "flip-vertically-left") == "true";

		bool flipVerticallyTop = true;
		if (iniFile->has("image", "flip-vertically-top"))
			flipVerticallyTop = iniFile->getString("image", "flip-vertically-top") == "true";

		bool flipVerticallyBottom = true;
		if (iniFile->has("image", "flip-vertically-bottom"))
			flipVerticallyBottom = iniFile->getString("image", "flip-vertically-bottom") == "true";

		bool flipVerticallyBack = true;
		if (iniFile->has("image", "flip-vertically-back"))
			flipVerticallyBack = iniFile->getString("image", "flip-vertically-back") == "true";

		bool flipVerticallyFront = true;
		if (iniFile->has("image", "flip-vertically-front"))
			flipVerticallyFront = iniFile->getString("image", "flip-vertically-front") == "true";

		// Get the image data and check if it's valid
		std::shared_ptr<Image> imageRight	= io->loadImage(imageRightFullPath.c_str(),  flipVerticallyRight,	HDRImage);
		std::shared_ptr<Image> imageLeft	= io->loadImage(imageLeftFullPath.c_str(),	 flipVerticallyLeft,	HDRImage);
		std::shared_ptr<Image> imageTop		= io->loadImage(imageTopFullPath.c_str(),	 flipVerticallyTop,		HDRImage);
		std::shared_ptr<Image> imageBottom	= io->loadImage(imageBottomFullPath.c_str(), flipVerticallyBottom,	HDRImage);
		std::shared_ptr<Image> imageBack	= io->loadImage(imageBackFullPath.c_str(),	 flipVerticallyBack,	HDRImage);
		std::shared_ptr<Image> imageFront	= io->loadImage(imageFrontFullPath.c_str(),  flipVerticallyFront,	HDRImage);
		
		// Generate the texture using OpenGL commands
		generate(imageLeft, imageRight, imageTop, imageBottom, imageFront, imageBack,
			wrapU, wrapV, wrapW, minFilter, magFilter, texFormat);
	}

	// Set cache name. Asset filepath can be an unique name for cubemap caching
	cacheName = generateCacheName(assetFilePath);
}

JFF::CubemapGLSTBI::CubemapGLSTBI(Engine* const engine, const Params& params) :
	engine(engine),

	cacheName(),
	cube(0u),
	imgInfo(),

	isDestroyed(false)
{
	JFF_LOG_INFO("Ctor CubemapGLSTBI")

	imgInfo.folder = params.folder;
	imgInfo.shaderVariableName = params.shaderVariableName;

	// Extract the rest of texture paramters from params
	imgInfo.numMipmapsGenerated = params.numMipmapsGenerated;
	GLint wrapU					= extractWrapOption(params.coordsWrapMode.u);
	GLint wrapV					= extractWrapOption(params.coordsWrapMode.v);
	GLint wrapW					= extractWrapOption(params.coordsWrapMode.w);
	GLint minFilter				= extractMinFilterOption(params.filterMode.minFilter);
	GLint magFilter				= extractMagFilterOption(params.filterMode.magFilter);
	GLint texFormat				= extractTextureFormatOption(params.numColorChannels, params.specialFormat);

	// Generate the texture using OpenGL commands
	generate(params.imgLeft, params.imgRight, params.imgTop, params.imgBottom, params.imgFront, params.imgBack,
		wrapU, wrapV, wrapW, minFilter, magFilter, texFormat);

	// Generate cache name
	std::string imgRightPath	= params.imgRight->data().filepath;
	std::string imgLeftPath		= params.imgLeft->data().filepath;
	std::string imgTopPath		= params.imgTop->data().filepath;
	std::string imgBottomPath	= params.imgBottom->data().filepath;
	std::string imgBackPath		= params.imgBack->data().filepath;
	std::string imgFrontPath	= params.imgFront->data().filepath;

	cacheName = generateCacheName(imgRightPath.c_str(), imgLeftPath.c_str(), imgTopPath.c_str(), 
		imgBottomPath.c_str(), imgBackPath.c_str(), imgFrontPath.c_str(),
		params.coordsWrapMode, params.filterMode, params.numColorChannels, params.specialFormat, params.numMipmapsGenerated);
}

JFF::CubemapGLSTBI::~CubemapGLSTBI()
{
	JFF_LOG_INFO("Dtor CubemapGLSTBI")

	// Ensure the cubemap GPU memory is destroyed
	if (!isDestroyed)
	{
		JFF_LOG_WARNING("Cubemap GPU memory successfully destroyed on Cubemap's destructor. You should call destroy() before destructor is called")
		destroy();
	}
}

std::string JFF::CubemapGLSTBI::getCacheName() const
{
	return cacheName;
}

void JFF::CubemapGLSTBI::writeToFile(const char* newFilename, bool storeInGeneratedSubfolder)
{
	// Select the target texture
	use(0); // Used texture unit 0 because it's not important here

	// Loop over all mipmap levels (mip level 0 is included)
	auto math = engine->math.lock();
	int mipmapLevels = math->clamp(imgInfo.numMipmapsGenerated, 0, 1'000'000); // Don't write auto-generated mipmaps (mipLevel == -1)
	int numChannels  = imgInfo.numChannels;
	bool HDR		 = imgInfo.HDR;
	bool bgra		 = imgInfo.bgra;

	GLenum imgFormat = extractImageFormat(numChannels, bgra);
	GLenum imgType	 = extractImageType(HDR);

	// Full path
	std::ostringstream oss;
	oss << "Assets" << JFF_SLASH;
	if (storeInGeneratedSubfolder)
		oss << "Generated" << JFF_SLASH;
	oss << newFilename;

	std::string relativePath = oss.str();
	std::string pathAppendix[] = { "_posx", "_negx", "_posy", "_negy", "_posz", "_negz" };
	std::string mipAppendix = "_mip";

	for (int mipLevel = 0; mipLevel < mipmapLevels + 1; ++mipLevel)
	{
		// Gather image info before writting to disk
		int width  = imgInfo.width / (int)std::pow(2, mipLevel);
		int height = imgInfo.height / (int)std::pow(2, mipLevel);

		// Read data from OpenGL and write it to disk
		if (HDR)
		{
			float* pixels = new float[(size_t)width * height * numChannels]; // TODO: Keep in mind memory alignment (glPixelStorei())
			for (int i = 0; i < 6; ++i)
			{
				// HDR have .hdr extension
				oss.str("");
				if(mipLevel == 0)
					oss << relativePath << pathAppendix[i] << ".hdr";
				else
					oss << relativePath << pathAppendix[i] << mipAppendix << mipLevel << ".hdr";

				std::string fullPath = oss.str();

				glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevel, imgFormat, imgType, pixels);
				stbi_write_hdr(fullPath.c_str(), width, height, numChannels, pixels);
			}
			delete[] pixels;
		}
		else
		{
			unsigned char* pixels = new unsigned char[(size_t)width * height * numChannels]; // TODO: Keep in mind memory alignment (glPixelStorei())
			for (int i = 0; i < 6; ++i)
			{
				// Non HDR have .png extension
				oss.str("");
				if(mipLevel == 0)
					oss << relativePath << pathAppendix[i] << ".png";
				else
					oss << relativePath << pathAppendix[i] << mipAppendix << mipLevel << ".png";

				std::string fullPath = oss.str();

				glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevel, imgFormat, imgType, pixels);
				stbi_write_png(fullPath.c_str(), width, height, numChannels, pixels, /* Stride between rows */ 0);
			}
			delete[] pixels;
		}
	}
}

void JFF::CubemapGLSTBI::use(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);
}

void JFF::CubemapGLSTBI::destroy()
{
	glDeleteTextures(1, &cube);
	isDestroyed = true;
}

std::string JFF::CubemapGLSTBI::getName() const
{
	return imgInfo.shaderVariableName;
}

JFF::Cubemap::ImageInfo JFF::CubemapGLSTBI::getImageInfo() const
{
	return imgInfo;
}

inline GLenum JFF::CubemapGLSTBI::extractImageFormat(const std::shared_ptr<Image>& image) const
{
	const Image::Data& imgData = image->data();
	return extractImageFormat(imgData.desiredNumChannels, imgData.bgra);
}

inline GLenum JFF::CubemapGLSTBI::extractImageFormat(int numChannels, bool bgra) const
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

inline GLenum JFF::CubemapGLSTBI::extractImageType(const std::shared_ptr<Image>& image) const
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

inline GLenum JFF::CubemapGLSTBI::extractImageType(bool isHDR) const
{
	return isHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;
}

inline const void* JFF::CubemapGLSTBI::extractImagePixels(const std::shared_ptr<Image>& image) const
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

inline GLint JFF::CubemapGLSTBI::extractWrapOption(const std::string& option) const
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

	return GL_CLAMP_TO_EDGE;
}

inline GLint JFF::CubemapGLSTBI::extractWrapOption(Cubemap::Wrap option) const
{
	switch (option)
	{
	case JFF::Cubemap::Wrap::REPEAT:
		return GL_REPEAT;
	case JFF::Cubemap::Wrap::MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;
	case JFF::Cubemap::Wrap::CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;
	case JFF::Cubemap::Wrap::CLAMP_TO_EDGE:
	default:
		return GL_CLAMP_TO_EDGE;
	}
}

inline GLint JFF::CubemapGLSTBI::extractMinFilterOption(const std::string& option)
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
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_NEAREST_MIPMAP_NEAREST;
	}
	else if (option == "linear-nearestMip")
	{
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_LINEAR_MIPMAP_NEAREST;
	}
	else if (option == "nearest-linearMip")
	{
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_NEAREST_MIPMAP_LINEAR;
	}
	else if (option == "linear-linearMip")
	{
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_LINEAR_MIPMAP_LINEAR;
	}

	return GL_LINEAR;
}

inline GLint JFF::CubemapGLSTBI::extractMinFilterOption(Cubemap::MinificationFilter option)
{
	switch (option)
	{
	case JFF::Cubemap::MinificationFilter::NEAREST:
		return GL_NEAREST;
	case JFF::Cubemap::MinificationFilter::NEAREST_NEAREST_MIP:
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_NEAREST_MIPMAP_NEAREST;
	case JFF::Cubemap::MinificationFilter::LINEAR_NEAREST_MIP:
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_LINEAR_MIPMAP_NEAREST;
	case JFF::Cubemap::MinificationFilter::NEAREST_LINEAR_MIP:
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_NEAREST_MIPMAP_LINEAR;
	case JFF::Cubemap::MinificationFilter::LINEAR_LINEAR_MIP:
		if (imgInfo.numMipmapsGenerated == 0)
		{
			JFF_LOG_WARNING("Attempting to create a cubemap with mip filtering without generating mipmaps. This cubemap may not work properly")
		}
		return GL_LINEAR_MIPMAP_LINEAR;
	case JFF::Cubemap::MinificationFilter::LINEAR:
	default:
		return GL_LINEAR;
	}
}

inline GLint JFF::CubemapGLSTBI::extractMagFilterOption(const std::string& option) const
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

inline GLint JFF::CubemapGLSTBI::extractMagFilterOption(Cubemap::MagnificationFilter option) const
{
	switch (option)
	{
	case JFF::Cubemap::MagnificationFilter::NEAREST:
		return GL_NEAREST;
	case JFF::Cubemap::MagnificationFilter::LINEAR:
	default:
		return GL_LINEAR;
	}
}

inline GLint JFF::CubemapGLSTBI::extractTextureFormatOption(int numColorChannels, const std::string& specialFormatOption) const
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

inline GLint JFF::CubemapGLSTBI::extractTextureFormatOption(int numColorChannels, Cubemap::SpecialFormat specialFormat) const
{
	switch (specialFormat)
	{
	case JFF::Cubemap::SpecialFormat::SRGB:
		// Auto transform from sRGB to Linear color space
		if (numColorChannels == 3)
			return GL_SRGB8;
		else
			return GL_SRGB8_ALPHA8;
	case JFF::Cubemap::SpecialFormat::HDR:
		// High precision unbound floating point channel types
		if (numColorChannels == 1)
			return GL_R16F;
		else if (numColorChannels == 2)
			return GL_RG16F;
		else if (numColorChannels == 3)
			return GL_RGB16F;
		else
			return GL_RGBA16F;
	case JFF::Cubemap::SpecialFormat::NONE:
	default:
		if (numColorChannels == 1)
			return GL_R8;
		else if (numColorChannels == 2)
			return GL_RG8;
		else if (numColorChannels == 3)
			return GL_RGB8;
		else
			return GL_RGBA8; // 4 byte color depth is preferred on Windows platform for aligment purposes // TODO: Check
	}
}

inline int JFF::CubemapGLSTBI::extractMipmapOption(const std::string& option) const
{
	int result = 0;
	if (option == "AUTO")
	{
		result = -1;
	}
	else
	{
		try
		{
			result = std::stoi(option);
		}
		catch (std::invalid_argument e)
		{
			JFF_LOG_ERROR("Cannot convert INI file value to int. Exception trace: " << e.what())
		}
		catch (std::out_of_range e)
		{
			JFF_LOG_ERROR("Overflow while converting INI file value to int. Exception trace: " << e.what())
		}
	}
	return result;
}

inline void JFF::CubemapGLSTBI::generate(const std::shared_ptr<Image>& imageLeft,
	const std::shared_ptr<Image>& imageRight, 
	const std::shared_ptr<Image>& imageTop, 
	const std::shared_ptr<Image>& imageBottom, 
	const std::shared_ptr<Image>& imageFront, 
	const std::shared_ptr<Image>& imageBack,
	GLint wrapU, GLint wrapV, GLint wrapW, 
	GLint minFilter, GLint magFilter,
	GLint textureFormat)
{
	// Get all image data and check if they are valid
	const Image::Data& imgLeft	 = imageLeft->data();
	const Image::Data& imgRight	 = imageRight->data();
	const Image::Data& imgTop	 = imageTop->data();
	const Image::Data& imgBottom = imageBottom->data();
	const Image::Data& imgFront  = imageFront->data();
	const Image::Data& imgBack	 = imageBack->data();

	if ((!imgLeft.rawData && !imgLeft.rawDataF) || 
		(!imgRight.rawData && !imgRight.rawDataF) || 
		(!imgTop.rawData && !imgTop.rawDataF) || 
		(!imgBottom.rawData && !imgBottom.rawDataF) || 
		(!imgFront.rawData && !imgFront.rawDataF) || 
		(!imgBack.rawData && !imgBack.rawDataF))
	{
		JFF_LOG_ERROR("One or more provided cubemap textures are invalid. Aborted")
		return;
	}

	// Gather some image info
	imgInfo.width		= imgFront.width; // Used imgFront just to take one of 6 images
	imgInfo.height		= imgFront.height;
	imgInfo.numChannels = imgFront.desiredNumChannels;
	imgInfo.HDR			= imgFront.imgChannelType == Image::ImageChannelType::FLOAT;
	imgInfo.bgra		= imgFront.bgra;

	// Generate cubemap object and bind it to work with it
	glGenTextures(1, &cube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);

	// Apply texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapU);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapV);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapW);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

	// Load cubemap faces
	loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, imageRight,	textureFormat, /* Mipmap 0 */ 0);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, imageLeft,	textureFormat, /* Mipmap 0 */ 0);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, imageTop,	textureFormat, /* Mipmap 0 */ 0);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, imageBottom, textureFormat, /* Mipmap 0 */ 0);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, imageBack,	textureFormat, /* Mipmap 0 */ 0); // Careful with front and back!
	loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, imageFront,	textureFormat, /* Mipmap 0 */ 0); // Careful with front and back!

	// Generate mipmaps automatically for this cubemap. In case of manual mipmap loading, this function will reserve mipmap memory space
	if (imgInfo.numMipmapsGenerated < 0 || imgInfo.numMipmapsGenerated >= 1)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Load mipmaps manually
	if (imgInfo.numMipmapsGenerated >= 1)
	{
		auto io = engine->io.lock();
		for (int mipmap = 1; mipmap <= imgInfo.numMipmapsGenerated; ++mipmap)
		{
			bool flipVertically = false; // Don't flip vertically generated images
			bool HDRImage = imgInfo.HDR;
			std::string mipSuffix = "_mip" + std::to_string(mipmap) + ".";

			std::string imageMipRightPath	= std::regex_replace(imgRight.filepath,  std::regex(R"raw(\.)raw"), mipSuffix);
			std::string imageMipLeftPath	= std::regex_replace(imgLeft.filepath,	 std::regex(R"raw(\.)raw"), mipSuffix);
			std::string imageMipTopPath		= std::regex_replace(imgTop.filepath,	 std::regex(R"raw(\.)raw"), mipSuffix);
			std::string imageMipBottomPath	= std::regex_replace(imgBottom.filepath, std::regex(R"raw(\.)raw"), mipSuffix);
			std::string imageMipBackPath	= std::regex_replace(imgBack.filepath,	 std::regex(R"raw(\.)raw"), mipSuffix);
			std::string imageMipFrontPath	= std::regex_replace(imgFront.filepath,  std::regex(R"raw(\.)raw"), mipSuffix);

			std::shared_ptr<Image> imageMipRight	= io->loadImage(imageMipRightPath.c_str(),	flipVertically, HDRImage);
			std::shared_ptr<Image> imageMipLeft		= io->loadImage(imageMipLeftPath.c_str(),	flipVertically, HDRImage);
			std::shared_ptr<Image> imageMipTop		= io->loadImage(imageMipTopPath.c_str(),	flipVertically, HDRImage);
			std::shared_ptr<Image> imageMipBottom	= io->loadImage(imageMipBottomPath.c_str(), flipVertically, HDRImage);
			std::shared_ptr<Image> imageMipBack		= io->loadImage(imageMipBackPath.c_str(),	flipVertically, HDRImage);
			std::shared_ptr<Image> imageMipFront	= io->loadImage(imageMipFrontPath.c_str(),	flipVertically, HDRImage);

			loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, imageMipRight,	textureFormat, mipmap);
			loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, imageMipLeft,	textureFormat, mipmap);
			loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, imageMipTop,		textureFormat, mipmap);
			loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, imageMipBottom,	textureFormat, mipmap);
			loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, imageMipBack,	textureFormat, mipmap); // Careful with front and back!
			loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, imageMipFront,	textureFormat, mipmap); // Careful with front and back!
		}
	}
}

inline void JFF::CubemapGLSTBI::loadSingleFace(GLenum facePosition, const std::shared_ptr<Image>& image, GLint textureFormat, GLint mipmapLevel)
{
	// Get the image data
	const Image::Data& img = image->data();

	// Fill the cubemap object with image data // TODO: May need to call glPixelStorei(GL_UNPACK_ALIGNMENT, #) ...
	GLsizei width		= img.width;
	GLsizei height		= img.height;
	GLint border		= 0; // Must be zero
	GLenum imageFormat	= extractImageFormat(image);
	GLenum imageType	= extractImageType(image); // The type of each image channel
	const void* pixels	= extractImagePixels(image);

	glTexImage2D(facePosition, mipmapLevel, textureFormat, width, height, border, imageFormat, imageType, pixels);
}
