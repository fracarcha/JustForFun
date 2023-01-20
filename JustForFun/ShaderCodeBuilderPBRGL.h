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
	class ShaderCodeBuilderPBRGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderPBRGL();
		virtual ~ShaderCodeBuilderPBRGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderPBRGL(const ShaderCodeBuilderPBRGL& other) = delete;
		ShaderCodeBuilderPBRGL& operator=(const ShaderCodeBuilderPBRGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderPBRGL(ShaderCodeBuilderPBRGL&& other) = delete;
		ShaderCodeBuilderPBRGL operator=(ShaderCodeBuilderPBRGL&& other) = delete;

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