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

#include "Saveable.h"
#include "Vec.h"

#include <vector>
#include <map>
#include <memory>

namespace JFF
{
	class Framebuffer : public Saveable
	{
	public:
		enum class PrefabFramebuffer // Prebuilt framebuffers with a single enum
		{
			// Forward rendering FBO
			FBO_PRE_PROCESS_FORWARD,

			// Deferred rendering FBOs
			FBO_GEOMETRY_DEFERRED,
			FBO_LIGHTING_DEFERRED,

			// Post-process FBO
			FBO_POST_PROCESS,

			// Shadow FBOs
			FBO_SHADOW_MAP,
			FBO_SHADOW_CUBEMAP,
		};

		enum class AttachmentPoint : char
		{
			COLOR_0 = 0,
			COLOR_1,
			COLOR_2,
			COLOR_3,
			COLOR_4,
			COLOR_5,
			COLOR_6,
			COLOR_7,
			COLOR_8,
			COLOR_9,
			COLOR_10,
			COLOR_11,
			COLOR_12,
			COLOR_13,
			COLOR_14,
			COLOR_15,

			DEPTH,
			STENCIL,
			DEPTH_STENCIL,
		};

		enum class TextureType
		{
			TEXTURE_2D,
			CUBEMAP,
		};

		enum class Wrap : char
		{
			REPEAT,
			MIRRORED_REPEAT,
			CLAMP_TO_EDGE, // UVs outside [0,1] range will use the nearest texel
			CLAMP_TO_BORDER, // USs outside [0,1] range will be black (or whatever border color is defined)
		};

		enum class MagnificationFilter : char
		{
			NEAREST,
			LINEAR,
		};

		enum class MinificationFilter : char
		{
			NEAREST,
			LINEAR,
			NEAREST_NEAREST_MIP,
			LINEAR_NEAREST_MIP,
			NEAREST_LINEAR_MIP,
			LINEAR_LINEAR_MIP,
		};

		struct CoordsWrapMode
		{
			Wrap u, v, w;
		};

		struct FilterMode
		{
			MinificationFilter minFilter;
			MagnificationFilter magFilter;
		};

		struct AttachmentData
		{
			// Width and height of the buffer in pixels. If buffer is a cubemap, this is the size of each face
			unsigned int width, height;
			
			// Render buffers are faster when rendering, but they cannot be sampled in shaders. Useful with depth and stencil channels
			bool renderBuffer;

			// Texture buffer options (renderBuffer == false)
			TextureType texType;			// In multisample framebuffers, this is always texture 2D
			CoordsWrapMode wrapMode;		// In multisample framebuffers, this is ignored
			Vec4 borderColor;				// If wrapMode == CLAMP_TO_BORDER, this is the color of the border used
			FilterMode filterMode;			// In multisample framebuffers, this is ignored
			bool HDR;						// Only valid for color attachments
			unsigned int numColorChannels;	// Only valid for color attachments
			int mipmapLevel;				// Default is zero. This uses a lower res version of this texture (e.g. level=1 -> width/2 height/2 with bilinear filter)
		};

		struct Params
		{
			unsigned int samplesPerPixel; // If > 1, it would enable multisample framebuffer
			std::map<AttachmentPoint, AttachmentData> attachments;
		};

		// Ctor & Dtor
		Framebuffer() {}
		virtual ~Framebuffer() {}

		// Copy ctor and copy assignment
		Framebuffer(const Framebuffer& other) = delete;
		Framebuffer& operator=(const Framebuffer& other) = delete;

		// Move ctor and assignment
		Framebuffer(Framebuffer&& other) = delete;
		Framebuffer operator=(Framebuffer&& other) = delete;

		// ----------------- FRAMEBUFFER INTERFACE ----------------- //

		// Uses this framebuffer as target for reading and rendering.
		// Optionally, this framebuffer can clear all its attached buffers
		virtual void enable(bool clearBuffers = true) = 0;

		// Stop using this framebuffer as target for reading and rendering. After that, the default framebuffer will be used.
		// In multisample framebuffers, multisample textures will be resolved and dumped into an internal non-multisample framebuffer for use
		virtual void disable() = 0;

		// Bind a texture specified by attachment point in specified texture unit
		virtual void useTexture(AttachmentPoint attachmentPoint, int textureUnit) = 0;

		// Change framebuffer size. This action will destroy previous internal buffer and create a new one
		virtual void setSize(unsigned int width, unsigned int height) = 0;

		// Get the size of buffer on attachment point. Attached mipmap level alter the resulting size
		virtual void getSize(AttachmentPoint attachmentPoint, unsigned int& width, unsigned int& height) = 0;

		/*
		* Copy the pixels from src buffer's attachment point to this buffer.
		* In case of copying depth, stencil or depth-stencil buffers, both attachment points should be DEPTH, STENCIL, or DEPTH_STENCIL.
		* WARNING: This function could change internal currently bound framebuffers
		*/
		virtual void copyBuffer(AttachmentPoint dstAttachmentPoint, AttachmentPoint srcAttachmentPoint, std::weak_ptr<Framebuffer> src) = 0;

		// Free GPU memory of this framebuffer making it useless
		virtual void destroy() = 0;
	};
}