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

#include "Framebuffer.h"

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

namespace JFF
{
	class FramebufferGLSTBI : public Framebuffer
	{
	protected:
		struct AttachmentDataInternal : public AttachmentData
		{
			GLuint buffer;
		};

		struct ParamsInternal
		{
			GLuint fbo;
			std::map<AttachmentPoint, AttachmentDataInternal> fboAttachments;
		};

	public:
		// Ctor & Dtor
		FramebufferGLSTBI(PrefabFramebuffer fboType, unsigned int width, unsigned int height, unsigned int samplesPerPixel = 0);
		FramebufferGLSTBI(const Params& params);
		virtual ~FramebufferGLSTBI();

		// Copy ctor and copy assignment
		FramebufferGLSTBI(const FramebufferGLSTBI& other) = delete;
		FramebufferGLSTBI& operator=(const FramebufferGLSTBI& other) = delete;

		// Move ctor and assignment
		FramebufferGLSTBI(FramebufferGLSTBI&& other) = delete;
		FramebufferGLSTBI operator=(FramebufferGLSTBI&& other) = delete;

		// -------------------------------- SAVEABLE INTERFACE -------------------------------- //

		virtual void writeToFile(const char* newFilename, bool storeInGeneratedSubfolder = true) override;

		// ----------------- FRAMEBUFFER INTERFACE ----------------- //

		// Uses this framebuffer as target for reading and rendering.
		// Optionally, this framebuffer can clear all its attached buffers
		virtual void enable(bool clearBuffers = true) override;

		// Stop using this framebuffer as target for reading and rendering. After that, the default framebuffer will be used.
		// In multisample framebuffers, multisample textures will be resolved and dumped into an internal non-multisample framebuffer for use
		virtual void disable() override;

		// Bind a texture specified by attachment point in specified texture unit. 
		virtual void useTexture(AttachmentPoint attachmentPoint, int textureUnit) override;

		// Change framebuffer size. This action will destroy previous internal buffer and create a new one
		virtual void setSize(unsigned int width, unsigned int height) override;

		// Get the size of buffer on attachment point. Attached mipmap level alter the resulting size
		virtual void getSize(AttachmentPoint attachmentPoint, unsigned int& width, unsigned int& height) override;

		/*
		* Copy the pixels from src buffer's attachment point to this buffer.
		* In case of copying depth, stencil or depth-stencil buffers, both attachment points should be DEPTH, STENCIL, or DEPTH_STENCIL.
		* WARNING: This function could change internal currently bound framebuffers
		*/
		virtual void copyBuffer(AttachmentPoint dstAttachmentPoint, AttachmentPoint srcAttachmentPoint, std::weak_ptr<Framebuffer> src) override;

		// Free GPU memory of this framebuffer making it useless
		virtual void destroy() override;

	private:
		inline void extractParamsData(PrefabFramebuffer fboType, unsigned int width, unsigned int height, unsigned int samplesPerPixel);
		inline void extractParamsData(const Params& params);
		inline void create();

		inline void createMainBuffer(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData);
		inline void createMainFrameBuffer();

		inline void createAuxBuffer(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData);
		inline void createAuxFrameBuffer();

		inline void createRenderbufferMultisample(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData);
		inline void createRenderBuffer(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData);
		inline void createTexture2DMultisample(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData);
		inline void createTexture2D(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData);
		inline void createCubemap(AttachmentPoint attachmentPoint, AttachmentDataInternal& attachmentData);

		inline GLenum attachmentPointToGL(AttachmentPoint attachmentPoint) const;
		inline GLenum texTypeToGL(TextureType texType) const;
		inline GLint wrapModeToGL(Wrap wrapMode) const;
		inline GLint magFilterModeToGL(MagnificationFilter filter) const;
		inline GLint minFilterModeToGL(MinificationFilter filter) const;
		inline GLint texFormatToGL(AttachmentPoint attachmentPoint, unsigned int numColorChannels, bool HDR) const;
		inline GLenum imgFormatToGL(AttachmentPoint attachmentPoint, unsigned int numColorChannels) const;
		inline GLenum imgTypeToGL(AttachmentPoint attachmentPoint) const;

		inline void configureReadAndWriteColorBuffers();

		inline void checkFramebufferStatus() const;

	protected:
		bool isDestroyed;

		unsigned int samplesPerPixel;
		ParamsInternal mainFBO;
		ParamsInternal auxFBO; // Auxiliary FBO is used to resolve multisample textures on main FBO

		// Store color buffers used on this framebuffer
		std::vector<GLenum> mainFBOColorBuffersUsed;

		// Clear mask
		GLbitfield clearMask;

	};
}