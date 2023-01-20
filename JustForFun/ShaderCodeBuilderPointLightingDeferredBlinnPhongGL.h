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
	class ShaderCodeBuilderPointLightingDeferredBlinnPhongGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderPointLightingDeferredBlinnPhongGL();
		virtual ~ShaderCodeBuilderPointLightingDeferredBlinnPhongGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderPointLightingDeferredBlinnPhongGL(const ShaderCodeBuilderPointLightingDeferredBlinnPhongGL& other) = delete;
		ShaderCodeBuilderPointLightingDeferredBlinnPhongGL& operator=(const ShaderCodeBuilderPointLightingDeferredBlinnPhongGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderPointLightingDeferredBlinnPhongGL(ShaderCodeBuilderPointLightingDeferredBlinnPhongGL&& other) = delete;
		ShaderCodeBuilderPointLightingDeferredBlinnPhongGL operator=(ShaderCodeBuilderPointLightingDeferredBlinnPhongGL&& other) = delete;

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