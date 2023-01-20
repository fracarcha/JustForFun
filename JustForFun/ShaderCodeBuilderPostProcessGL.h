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
	class ShaderCodeBuilderPostProcessGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderPostProcessGL();
		virtual ~ShaderCodeBuilderPostProcessGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderPostProcessGL(const ShaderCodeBuilderPostProcessGL& other) = delete;
		ShaderCodeBuilderPostProcessGL& operator=(const ShaderCodeBuilderPostProcessGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderPostProcessGL(ShaderCodeBuilderPostProcessGL&& other) = delete;
		ShaderCodeBuilderPostProcessGL operator=(ShaderCodeBuilderPostProcessGL&& other) = delete;

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