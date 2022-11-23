#include "TextureGL.h"

#include "Log.h"
#include "Engine.h"

JFF::TextureGL::TextureGL(JFF::Engine* const engine, const char* name, const char* assetFilePath) :
	engine(engine),

	cacheName(),
	tex(0u),
	texName(name),

	mipmapsGenerated(false),
	isDestroyed(false)
{
	JFF_LOG_INFO("Ctor TextureGL")

	auto io = engine->io.lock();

	// Load the ini file that contains the image filename and texture options
	std::shared_ptr<INIFile> iniFile = io->loadINIFile(assetFilePath);

	// Load the image checking if INI file has enough loading info
	std::string imageFilePath = iniFile->getString("texture", "path");
	if (imageFilePath.empty())
	{
		JFF_LOG_ERROR("Failed to load texture. Texture asset doesn't contain a valid image path")
		return;
	}

	// Get the image data and check if it's valid
	std::shared_ptr<Image> image = io->loadImage(imageFilePath.c_str());

	// Extract the rest of texture paramters from INI file
	GLint wrapU = extractWrapOption(iniFile->getString("config", "wrapU"));
	GLint wrapV = extractWrapOption(iniFile->getString("config", "wrapV"));
	GLint wrapW = extractWrapOption(iniFile->getString("config", "wrapW"));
	GLint minFilter = extractMinFilterOption(iniFile->getString("config", "filter-min"));
	GLint magFilter = extractMagFilterOption(iniFile->getString("config", "filter-mag"));
	GLint textureFormat = extractTextureFormatOption(iniFile->getString("config", "sRGB"));

	// Generate the texture using OpenGL commands
	generate(image, wrapU, wrapV, wrapW, minFilter, magFilter, textureFormat);

	// Set cache name. Asset filepath can be an unique name for texture caching
	cacheName = generateCacheName(assetFilePath);
}

JFF::TextureGL::TextureGL(Engine* const engine, const char* name, const std::shared_ptr<Image>& image,
	const Texture::CoordsWrapMode& coordsWrapMode, const Texture::FilterMode& filterMode, bool isSRGBTexture) :
	engine(engine),

	cacheName(),
	tex(0u),
	texName(name),
	mipmapsGenerated(false),
	isDestroyed(false)
{
	JFF_LOG_INFO("Ctor TextureGL")

	// Extract the rest of texture paramters from INI file
	GLint wrapU = extractWrapOption(coordsWrapMode.u);
	GLint wrapV = extractWrapOption(coordsWrapMode.v);
	GLint wrapW = extractWrapOption(coordsWrapMode.w);
	GLint minFilter = extractMinFilterOption(filterMode.minFilter);
	GLint magFilter = extractMagFilterOption(filterMode.magFilter);
	GLint textureFormat = extractTextureFormatOption(isSRGBTexture);

	// Generate the texture using OpenGL commands
	generate(image, wrapU, wrapV, wrapW, minFilter, magFilter, textureFormat);

	// Generate cache name
	cacheName = generateCacheName(image->data().filepath.c_str(), coordsWrapMode, filterMode);
}

JFF::TextureGL::~TextureGL()
{
	JFF_LOG_INFO("Dtor TextureGL")

	// Ensure the texture GPU memory is destroyed
	if (!isDestroyed)
	{
		JFF_LOG_WARNING("Texture GPU memory successfully destroyed on Texture's destructor. You should call destroy() before destructor is called")
		destroy();
	}
}

std::string JFF::TextureGL::getCacheName() const
{
	return cacheName;
}

void JFF::TextureGL::use(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void JFF::TextureGL::destroy()
{
	glDeleteTextures(1, &tex);
	isDestroyed = true;
}

std::string JFF::TextureGL::getName() const
{
	return texName;
}

inline GLenum JFF::TextureGL::extractImageFormat(int numChannels) const
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

inline GLint JFF::TextureGL::extractWrapOption(const std::string& option) const
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

inline GLint JFF::TextureGL::extractWrapOption(Texture::Wrap option) const
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

inline GLint JFF::TextureGL::extractMinFilterOption(const std::string& option) 
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

inline GLint JFF::TextureGL::extractMinFilterOption(Texture::MinificationFilter option)
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

inline GLint JFF::TextureGL::extractMagFilterOption(const std::string& option) const
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

inline GLint JFF::TextureGL::extractMagFilterOption(Texture::MagnificationFilter option) const
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

inline GLint JFF::TextureGL::extractTextureFormatOption(const std::string& option) const
{
	/*
	* If the texture is in sRGB (the artist manually gamma corrected the colors working viewing a monitor),
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

inline GLint JFF::TextureGL::extractTextureFormatOption(bool isSRGBTexture) const
{
	return isSRGBTexture ? GL_SRGB_ALPHA : GL_RGBA8;
}

inline void JFF::TextureGL::generate(const std::shared_ptr<Image>& image, 
	GLint wrapU, GLint wrapV, GLint wrapW, 
	GLint minFilter, GLint magFilter, GLint textureFormat)
{
	// Get the image data and check if it's valid
	const Image::Data& img = image->data();
	if (!img.rawData)
	{
		JFF_LOG_ERROR("Failed to load texture. The provided image isn't valid")
		return;
	}

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
	GLint mipmapLevel = 0; // Mipmap level (each mipmap can be loaded one by one or automatically, using glGenerateMipmap()) 
	GLsizei width = img.width;
	GLsizei height = img.height;
	GLint border = 0; // Must be zero
	GLenum imageFormat = extractImageFormat(img.desiredNumChannels);
	GLenum imageType = GL_UNSIGNED_BYTE; // The type of each image channel
	const void* pixels = img.rawData;

	glTexImage2D(GL_TEXTURE_2D, mipmapLevel, textureFormat, width, height, border, imageFormat, imageType, pixels);

	// Generate mipmaps automatically for this texture
	if (mipmapsGenerated)
		glGenerateMipmap(GL_TEXTURE_2D);
}
