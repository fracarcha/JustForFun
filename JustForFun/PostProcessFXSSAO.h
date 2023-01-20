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

	class PostProcessFXSSAO : public PostProcessFX
	{
	public:
		// Ctor & Dtor
		PostProcessFXSSAO(Engine* const engine, int bufferWidth, int bufferHeight, 
			unsigned int numSamples = 64, float sampleHemisphereRadius = 0.5f, 
			unsigned int numBlurSteps = 4, float intensity = 1.0f);
		virtual ~PostProcessFXSSAO();

		// Copy ctor and copy assignment
		PostProcessFXSSAO(const PostProcessFXSSAO& other) = delete;
		PostProcessFXSSAO& operator=(const PostProcessFXSSAO& other) = delete;

		// Move ctor and assignment
		PostProcessFXSSAO(PostProcessFXSSAO&& other) = delete;
		PostProcessFXSSAO operator=(PostProcessFXSSAO&& other) = delete;

		// ---------------------- POST PROCESS FX OVERRIDES ---------------------- //

		// Execute the post processing effect using as input the value of another framebuffer
		virtual void execute(
			const std::weak_ptr<Framebuffer>& ppFBO,
			const std::weak_ptr<Framebuffer>& ppFBO2,
			const std::weak_ptr<MeshComponent>& planeMesh) override;

		// Changes the sizes of all internal framebuffer this effect has it this makes sense for the concrete effect
		virtual void updateFramebufferSize(int width, int height) override;

	private:
		inline std::shared_ptr<Texture> generateRandomTangentsTexture() const;
		inline void generateHemisphereSamples();
		inline void sendHemisphereSamples();

	protected:
		Engine* engine;

		static const int MAX_NUM_SAMPLES = 64; // Ensure SSAO shader has the same value
		unsigned int numHemisphereSamples;
		float sampleHemisphereRadius;	
		unsigned int numBlurSteps; // Each pass is a horizontal followed by a vertical blur
		float intensity;

		// Materials
		std::shared_ptr<Material> SSAOMaterial;
		std::shared_ptr<Material> gaussianBlurHorizontalMaterial;
		std::shared_ptr<Material> gaussianBlurVerticalMaterial;
		std::shared_ptr<Material> colorCopyMaterial;

		// Framebuffers
		std::shared_ptr<Framebuffer> SSAO_FBO;
		std::shared_ptr<Framebuffer> gaussianBlurHorizontalFBO;
		std::shared_ptr<Framebuffer> gaussianBlurVerticalFBO;

		// Hemisphere samples used to check if a fragment is occluded
		std::vector<Vec3> hemisphereSamplesTangentSpace;
	};
}