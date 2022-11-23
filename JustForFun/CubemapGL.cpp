#include "CubemapGL.h"

#include "Log.h"
#include "Engine.h"

JFF::CubemapGL::CubemapGL(Engine* const engine, const char* name, const char* assetFilePath) :
	engine(engine),

	cacheName(),
	cube(0u),
	cubeName(name),

	mipmapsGenerated(false),
	isDestroyed(false)
{
	JFF_LOG_INFO("Ctor CubemapGL")

	auto io = engine->io.lock();

	// Load the ini file that contains the image filename and cubemap options
	std::shared_ptr<INIFile> iniFile = io->loadINIFile(assetFilePath);

	// Extract cubemap paramters from INI file
	GLint wrapU = extractWrapOption(iniFile->getString("config", "wrapU"));
	GLint wrapV = extractWrapOption(iniFile->getString("config", "wrapV"));
	GLint wrapW = extractWrapOption(iniFile->getString("config", "wrapW"));
	GLint minFilter = extractMinFilterOption(iniFile->getString("config", "filter-min"));
	GLint magFilter = extractMagFilterOption(iniFile->getString("Config", "filter-mag"));
	GLint texFormat = extractTextureFormatOption(iniFile->getString("config", "sRGB"));

	// Loading images has two options: All cubemap info included in a single image or each cubemap face has a unique image
	if (iniFile->has("cubemap", "path")) // Load single image containing all texture faces
	{
		std::string imageFilePath = iniFile->getString("cubemap", "path");

		// Get the image data and check if it's valid
		std::shared_ptr<Image> image = io->loadImage(imageFilePath.c_str()); // TODO: Test. Flip vertically ??

		// Generate the texture using OpenGL commands
		generate(image, wrapU, wrapV, wrapW, minFilter, magFilter, texFormat);
	}
	else // Load one image per cube face
	{
		std::string imageLeftPath = iniFile->getString("cubemap", "pathLeft");
		std::string imageRightPath = iniFile->getString("cubemap", "pathRight");
		std::string imageTopPath = iniFile->getString("cubemap", "pathTop");
		std::string imageBottomPath = iniFile->getString("cubemap", "pathBottom");
		std::string imageFrontPath = iniFile->getString("cubemap", "pathFront");
		std::string imageBackPath = iniFile->getString("cubemap", "pathBack");

		// Get the image data and check if it's valid
		const bool noVerticalFlip = false; // Don't flip vertically those images on load
		std::shared_ptr<Image> imageLeft = io->loadImage(imageLeftPath.c_str(), noVerticalFlip);
		std::shared_ptr<Image> imageRight = io->loadImage(imageRightPath.c_str(), noVerticalFlip);
		std::shared_ptr<Image> imageTop = io->loadImage(imageTopPath.c_str(), noVerticalFlip);
		std::shared_ptr<Image> imageBottom = io->loadImage(imageBottomPath.c_str(), noVerticalFlip);
		std::shared_ptr<Image> imageFront = io->loadImage(imageFrontPath.c_str(), noVerticalFlip);
		std::shared_ptr<Image> imageBack = io->loadImage(imageBackPath.c_str(), noVerticalFlip);
		
		// Generate the texture using OpenGL commands
		generate(imageLeft, imageRight, imageTop, imageBottom, imageFront, imageBack,
			wrapU, wrapV, wrapW, minFilter, magFilter, texFormat);
	}

	// Set cache name. Asset filepath can be an unique name for cubemap caching
	cacheName = generateCacheName(assetFilePath);
}

JFF::CubemapGL::~CubemapGL()
{
	JFF_LOG_INFO("Dtor CubemapGL")

	// Ensure the cubemap GPU memory is destroyed
	if (!isDestroyed)
	{
		JFF_LOG_WARNING("Cubemap GPU memory successfully destroyed on Cubemap's destructor. You should call destroy() before destructor is called")
		destroy();
	}
}

std::string JFF::CubemapGL::getCacheName() const
{
	return cacheName;
}

void JFF::CubemapGL::use(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);
}

void JFF::CubemapGL::destroy()
{
	glDeleteTextures(1, &cube);
	isDestroyed = true;
}

std::string JFF::CubemapGL::getName() const
{
	return cubeName;
}

inline GLenum JFF::CubemapGL::extractImageFormat(int numChannels) const
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

inline GLint JFF::CubemapGL::extractWrapOption(const std::string& option) const
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

inline GLint JFF::CubemapGL::extractMinFilterOption(const std::string& option)
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

inline GLint JFF::CubemapGL::extractMagFilterOption(const std::string& option) const
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

inline GLint JFF::CubemapGL::extractTextureFormatOption(const std::string& option) const
{
	/*
	* If the cubemap is in sRGB (the artist manually gamma corrected the colors working viewing a monitor),
	* we re-correct sRGB to work in linear space
	*/

	if (option == "true")
	{
		return GL_SRGB_ALPHA; // Auto transform from sRGB to Linear color space
	}
	else if (option == "false")
	{
		return GL_RGBA8; // 4 byte color depth is preferred on Windows platform for aligment purposes // TODO: Check 
	}

	return GL_RGBA8;
}

inline void JFF::CubemapGL::generate(const std::shared_ptr<Image>& image,
	GLint wrapU, GLint wrapV, GLint wrapW, 
	GLint minFilter, GLint magFilter,
	GLint textureFormat)
{
	// TODO: generate cubemap from single image
	JFF_LOG_WARNING("Cannot load a cubemap from a single image. Not implemented method (WIP)") // TODO: remove
}

inline void JFF::CubemapGL::generate(const std::shared_ptr<Image>& imageLeft,
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
	const Image::Data& imgLeft = imageLeft->data();
	const Image::Data& imgRight = imageRight->data();
	const Image::Data& imgTop = imageTop->data();
	const Image::Data& imgBottom = imageBottom->data();
	const Image::Data& imgFront = imageFront->data();
	const Image::Data& imgBack = imageBack->data();

	if ((!imgLeft.rawData) || (!imgRight.rawData) || 
		(!imgTop.rawData) || (!imgBottom.rawData) || 
		(!imgFront.rawData) || (!imgBack.rawData))
	{
		JFF_LOG_ERROR("One or more provided cubemap textures are invalid. Aborted")
		return;
	}

	// Generate cubemap object and bind it to work with it
	glGenTextures(1, &cube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);

	// Apply texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapU);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapV);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapW);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);

	loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, imgRight, textureFormat);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, imgLeft, textureFormat);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, imgTop, textureFormat);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, imgBottom, textureFormat);
	loadSingleFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, imgBack, textureFormat); // Careful with front and back!
	loadSingleFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, imgFront, textureFormat); // Careful with front and back!

	// Generate mipmaps automatically for this cubemap
	if (mipmapsGenerated)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

inline void JFF::CubemapGL::loadSingleFace(GLenum facePosition, const Image::Data& img, GLint textureFormat)
{
	// Fill the cubemap object with image data // TODO: May need to call glPixelStorei(GL_UNPACK_ALIGNMENT, #) ...
	GLint mipmapLevel = 0; // Mipmap level (each mipmap can be loaded one by one or automatically, using glGenerateMipmap()) 
	GLsizei width = img.width;
	GLsizei height = img.height;
	GLint border = 0; // Must be zero
	GLenum imageFormat = extractImageFormat(img.desiredNumChannels);
	GLenum imageType = GL_UNSIGNED_BYTE; // The type of each image channel
	const void* pixels = img.rawData;

	glTexImage2D(facePosition, mipmapLevel, textureFormat, width, height, border, imageFormat, imageType, pixels);
}
