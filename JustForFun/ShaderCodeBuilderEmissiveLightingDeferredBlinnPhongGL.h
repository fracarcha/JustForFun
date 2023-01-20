/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco Jos� Carmona.
*
* All Rights Reserved.
*/

#pragma once

#include "ShaderCodeBuilder.h"

namespace JFF
{
	class ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL();
		virtual ~ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL(const ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL& other) = delete;
		ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL& operator=(const ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL(ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL&& other) = delete;
		ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL operator=(ShaderCodeBuilderEmissiveLightingDeferredBlinnPhongGL&& other) = delete;

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