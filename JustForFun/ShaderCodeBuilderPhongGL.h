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
	class ShaderCodeBuilderPhongGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderPhongGL();
		virtual ~ShaderCodeBuilderPhongGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderPhongGL(const ShaderCodeBuilderPhongGL& other) = delete;
		ShaderCodeBuilderPhongGL& operator=(const ShaderCodeBuilderPhongGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderPhongGL(ShaderCodeBuilderPhongGL&& other) = delete;
		ShaderCodeBuilderPhongGL operator=(ShaderCodeBuilderPhongGL&& other) = delete;

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