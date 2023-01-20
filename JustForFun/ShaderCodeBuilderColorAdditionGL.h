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
	class ShaderCodeBuilderColorAdditionGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderColorAdditionGL();
		virtual ~ShaderCodeBuilderColorAdditionGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderColorAdditionGL(const ShaderCodeBuilderColorAdditionGL& other) = delete;
		ShaderCodeBuilderColorAdditionGL& operator=(const ShaderCodeBuilderColorAdditionGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderColorAdditionGL(ShaderCodeBuilderColorAdditionGL&& other) = delete;
		ShaderCodeBuilderColorAdditionGL operator=(ShaderCodeBuilderColorAdditionGL&& other) = delete;

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