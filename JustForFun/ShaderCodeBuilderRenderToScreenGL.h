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
	class ShaderCodeBuilderRenderToScreenGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderRenderToScreenGL();
		virtual ~ShaderCodeBuilderRenderToScreenGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderRenderToScreenGL(const ShaderCodeBuilderRenderToScreenGL& other) = delete;
		ShaderCodeBuilderRenderToScreenGL& operator=(const ShaderCodeBuilderRenderToScreenGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderRenderToScreenGL(ShaderCodeBuilderRenderToScreenGL&& other) = delete;
		ShaderCodeBuilderRenderToScreenGL operator=(ShaderCodeBuilderRenderToScreenGL&& other) = delete;

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