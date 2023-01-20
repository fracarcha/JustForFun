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
#include "MeshComponent.h"

#include <memory>

namespace JFF
{
	class PostProcessFX
	{
	public:
		// Ctor & Dtor
		PostProcessFX() {}
		virtual ~PostProcessFX() {}

		// Copy ctor and copy assignment
		PostProcessFX(const PostProcessFX& other) = delete;
		PostProcessFX& operator=(const PostProcessFX& other) = delete;

		// Move ctor and assignment
		PostProcessFX(PostProcessFX&& other) = delete;
		PostProcessFX operator=(PostProcessFX&& other) = delete;

		// ---------------------- POST PROCESS FX INTERFACE ---------------------- //

		// Execute the post processing effect using as input the value of another framebuffer
		virtual void execute(
			const std::weak_ptr<Framebuffer>& ppFBO, 
			const std::weak_ptr<Framebuffer>& ppFBO2,
			const std::weak_ptr<MeshComponent>& planeMesh) = 0;

		// Changes the sizes of all internal framebuffer this effect has it this makes sense for the concrete effect
		virtual void updateFramebufferSize(int width, int height) = 0;
	};
}