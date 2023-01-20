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

#include "PostProcessFX.h"
#include "Material.h"

namespace JFF
{
	class Engine;

	class PostProcessFXBloom : public PostProcessFX
	{
	public:
		// Ctor & Dtor
		PostProcessFXBloom(Engine* const engine, int bufferWidth, int bufferHeight, float threshold, float intensity);
		virtual ~PostProcessFXBloom();

		// Copy ctor and copy assignment
		PostProcessFXBloom(const PostProcessFXBloom& other) = delete;
		PostProcessFXBloom& operator=(const PostProcessFXBloom& other) = delete;

		// Move ctor and assignment
		PostProcessFXBloom(PostProcessFXBloom&& other) = delete;
		PostProcessFXBloom operator=(PostProcessFXBloom&& other) = delete;

		// ---------------------- POST PROCESS FX OVERRIDES ---------------------- //

		// Execute the post processing effect using as input the value of another framebuffer
		virtual void execute(
			const std::weak_ptr<Framebuffer>& ppFBO, 
			const std::weak_ptr<Framebuffer>& ppFBO2,
			const std::weak_ptr<MeshComponent>& planeMesh) override;

		// Changes the sizes of all internal framebuffer this effect has it this makes sense for the concrete effect
		virtual void updateFramebufferSize(int width, int height) override;

	protected:
		Engine* engine;

		float threshold;
		float intensity;

		std::shared_ptr<Material> colorCopyMaterial;
		std::shared_ptr<Material> highPassFilterMaterial;
		std::shared_ptr<Material> gaussianBlurHorizontalMaterial;
		std::shared_ptr<Material> gaussianBlurVerticalMaterial;
		std::shared_ptr<Material> colorAdditionMaterial;

		// Next two framebuffers have textures of mip level = 1
		std::shared_ptr<Framebuffer> highPassFilterFBO;
		std::shared_ptr<Framebuffer> bloomResultFBO;

		// Next framebuffers have descending size mip levels, starting in 2
		unsigned int bloomNumPasses; // Min value = 4
		std::vector<std::shared_ptr<Framebuffer>> gaussianBlurHorizontalFBOs;
		std::vector<std::shared_ptr<Framebuffer>> gaussianBlurVerticalFBOs;
	};
}