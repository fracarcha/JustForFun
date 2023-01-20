/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "FramebufferGLSTBI.h"

#include "Log.h"

#include "stb_image_write.h"
#include "FileSystemSetup.h"

#include <sstream>
#include <vector>
#include <stdexcept>
#include <cmath>

JFF::FramebufferGLSTBI::FramebufferGLSTBI(PrefabFramebuffer fboType, unsigned int width, unsigned int height, unsigned int samplesPerPixel) :
	isDestroyed(false),

	samplesPerPixel(0u),
	mainFBO(),
	auxFBO(),

	mainFBOColorBuffersUsed(),

	clearMask(0u)
{
	JFF_LOG_INFO("Ctor FramebufferGLSTBI")

	extractParamsData(fboType, width, height, samplesPerPixel);
	create();
}

JFF::FramebufferGLSTBI::FramebufferGLSTBI(const Params& params) :
	isDestroyed(false),

	samplesPerPixel(0u),
	mainFBO(),
	auxFBO(),

	mainFBOColorBuffersUsed(),

	clearMask(0u)
{
	JFF_LOG_INFO("Ctor FramebufferGLSTBI")

	extractParamsData(params);
	create();
}

JFF::FramebufferGLSTBI::~FramebufferGLSTBI()
{
	JFF_LOG_INFO("Dtor FramebufferGLSTBI")

	if (!isDestroyed)
	{
		JFF_LOG_WARNING("Framebuffer GPU memory successfully destroyed on Framebuffer's destructor. However, you should call destroy() before destructor is called")
		destroy();
	}
}

void JFF::FramebufferGLSTBI::writeToFile(const char* newFilename, bool storeInGeneratedSubfolder)
{
	// Select main FBO or aux FBO depending if this framebuffer is multisample
	if (samplesPerPixel > 1)
	{
		// Resolve buffers in case of multisample framebuffer
		//disable();

		JFF_LOG_ERROR("Cannot write multisample framebuffer to disk. Not implemented")
		return;
		// TODO: Implement
	}

	// Use main framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO.fbo);

	// Full path
	std::ostringstream oss;
	oss << "Assets" << JFF_SLASH;
	if (storeInGeneratedSubfolder)
		oss << "Generated" << JFF_SLASH;
	oss << newFilename;

	std::string relativePath = oss.str();

	// TODO: mipmaps

	// Loop over all attachment points and create a file with their content
	for (auto& pair : mainFBO.fboAttachments)
	{
		if (pair.second.texType == TextureType::CUBEMAP)
		{
			JFF_LOG_WARNING("Cannot write to disk a framebuffer channel based in a cubemap texture")
			continue;
		}

		if (pair.first == AttachmentPoint::DEPTH || pair.first == AttachmentPoint::STENCIL || pair.first == AttachmentPoint::DEPTH_STENCIL)
		{
			continue; // Cannot write to disk framebuffer channels: depth, stencil and depth_stencil
		}

		glReadBuffer(GL_COLOR_ATTACHMENT0 + (char)pair.first);

		if (pair.second.HDR)
		{
			/* 
			* NOTE: As STBI documentation says: 
			* 
			* HDR expects linear float data. Since the format is always 32 - bit rgb(e)
			* data, alpha (if provided) is discarded, and for monochrome data it is
			* replicated across all three channels.
			*/

			/*
			* WARNING: STBI can't store negative floats on disk. Don not use this function to store textures like
			* position or normals
			*/

			// HDR have .hdr extension
			oss.str("");
			if (mainFBO.fboAttachments.size() == 1)
				oss << relativePath << ".hdr";
			else
				oss << relativePath << "_color_" << (int) pair.first << ".hdr";

			std::string fullPath = oss.str();

			unsigned int width	= pair.second.width;
			unsigned int height = pair.second.height;
			int numChannels		= pair.second.numColorChannels;
			GLenum format		= imgFormatToGL(pair.first, numChannels);
			GLenum type			= GL_FLOAT;
			float* pixels		= new float[(size_t) width * height * numChannels]; // TODO: Keep in mind memory alignment (glPixelStorei())

			glReadPixels(0, 0, width, height, format, type, pixels);
			stbi_write_hdr(fullPath.c_str(), width, height, numChannels, pixels);

			delete[] pixels;
		}
		else
		{
			// Non HDR have .png extension
			oss.str("");
			if (mainFBO.fboAttachments.size() == 1)
				oss << relativePath << ".png";
			else
				oss << relativePath << "_color_" << (int) pair.first << ".png";

			std::string fullPath = oss.str();

			unsigned int width	  = pair.second.width;
			unsigned int height   = pair.second.height;
			int numChannels		  = pair.second.numColorChannels;
			GLenum format		  = imgFormatToGL(pair.first, numChannels);
			GLenum type			  = GL_UNSIGNED_BYTE;
			unsigned char* pixels = new unsigned char[(size_t) width * height * numChannels]; // TODO: Keep in mind memory alignment (glPixelStorei())
					
			glReadPixels(0, 0, width, height, format, type, pixels);
			stbi_write_png(fullPath.c_str(), width, height, numChannels, pixels, /* Stride between rows */ 0);

			delete[] pixels;
		}
		
	} // End for loop

	// We have to tell OpenGL we are rendering to multiple color buffers or none of them
	configureReadAndWriteColorBuffers();

	// Bind main screen framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

void JFF::FramebufferGLSTBI::enable(bool clearBuffers)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO.fbo);

	if (clearBuffers)
		glClear(clearMask); // Clear FBO buffers before use
}

void JFF::FramebufferGLSTBI::disable()
{
	if (samplesPerPixel > 1) // Do this on multisample framebuffers only
	{
		/*
		* Dump from fboMultisampling to fbo to allow sampling the texture
		* NOTE: You can sample from multisampling texture, but you need to create sampler2DMS on shader...
		*
		* The process of converting a multisample texture on a normal texture applying anti-aliasing is called "resolve"
		* The process of resolving a HDR multisample texture need more work, which isn't done here. So if HDR is used with
		* multisampling, take care when powerful lights hits a scene, because it can "remove" the effect of anti-aliasing.
		*/
		// TODO: There's a cool article about multisampling and HDR-multisample phenomenon and how to fix it here: https://mynameismjp.wordpress.com/2012/10/24/msaa-overview/
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFBO.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, auxFBO.fbo);

		// Dump data individually because glBlitFramebuffer() only dumps one color attachment at a time
		for (auto& pair : mainFBO.fboAttachments)
		{
			// Copy only texture attachments and ignore renderbuffers. The objective of this dump is to have sampleable textures
			if (pair.second.renderBuffer)
				continue;

			unsigned int readWidth	 = mainFBO.fboAttachments[pair.first].width;
			unsigned int readHeight	 = mainFBO.fboAttachments[pair.first].height;
			unsigned int writeWidth	 = auxFBO.fboAttachments[pair.first].width;
			unsigned int writeHeight = auxFBO.fboAttachments[pair.first].height;

			if (pair.first == AttachmentPoint::DEPTH)
			{
				glBlitFramebuffer(0, 0, readWidth, readHeight, 0, 0, writeWidth, writeHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); // Dump data
			}
			else if (pair.first == AttachmentPoint::STENCIL)
			{
				glBlitFramebuffer(0, 0, readWidth, readHeight, 0, 0, writeWidth, writeHeight, GL_STENCIL_BUFFER_BIT, GL_NEAREST); // Dump data
			}
			else if (pair.first == AttachmentPoint::DEPTH_STENCIL)
			{
				glBlitFramebuffer(0, 0, readWidth, readHeight, 0, 0, writeWidth, writeHeight, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST); // Dump data
			}
			else // Color attachment points
			{
				glReadBuffer(GL_COLOR_ATTACHMENT0 + (char) pair.first);
				glDrawBuffer(GL_COLOR_ATTACHMENT0 + (char) pair.first);

				glBlitFramebuffer(0, 0, readWidth, readHeight, 0, 0, writeWidth, writeHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST); // Dump data
			}

		}
	}

	// Bind main screen framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void JFF::FramebufferGLSTBI::useTexture(AttachmentPoint attachmentPoint, int textureUnit)
{
	try
	{
		AttachmentDataInternal& attachmentData = samplesPerPixel > 1 ? 
			auxFBO.fboAttachments.at(attachmentPoint) : mainFBO.fboAttachments.at(attachmentPoint);

		if (attachmentData.renderBuffer)
		{
			JFF_LOG_WARNING("Cannot use render buffers as textures. Aborted")
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(texTypeToGL(attachmentData.texType), attachmentData.buffer);
		}
	}
	catch (std::out_of_range e)
	{
		JFF_LOG_WARNING("Attachment point not found on current Framebuffer. Aborted")
	}
}

void JFF::FramebufferGLSTBI::setSize(unsigned int width, unsigned int height)
{
	for (auto& pair : mainFBO.fboAttachments)
	{
		pair.second.width = width;
		pair.second.height = height;
	}

	if (samplesPerPixel > 1)
	{
		for (auto& pair : auxFBO.fboAttachments)
		{
			pair.second.width = width;
			pair.second.height = height;
		}
	}

	destroy(); // Destroy previous buffers
	isDestroyed = false; // Cancel destroyed flag set by destroy()

	create(); // Create new buffers with the new size
}

void JFF::FramebufferGLSTBI::getSize(AttachmentPoint attachmentPoint, unsigned int& width, unsigned int& height)
{
	try
	{
		AttachmentDataInternal& attachmentData = mainFBO.fboAttachments.at(attachmentPoint);

		if (attachmentData.mipmapLevel > 0)
		{
			width = attachmentData.width / (unsigned int) std::pow(2, attachmentData.mipmapLevel);
			height = attachmentData.height / (unsigned int) std::pow(2, attachmentData.mipmapLevel);
		}
		else
		{
			width = attachmentData.width;
			height = attachmentData.height;
		}
	}
	catch (std::out_of_range e)
	{
		JFF_LOG_WARNING("Attachment point not found on current Framebuffer. Aborted")
	}
}

void JFF::FramebufferGLSTBI::copyBuffer(
	AttachmentPoint dstAttachmentPoint, 
	AttachmentPoint srcAttachmentPoint,
	std::weak_ptr<Framebuffer> src)
{
	std::shared_ptr<FramebufferGLSTBI> srcGL = std::dynamic_pointer_cast<FramebufferGLSTBI>(src.lock());
	if (srcGL)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, srcGL->mainFBO.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->mainFBO.fbo);

		// Use width and height of this FBO's attachment point. Sizes on both buffers should be equal
		unsigned int w = mainFBO.fboAttachments[dstAttachmentPoint].width;
		unsigned int h = mainFBO.fboAttachments[dstAttachmentPoint].height;

		if (dstAttachmentPoint == AttachmentPoint::DEPTH)
		{
			if (srcAttachmentPoint != AttachmentPoint::DEPTH)
			{
				JFF_LOG_ERROR("When using copyBuffer() to copy DEPTH, both dstAttachmentPoint and srcAttachmentPoint should be AttachmentPoint::DEPTH. Aborted")
				return;
			}

			glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		}
		else if (dstAttachmentPoint == AttachmentPoint::STENCIL)
		{
			if (srcAttachmentPoint != AttachmentPoint::STENCIL)
			{
				JFF_LOG_ERROR("When using copyBuffer() to copy STENCIL, both dstAttachmentPoint and srcAttachmentPoint should be AttachmentPoint::STENCIL. Aborted")
				return;
			}

			glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		}
		else if (dstAttachmentPoint == AttachmentPoint::DEPTH_STENCIL)
		{
			if (srcAttachmentPoint != AttachmentPoint::DEPTH_STENCIL)
			{
				JFF_LOG_ERROR("When using copyBuffer() to copy DEPTH_STENCIL, both dstAttachmentPoint and srcAttachmentPoint should be AttachmentPoint::DEPTH_STENCIL. Aborted")
				return;
			}

			glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		}
		else
		{
			glReadBuffer(GL_COLOR_ATTACHMENT0 + (char) srcAttachmentPoint);
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + (char) dstAttachmentPoint);

			glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_FRAMEBUFFER, this->mainFBO.fbo);
			this->configureReadAndWriteColorBuffers();

			glBindFramebuffer(GL_FRAMEBUFFER, srcGL->mainFBO.fbo);
			srcGL->configureReadAndWriteColorBuffers();
		}
	}
}

void JFF::FramebufferGLSTBI::destroy()
{
	isDestroyed = true;

	// Delete all memory reserved on GPU

	// Main framebuffer
	glDeleteFramebuffers(1, &mainFBO.fbo);
	for (auto& pair : mainFBO.fboAttachments)
	{
		if (pair.second.renderBuffer)
		{
			glDeleteRenderbuffers(1, &pair.second.buffer);
		}
		else
		{
			glDeleteTextures(1, &pair.second.buffer);
		}
	}
	
	// Auxiliary framebuffer
	if (samplesPerPixel > 1)
	{
		glDeleteFramebuffers(1, &auxFBO.fbo);
		for (auto& pair : auxFBO.fboAttachments)
		{
			if (pair.second.renderBuffer)
			{
				glDeleteRenderbuffers(1, &pair.second.buffer);
			}
			else
			{
				glDeleteTextures(1, &pair.second.buffer);
			}
		}
	}

	// Clear color buffers used on this framebuffer
	mainFBOColorBuffersUsed.clear();
}

inline void JFF::FramebufferGLSTBI::extractParamsData(PrefabFramebuffer fboType, unsigned int width, unsigned int height, unsigned int samplesPerPixel)
{
	switch (fboType)
	{
	case JFF::Framebuffer::PrefabFramebuffer::FBO_PRE_PROCESS_FORWARD:
		{
			AttachmentData textureData;
			textureData.width				= width;
			textureData.height				= height;
			textureData.renderBuffer		= false;
			textureData.texType				= TextureType::TEXTURE_2D;
			textureData.wrapMode			= { Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE };
			textureData.filterMode			= { MinificationFilter::LINEAR, MagnificationFilter::LINEAR };
			textureData.HDR					= true;
			textureData.numColorChannels	= 4;
			textureData.mipmapLevel			= 0;

			AttachmentData renderbufferData;
			renderbufferData.width			= width;
			renderbufferData.height			= height;
			renderbufferData.renderBuffer	= true;

			Params params;
			params.samplesPerPixel = samplesPerPixel;
			params.attachments[AttachmentPoint::COLOR_0]		= textureData;		// Final color channel
			params.attachments[AttachmentPoint::DEPTH_STENCIL]	= renderbufferData; // Depth-stencil renderbuffer

			extractParamsData(params);
		}
		break;
	case JFF::Framebuffer::PrefabFramebuffer::FBO_GEOMETRY_DEFERRED:
		{
			AttachmentData textureDataHighPrecision;
			textureDataHighPrecision.width				= width;
			textureDataHighPrecision.height				= height;
			textureDataHighPrecision.renderBuffer		= false;
			textureDataHighPrecision.texType			= TextureType::TEXTURE_2D;
			textureDataHighPrecision.wrapMode			= { Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE };
			textureDataHighPrecision.filterMode			= { MinificationFilter::LINEAR, MagnificationFilter::LINEAR };
			textureDataHighPrecision.HDR				= true; // High precision (GL_RGBA16F)
			textureDataHighPrecision.numColorChannels	= 4;
			textureDataHighPrecision.mipmapLevel		= 0;

			AttachmentData textureDataLowPrecision = textureDataHighPrecision;
			textureDataLowPrecision.HDR	= false; // Low precision (GL_RGBA)

			AttachmentData renderbufferData;
			renderbufferData.width			= width;
			renderbufferData.height			= height;
			renderbufferData.renderBuffer	= true;

			Params params;
			if (samplesPerPixel > 1)
			{
				JFF_LOG_WARNING("MSAA is incompatible with deferred shading. Switching off MSAA")
			}
			params.samplesPerPixel = 0u;
			params.attachments[AttachmentPoint::COLOR_0] = textureDataHighPrecision; // World positions channel (Need high precision to store fragment positions without [0,1] bounds)
			params.attachments[AttachmentPoint::COLOR_1] = textureDataHighPrecision; // World normals channel
			params.attachments[AttachmentPoint::COLOR_2] = textureDataLowPrecision;	 // Albedo(RGB)/specular(A) channel
			params.attachments[AttachmentPoint::COLOR_3] = textureDataHighPrecision; // Ambient(RGB)/Shininess(A) channel (Need high precision to have tiny colors in ambient and shininess beyond 1.0)
			params.attachments[AttachmentPoint::COLOR_4] = textureDataLowPrecision;	 // Reflection(RGB) channel
			params.attachments[AttachmentPoint::COLOR_5] = textureDataHighPrecision; // Emissive(RGB) channel (Need high precision to have colors beyond 1.0 and have beautiful bloom effects)
			params.attachments[AttachmentPoint::DEPTH_STENCIL] = renderbufferData;	 // Depth-stencil renderbuffer

			extractParamsData(params);
		}
		break;
	case JFF::Framebuffer::PrefabFramebuffer::FBO_LIGHTING_DEFERRED:
		{
			AttachmentData textureDataHighPrecision;
			textureDataHighPrecision.width				= width;
			textureDataHighPrecision.height				= height;
			textureDataHighPrecision.renderBuffer		= false;
			textureDataHighPrecision.texType			= TextureType::TEXTURE_2D;
			textureDataHighPrecision.wrapMode			= { Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE };
			textureDataHighPrecision.filterMode			= { MinificationFilter::LINEAR, MagnificationFilter::LINEAR };
			textureDataHighPrecision.HDR				= true; // High precision (GL_RGBA16F)
			textureDataHighPrecision.numColorChannels	= 4;
			textureDataHighPrecision.mipmapLevel		= 0;

			AttachmentData renderbufferData;
			renderbufferData.width = width;
			renderbufferData.height = height;
			renderbufferData.renderBuffer = true;

			Params params;
			if (samplesPerPixel > 1)
			{
				JFF_LOG_WARNING("MSAA is incompatible with deferred shading. Switching off MSAA")
			}
			params.samplesPerPixel = 0u;
			params.attachments[AttachmentPoint::COLOR_0] = textureDataHighPrecision; // Final color with lights applied
			// Next depth stencil buffer is a forwarded copy of FBO_GEOMETRY_DEFERRED used to make forward shading with translucent objects
			params.attachments[AttachmentPoint::DEPTH_STENCIL] = renderbufferData;	 // Depth-stencil renderbuffer

			extractParamsData(params);
		}
		break;
	case JFF::Framebuffer::PrefabFramebuffer::FBO_POST_PROCESS:
		{
			AttachmentData textureData;
			textureData.width				= width;
			textureData.height				= height;
			textureData.renderBuffer		= false;
			textureData.texType				= TextureType::TEXTURE_2D;
			textureData.wrapMode			= { Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE };
			textureData.filterMode			= { MinificationFilter::NEAREST, MagnificationFilter::NEAREST };
			textureData.HDR					= true; // TODO: Is this precision really needed?
			textureData.numColorChannels	= 4;
			textureData.mipmapLevel			= 0;

			Params params;
			params.samplesPerPixel = 0u;
			params.attachments[AttachmentPoint::COLOR_0] = textureData; // Final color channel

			extractParamsData(params);
		}
		break;
	case JFF::Framebuffer::PrefabFramebuffer::FBO_SHADOW_MAP:
		{
			AttachmentData textureData;
			textureData.width			= width;
			textureData.height			= height;
			textureData.renderBuffer	= false;
			textureData.texType			= TextureType::TEXTURE_2D;
			textureData.wrapMode		= { Wrap::CLAMP_TO_BORDER, Wrap::CLAMP_TO_BORDER, Wrap::CLAMP_TO_BORDER };
			textureData.borderColor		= Vec4(1.0f, 1.0f, 1.0f, 1.0f); // Use white as border color to make objects outside shadow map frustum (in x and y) to not have shadow
			textureData.filterMode		= { MinificationFilter::NEAREST, MagnificationFilter::NEAREST };
			textureData.mipmapLevel		= 0;

			Params params;
			params.samplesPerPixel = 0u;
			params.attachments[AttachmentPoint::DEPTH] = textureData;

			extractParamsData(params);
		}
		break;
	case JFF::Framebuffer::PrefabFramebuffer::FBO_SHADOW_CUBEMAP:
		{
			AttachmentData textureData;
			textureData.width			= width;
			textureData.height			= height;
			textureData.renderBuffer	= false;
			textureData.texType			= TextureType::CUBEMAP;
			textureData.wrapMode		= { Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE, Wrap::CLAMP_TO_EDGE };
			textureData.filterMode		= { MinificationFilter::NEAREST, MagnificationFilter::NEAREST };
			textureData.mipmapLevel		= 0;

			Params params;
			params.samplesPerPixel = 0u;
			params.attachments[AttachmentPoint::DEPTH] = textureData;

			extractParamsData(params);
		}
		break;
	default:
		break;
	}
}

inline void JFF::FramebufferGLSTBI::extractParamsData(const Params& params)
{
	samplesPerPixel = params.samplesPerPixel;

	for (auto& pair : params.attachments)
	{
		AttachmentPoint attachmentPoint = pair.first;
		AttachmentData attachmentData = pair.second;

		AttachmentDataInternal attachmentDataInternal;
		attachmentDataInternal.width			= attachmentData.width;
		attachmentDataInternal.height			= attachmentData.height;
		attachmentDataInternal.renderBuffer		= attachmentData.renderBuffer;
		attachmentDataInternal.texType			= attachmentData.texType;
		attachmentDataInternal.wrapMode			= attachmentData.wrapMode;
		attachmentDataInternal.borderColor		= attachmentData.borderColor;
		attachmentDataInternal.filterMode		= attachmentData.filterMode;
		attachmentDataInternal.HDR				= attachmentData.HDR;
		attachmentDataInternal.numColorChannels = attachmentData.numColorChannels;
		attachmentDataInternal.mipmapLevel		= attachmentData.mipmapLevel;

		// Fill the main fbo with params info
		mainFBO.fboAttachments[attachmentPoint] = attachmentDataInternal;

		// In multisample fbos, texture attachments (renderbuffers are discarded) are replicated in a non multisample fbo
		if (samplesPerPixel > 1 && !attachmentDataInternal.renderBuffer)
			auxFBO.fboAttachments[attachmentPoint] = attachmentDataInternal;

		// Configure OpenGL clear mask depending on attachments
		if (attachmentPoint == AttachmentPoint::DEPTH)
			clearMask |= GL_DEPTH_BUFFER_BIT;
		else if (attachmentPoint == AttachmentPoint::STENCIL)
			clearMask |= GL_STENCIL_BUFFER_BIT;
		else if (attachmentPoint == AttachmentPoint::DEPTH_STENCIL)
			clearMask |= GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		else // Any other color buffer
			clearMask |= GL_COLOR_BUFFER_BIT;
	}
}

inline void JFF::FramebufferGLSTBI::create()
{
	/*
	* In case of multisampling two framebuffers are created: one which receives rendering commands and applies MSAA and
	* another which receives color buffer from multisample framebuffer in order to manage sampling
	* on shaders
	*/

	/*
	* NOTE: Renderbuffers are used because it's more efficient than textures if there's no need to sample it in shaders.
	* The most common use case is to store depth and stencil info, which is used by OpenGL fixed pipeline automatically if
	* depth and stencil are enabled (just like the default framebuffer)
	*/

	for (auto& pair : mainFBO.fboAttachments)
	{
		createMainBuffer(pair.first, pair.second);
	}
	createMainFrameBuffer();

	// Multisample aux buffer creation
	if (samplesPerPixel > 1)
	{
		for (auto& pair : auxFBO.fboAttachments)
		{
			createAuxBuffer(pair.first, pair.second);
		}
		createAuxFrameBuffer();
	}
}


inline void JFF::FramebufferGLSTBI::createMainBuffer(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData)
{
	if (samplesPerPixel > 1) // Multisample buffer
	{
		if (attachmentData.renderBuffer)
		{
			createRenderbufferMultisample(attachmentPoint, attachmentData);
		}
		else
		{
			if (attachmentData.texType == TextureType::CUBEMAP)
			{
				JFF_LOG_WARNING("Cannot create cubemap texture targeting to multisample framebuffer. Aborted")
				return;
			}
			createTexture2DMultisample(attachmentPoint, attachmentData);
		}
	}
	else // Non multisample buffer
	{
		if (attachmentData.renderBuffer)
		{
			createRenderBuffer(attachmentPoint, attachmentData);
		}
		else
		{
			if (attachmentData.texType == TextureType::CUBEMAP)
			{
				createCubemap(attachmentPoint, attachmentData);
			}
			else // attachmentData.texType == TextureType::TEXTURE_2D
			{
				createTexture2D(attachmentPoint, attachmentData);
			}
		}
	}
}

inline void JFF::FramebufferGLSTBI::createMainFrameBuffer()
{
	// Create the framebuffer
	glGenFramebuffers(1, &mainFBO.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO.fbo); // Framebuffers can be bound for read (GL_READ_FRAMEBUFFER) or write (GL_DRAW_FRAMEBUFFER) separately

	// Attach each buffer to their corresponding attachment point into the framebuffer
	for (auto& pair : mainFBO.fboAttachments)
	{
		// Get the attachment point of each buffer
		GLenum attachmentPoint = attachmentPointToGL(pair.first);

		// Store the attachment point if it's a color buffer
		if (pair.first != AttachmentPoint::DEPTH && pair.first != AttachmentPoint::STENCIL && pair.first != AttachmentPoint::DEPTH_STENCIL)
			mainFBOColorBuffersUsed.push_back(attachmentPoint);

		if (samplesPerPixel > 1) // Multisample framebuffer
		{
			if (pair.second.renderBuffer)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentPoint, GL_RENDERBUFFER, pair.second.buffer);
			}
			else 
			{
				if (pair.second.texType == TextureType::TEXTURE_2D)
				{
					if (pair.second.mipmapLevel > 0)
					{
						JFF_LOG_ERROR("Multisample framebuffer doesn't allow textures with mipmap level other than 0. Aborted")
					}
					else
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D_MULTISAMPLE, pair.second.buffer, pair.second.mipmapLevel);
					}
				}
				else // TextureType::CUBEMAP
				{
					JFF_LOG_WARNING("Cubemap textures are not allowed in multisample framebuffers. Framebuffer attachment aborted")
				}
			}
		}
		else // Non multisample framebuffers
		{
			if (pair.second.renderBuffer)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentPoint, GL_RENDERBUFFER, pair.second.buffer);
			}
			else
			{
				if (pair.second.texType == TextureType::TEXTURE_2D)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, pair.second.buffer, pair.second.mipmapLevel);
				}
				else // TextureType::CUBEMAP
				{
					// NOTE: Note that next function is different from glFramebufferTexture2D()
					glFramebufferTexture(GL_FRAMEBUFFER, attachmentPoint, pair.second.buffer, pair.second.mipmapLevel);
				}
			}
		}
	}

	// We have to tell OpenGL we are rendering to multiple color buffers or none of them
	configureReadAndWriteColorBuffers();

	// Check framebuffer status
	checkFramebufferStatus();

	// Set the default framebuffer after this operation
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // 0 is the default framebuffer used by OpenGL to draw on screen
}

inline void JFF::FramebufferGLSTBI::createAuxBuffer(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData)
{
	if (attachmentData.renderBuffer)
	{
		JFF_LOG_WARNING("Cannot create renderbuffer for auxiliary framebuffer")
	}
	else
	{
		if (attachmentData.texType == TextureType::CUBEMAP)
		{
			JFF_LOG_WARNING("Cannot create cubemap texture for auxiliary framebuffer")
		}
		else // attachmentData.texType == TextureType::TEXTURE_2D
		{
			createTexture2D(attachmentPoint, attachmentData);
		}
	}
}

inline void JFF::FramebufferGLSTBI::createAuxFrameBuffer()
{
	// Create the framebuffer
	glGenFramebuffers(1, &auxFBO.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, auxFBO.fbo); // Framebuffers can be bound for read (GL_READ_FRAMEBUFFER) or write (GL_DRAW_FRAMEBUFFER) separately

		// Attach each buffer to their corresponding attachment point into the framebuffer
	std::vector<GLenum> colorBuffersUsed;
	for (auto& pair : auxFBO.fboAttachments)
	{
		// Get the attachment point of each buffer
		GLenum attachmentPoint = attachmentPointToGL(pair.first);

		// Store the attachment point if it's a color buffer
		if (pair.first != AttachmentPoint::DEPTH && pair.first != AttachmentPoint::STENCIL && pair.first != AttachmentPoint::DEPTH_STENCIL)
			colorBuffersUsed.push_back(attachmentPoint);
	
		if (pair.second.renderBuffer)
		{
			JFF_LOG_WARNING("Auxiliary framebuffer cannot use Renderbuffers because the objective of this buffer is to 'resolve' multisample textures")
		}
		else
		{
			if (pair.second.texType == TextureType::TEXTURE_2D)
			{
				if (pair.second.mipmapLevel > 0)
				{
					JFF_LOG_ERROR("Multisample framebuffer doesn't allow textures with mipmap level other than 0. Aborted")
				} 
				else
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, pair.second.buffer, pair.second.mipmapLevel);
				}
			}
			else // TextureType::CUBEMAP
			{
				JFF_LOG_WARNING("Auxiliary framebuffer cannot use Cubemap textures")
			}
		}
	}
	
	// Tell OpenGL that this fbo won't use color attachments. Next lines doesn't affect writing on depth buffer, they only affect color buffers
	if (colorBuffersUsed.size() <= 0)
	{
		// NOTE: Next functions are needed for fbo to be considered "complete" by OpenGL, because one condition is to have at least one color buffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	// NOTE: Since this framebuffer is not used to draw into using glDrawArrays or glDrawElements, there's no need to specify glDrawBuffers()

	// Check framebuffer status
	checkFramebufferStatus();

	// Set the default framebuffer after this operation
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // 0 is the default framebuffer used by OpenGL to draw on screen
}


inline void JFF::FramebufferGLSTBI::createRenderbufferMultisample(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData)
{
	glGenRenderbuffers(1, &attachmentData.buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, attachmentData.buffer);

	// Define the format and size of the renderbuffer
	GLint texFormat = texFormatToGL(attachmentPoint, attachmentData.numColorChannels, attachmentData.HDR);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samplesPerPixel, texFormat, attachmentData.width, attachmentData.height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

inline void JFF::FramebufferGLSTBI::createRenderBuffer(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData)
{
	glGenRenderbuffers(1, &attachmentData.buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, attachmentData.buffer);

	// Define the format and size of the renderbuffer
	GLint texFormat = texFormatToGL(attachmentPoint, attachmentData.numColorChannels, attachmentData.HDR);
	glRenderbufferStorage(GL_RENDERBUFFER, texFormat, attachmentData.width, attachmentData.height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

inline void JFF::FramebufferGLSTBI::createTexture2DMultisample(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData)
{
	glGenTextures(1, &attachmentData.buffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, attachmentData.buffer);

	// NOTE: Texture parameters are incompatible with multisample texture

	GLint texFormat = texFormatToGL(attachmentPoint, attachmentData.numColorChannels, attachmentData.HDR);
	GLboolean fixedSampleLocations = GL_TRUE; // If true, the image will use identical sample locations and the same number of subsamples for each texel

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samplesPerPixel, texFormat, attachmentData.width, attachmentData.height, fixedSampleLocations);

	// NOTE: Texture mipmaps are incompatible with multisample texture

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

inline void JFF::FramebufferGLSTBI::createTexture2D(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData)
{
	glGenTextures(1, &attachmentData.buffer);
	glBindTexture(GL_TEXTURE_2D, attachmentData.buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeToGL(attachmentData.wrapMode.u));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeToGL(attachmentData.wrapMode.v));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapModeToGL(attachmentData.wrapMode.w));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterModeToGL(attachmentData.filterMode.minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterModeToGL(attachmentData.filterMode.magFilter));

	if (attachmentData.wrapMode.u == Wrap::CLAMP_TO_BORDER ||
		attachmentData.wrapMode.v == Wrap::CLAMP_TO_BORDER ||
		attachmentData.wrapMode.w == Wrap::CLAMP_TO_BORDER)
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, *attachmentData.borderColor);
	}

	// Fill the texture object with image data // TODO: May need to call glPixelStorei(GL_UNPACK_ALIGNMENT, #) ...
	GLint mipmapLevel = 0; // TODO: Mipmap level (each mipmap can be loaded one by one or automatically, using glGenerateMipmap()) 
	GLint textureFormat = texFormatToGL(attachmentPoint, attachmentData.numColorChannels, attachmentData.HDR);
	GLint border = 0; // Must be zero
	GLenum imageFormat = imgFormatToGL(attachmentPoint, attachmentData.numColorChannels);
	GLenum imageType = imgTypeToGL(attachmentPoint); // The type of each image channel
	const void* pixels = nullptr;

	glTexImage2D(GL_TEXTURE_2D, mipmapLevel, textureFormat, attachmentData.width, attachmentData.height,
		border, imageFormat, imageType, pixels);

	// Generate mipmaps if mipmap level request is greater than the default one (zero)
	// TODO: This is suboptimal because we are generating a whole set of mipmaps, but framebuffer is gonna use only one of them
	if (attachmentData.mipmapLevel > 0)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

inline void JFF::FramebufferGLSTBI::createCubemap(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData)
{
	// Generate cubemap object and bind it to work with it
	glGenTextures(1, &attachmentData.buffer);
	glBindTexture(GL_TEXTURE_CUBE_MAP, attachmentData.buffer);

	// Apply texture parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapModeToGL(attachmentData.wrapMode.u));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapModeToGL(attachmentData.wrapMode.v));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapModeToGL(attachmentData.wrapMode.w));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilterModeToGL(attachmentData.filterMode.minFilter));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilterModeToGL(attachmentData.filterMode.magFilter));

	if (attachmentData.wrapMode.u == Wrap::CLAMP_TO_BORDER ||
		attachmentData.wrapMode.v == Wrap::CLAMP_TO_BORDER ||
		attachmentData.wrapMode.w == Wrap::CLAMP_TO_BORDER)
	{
		glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, *attachmentData.borderColor);
	}

	// Fill the cubemap object with image (empty) data
	GLint mipmapLevel = 0; // TODO: Mipmap level (each mipmap can be loaded one by one or automatically, using glGenerateMipmap())
	GLint textureFormat = texFormatToGL(attachmentPoint, attachmentData.numColorChannels, attachmentData.HDR);
	GLint border = 0; // Must be zero
	GLenum imageFormat = imgFormatToGL(attachmentPoint, attachmentData.numColorChannels);
	GLenum imageType = imgTypeToGL(attachmentPoint); // The type of each image channel
	const void* pixels = nullptr;

	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipmapLevel, textureFormat,
			attachmentData.width, attachmentData.height, border, imageFormat, imageType, pixels);

	// Generate mipmaps if mipmap level request is greater than the default one (zero)
	// TODO: This is suboptimal because we are generating a whole set of mipmaps, but framebuffer is gonna use only one of them
	if (attachmentData.mipmapLevel > 0)
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


inline GLenum JFF::FramebufferGLSTBI::attachmentPointToGL(AttachmentPoint attachmentPoint) const
{
	switch (attachmentPoint)
	{
	case JFF::Framebuffer::AttachmentPoint::COLOR_0:
		return GL_COLOR_ATTACHMENT0;
	case JFF::Framebuffer::AttachmentPoint::COLOR_1:
		return GL_COLOR_ATTACHMENT1;
	case JFF::Framebuffer::AttachmentPoint::COLOR_2:
		return GL_COLOR_ATTACHMENT2;
	case JFF::Framebuffer::AttachmentPoint::COLOR_3:
		return GL_COLOR_ATTACHMENT3;
	case JFF::Framebuffer::AttachmentPoint::COLOR_4:
		return GL_COLOR_ATTACHMENT4;
	case JFF::Framebuffer::AttachmentPoint::COLOR_5:
		return GL_COLOR_ATTACHMENT5;
	case JFF::Framebuffer::AttachmentPoint::COLOR_6:
		return GL_COLOR_ATTACHMENT6;
	case JFF::Framebuffer::AttachmentPoint::COLOR_7:
		return GL_COLOR_ATTACHMENT7;
	case JFF::Framebuffer::AttachmentPoint::COLOR_8:
		return GL_COLOR_ATTACHMENT8;
	case JFF::Framebuffer::AttachmentPoint::COLOR_9:
		return GL_COLOR_ATTACHMENT9;
	case JFF::Framebuffer::AttachmentPoint::COLOR_10:
		return GL_COLOR_ATTACHMENT10;
	case JFF::Framebuffer::AttachmentPoint::COLOR_11:
		return GL_COLOR_ATTACHMENT11;
	case JFF::Framebuffer::AttachmentPoint::COLOR_12:
		return GL_COLOR_ATTACHMENT12;
	case JFF::Framebuffer::AttachmentPoint::COLOR_13:
		return GL_COLOR_ATTACHMENT13;
	case JFF::Framebuffer::AttachmentPoint::COLOR_14:
		return GL_COLOR_ATTACHMENT14;
	case JFF::Framebuffer::AttachmentPoint::COLOR_15:
		return GL_COLOR_ATTACHMENT15;
	case JFF::Framebuffer::AttachmentPoint::DEPTH:
		return GL_DEPTH_ATTACHMENT;
	case JFF::Framebuffer::AttachmentPoint::STENCIL:
		return GL_STENCIL_ATTACHMENT;
	case JFF::Framebuffer::AttachmentPoint::DEPTH_STENCIL:
	default:
		return GL_DEPTH_STENCIL_ATTACHMENT;
	}
}

inline GLenum JFF::FramebufferGLSTBI::texTypeToGL(TextureType texType) const
{
	switch (texType)
	{
	case JFF::Framebuffer::TextureType::CUBEMAP:
		return GL_TEXTURE_CUBE_MAP;
	case JFF::Framebuffer::TextureType::TEXTURE_2D:
	default:
		return GL_TEXTURE_2D;
	}
}

inline GLint JFF::FramebufferGLSTBI::wrapModeToGL(Wrap wrapMode) const
{
	switch (wrapMode)
	{
	case JFF::Framebuffer::Wrap::MIRRORED_REPEAT:
		return GL_MIRRORED_REPEAT;
	case JFF::Framebuffer::Wrap::CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
	case JFF::Framebuffer::Wrap::CLAMP_TO_BORDER:
		return GL_CLAMP_TO_BORDER;
	case JFF::Framebuffer::Wrap::REPEAT:
	default:
		return GL_REPEAT;
	}
}

inline GLint JFF::FramebufferGLSTBI::magFilterModeToGL(MagnificationFilter filter) const
{
	switch (filter)
	{
	case JFF::Framebuffer::MagnificationFilter::LINEAR:
		return GL_LINEAR;
	case JFF::Framebuffer::MagnificationFilter::NEAREST:
	default:
		return GL_NEAREST;
	}
}

inline GLint JFF::FramebufferGLSTBI::minFilterModeToGL(MinificationFilter filter) const
{
	switch (filter)
	{
	case JFF::Framebuffer::MinificationFilter::NEAREST:
		return GL_NEAREST;
	case JFF::Framebuffer::MinificationFilter::NEAREST_NEAREST_MIP:
		return GL_NEAREST_MIPMAP_NEAREST;
	case JFF::Framebuffer::MinificationFilter::LINEAR_NEAREST_MIP:
		return GL_LINEAR_MIPMAP_NEAREST;
	case JFF::Framebuffer::MinificationFilter::NEAREST_LINEAR_MIP:
		return GL_NEAREST_MIPMAP_LINEAR;
	case JFF::Framebuffer::MinificationFilter::LINEAR_LINEAR_MIP:
		return GL_LINEAR_MIPMAP_LINEAR;
	case JFF::Framebuffer::MinificationFilter::LINEAR:
	default:
		return GL_LINEAR;
	}
}

inline GLint JFF::FramebufferGLSTBI::texFormatToGL(AttachmentPoint attachmentPoint, unsigned int numColorChannels, bool HDR) const
{
	switch (attachmentPoint)
	{
	case JFF::Framebuffer::AttachmentPoint::DEPTH:
		return GL_DEPTH_COMPONENT;
	case JFF::Framebuffer::AttachmentPoint::STENCIL:
		// Generate an error here. More info here: https://www.khronos.org/opengl/wiki/Image_Format#Stencil_only
		JFF_LOG_WARNING("Unsupported stencil channel only. Use DEPTH_STENCIL if you want to use stencil buffer")
		return 0;
	case JFF::Framebuffer::AttachmentPoint::DEPTH_STENCIL:
		return GL_DEPTH24_STENCIL8;
	case JFF::Framebuffer::AttachmentPoint::COLOR_0:
	case JFF::Framebuffer::AttachmentPoint::COLOR_1:
	case JFF::Framebuffer::AttachmentPoint::COLOR_2:
	case JFF::Framebuffer::AttachmentPoint::COLOR_3:
	case JFF::Framebuffer::AttachmentPoint::COLOR_4:
	case JFF::Framebuffer::AttachmentPoint::COLOR_5:
	case JFF::Framebuffer::AttachmentPoint::COLOR_6:
	case JFF::Framebuffer::AttachmentPoint::COLOR_7:
	case JFF::Framebuffer::AttachmentPoint::COLOR_8:
	case JFF::Framebuffer::AttachmentPoint::COLOR_9:
	case JFF::Framebuffer::AttachmentPoint::COLOR_10:
	case JFF::Framebuffer::AttachmentPoint::COLOR_11:
	case JFF::Framebuffer::AttachmentPoint::COLOR_12:
	case JFF::Framebuffer::AttachmentPoint::COLOR_13:
	case JFF::Framebuffer::AttachmentPoint::COLOR_14:
	case JFF::Framebuffer::AttachmentPoint::COLOR_15:
	default:
		// 16 bits per channel. This is used for HDR because this format doesn't clamp colors in range [0,1]
		// NOTE: 4 byte color depth (GL_RGBA) is preferred on Windows platform for aligment purposes // TODO: Check
		if (numColorChannels == 1)
			return HDR ? GL_R16F : GL_RED;
		else if (numColorChannels == 2)
			return HDR ? GL_RG16F : GL_RG;
		else if (numColorChannels == 3)
			return HDR ? GL_RGB16F : GL_RGB;
		else
			return HDR ? GL_RGBA16F : GL_RGBA;
	}
}

inline GLenum JFF::FramebufferGLSTBI::imgFormatToGL(AttachmentPoint attachmentPoint, unsigned int numColorChannels) const
{
	switch (attachmentPoint)
	{
	case JFF::Framebuffer::AttachmentPoint::DEPTH:
		return GL_DEPTH_COMPONENT;
	case JFF::Framebuffer::AttachmentPoint::STENCIL:
		// Generate an error here. More info here: https://www.khronos.org/opengl/wiki/Image_Format#Stencil_only
		JFF_LOG_WARNING("Unsupported stencil channel only. Use DEPTH_STENCIL if you want to use stencil buffer")
			return 0u;
	case JFF::Framebuffer::AttachmentPoint::DEPTH_STENCIL:
		return GL_DEPTH_STENCIL;
	case JFF::Framebuffer::AttachmentPoint::COLOR_0:
	case JFF::Framebuffer::AttachmentPoint::COLOR_1:
	case JFF::Framebuffer::AttachmentPoint::COLOR_2:
	case JFF::Framebuffer::AttachmentPoint::COLOR_3:
	case JFF::Framebuffer::AttachmentPoint::COLOR_4:
	case JFF::Framebuffer::AttachmentPoint::COLOR_5:
	case JFF::Framebuffer::AttachmentPoint::COLOR_6:
	case JFF::Framebuffer::AttachmentPoint::COLOR_7:
	case JFF::Framebuffer::AttachmentPoint::COLOR_8:
	case JFF::Framebuffer::AttachmentPoint::COLOR_9:
	case JFF::Framebuffer::AttachmentPoint::COLOR_10:
	case JFF::Framebuffer::AttachmentPoint::COLOR_11:
	case JFF::Framebuffer::AttachmentPoint::COLOR_12:
	case JFF::Framebuffer::AttachmentPoint::COLOR_13:
	case JFF::Framebuffer::AttachmentPoint::COLOR_14:
	case JFF::Framebuffer::AttachmentPoint::COLOR_15:
	default:
		// 16 bits per channel. This is used for HDR because this format doesn't clamp colors in range [0,1]
		// NOTE: 4 byte color depth (GL_RGBA) is preferred on Windows platform for aligment purposes // TODO: Check
		if (numColorChannels == 1)
			return GL_RED;
		else if (numColorChannels == 2)
			return GL_RG;
		else if (numColorChannels == 3)
			return GL_RGB;
		else
			return GL_RGBA;
	}
}

inline GLenum JFF::FramebufferGLSTBI::imgTypeToGL(AttachmentPoint attachmentPoint) const
{
	switch (attachmentPoint)
	{
	case JFF::Framebuffer::AttachmentPoint::DEPTH:
	case JFF::Framebuffer::AttachmentPoint::DEPTH_STENCIL:
		return GL_FLOAT;
	case JFF::Framebuffer::AttachmentPoint::STENCIL:
		// Generate an error here. More info here: https://www.khronos.org/opengl/wiki/Image_Format#Stencil_only
		JFF_LOG_WARNING("Unsupported stencil channel only. Use DEPTH_STENCIL if you want to use stencil buffer")
			return 0u;
	case JFF::Framebuffer::AttachmentPoint::COLOR_0:
	case JFF::Framebuffer::AttachmentPoint::COLOR_1:
	case JFF::Framebuffer::AttachmentPoint::COLOR_2:
	case JFF::Framebuffer::AttachmentPoint::COLOR_3:
	case JFF::Framebuffer::AttachmentPoint::COLOR_4:
	case JFF::Framebuffer::AttachmentPoint::COLOR_5:
	case JFF::Framebuffer::AttachmentPoint::COLOR_6:
	case JFF::Framebuffer::AttachmentPoint::COLOR_7:
	case JFF::Framebuffer::AttachmentPoint::COLOR_8:
	case JFF::Framebuffer::AttachmentPoint::COLOR_9:
	case JFF::Framebuffer::AttachmentPoint::COLOR_10:
	case JFF::Framebuffer::AttachmentPoint::COLOR_11:
	case JFF::Framebuffer::AttachmentPoint::COLOR_12:
	case JFF::Framebuffer::AttachmentPoint::COLOR_13:
	case JFF::Framebuffer::AttachmentPoint::COLOR_14:
	case JFF::Framebuffer::AttachmentPoint::COLOR_15:
	default:
		return GL_UNSIGNED_BYTE;
	}
}

inline void JFF::FramebufferGLSTBI::configureReadAndWriteColorBuffers()
{
	if (mainFBOColorBuffersUsed.size() <= 0)
	{
		// Tell OpenGL that this fbo won't use color attachments. Next lines doesn't affect writing on depth buffer, they only affect color buffers
		// NOTE: Next functions are needed for fbo to be considered "complete" by OpenGL, because one condition is to have at least one color buffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
	{
		glDrawBuffers((GLsizei)mainFBOColorBuffersUsed.size(), mainFBOColorBuffersUsed.data());
	}
}


inline void JFF::FramebufferGLSTBI::checkFramebufferStatus() const
{
	// NOTE: The error messages are taken from Kronos documentation: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml
	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_COMPLETE:
		// All OK
		break;
	case GL_FRAMEBUFFER_UNDEFINED:
		JFF_LOG_ERROR("Specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist")
			break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		JFF_LOG_ERROR("Any of the framebuffer attachment points are framebuffer incomplete")
			break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		JFF_LOG_ERROR("The framebuffer does not have at least one image attached to it")
			break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		JFF_LOG_ERROR("The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi")
			break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		JFF_LOG_ERROR("GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER")
			break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		JFF_LOG_ERROR("The combination of internal formats of the attached images violates an implementation-dependent set of restrictions")
			break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		JFF_LOG_ERROR("The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; or the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES; or the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures")
			break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		JFF_LOG_ERROR("Any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target")
			break;
	default:
		JFF_LOG_ERROR("Unknown framebuffer error")
			break;
	}
}
