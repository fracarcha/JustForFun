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

#include "ShaderCodeBuilder.h"

namespace JFF
{
	class ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL();
		virtual ~ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL(const ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL& other) = delete;
		ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL& operator=(const ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL(ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL&& other) = delete;
		ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL operator=(ShaderCodeBuilderEnvironmentLightingDeferredBlinnPhongGL&& other) = delete;

		// ------------------------ SHADER CODE BUILDER INTERFACE ------------------------ //

		// Generate a compilable shader code from params
		virtual void generateCode(const Params& params,
			std::string& outVertexShaderCode,
			std::string& outGeometryShaderCode,
			std::string& outFragmentShaderCode) const override;

	private:
		inline std::string getShaderVersionLine(const Params& params) const;
		inline std::string getVertexShaderCode(const Params& params) const;
		inline std::string getFragmentShaderCode(const Params& params) const;
	};
}