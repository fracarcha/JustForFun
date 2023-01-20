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
	class ShaderCodeBuilderDebugGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderDebugGL();
		virtual ~ShaderCodeBuilderDebugGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderDebugGL(const ShaderCodeBuilderDebugGL& other) = delete;
		ShaderCodeBuilderDebugGL& operator=(const ShaderCodeBuilderDebugGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderDebugGL(ShaderCodeBuilderDebugGL&& other) = delete;
		ShaderCodeBuilderDebugGL operator=(ShaderCodeBuilderDebugGL&& other) = delete;

		// ------------------------ SHADER CODE BUILDER INTERFACE ------------------------ //

		// Generate a compilable shader code from params
		virtual void generateCode(const Params& params,
			std::string& outVertexShaderCode,
			std::string& outGeometryShaderCode,
			std::string& outFragmentShaderCode) const override;

	private:
		inline std::string getShaderVersionLine(const Params& params) const;
		inline std::string getVertexShaderCode(const Params& params) const;
		inline std::string getGeometryShaderCode(const Params& params) const;
		inline std::string getFragmentShaderCode(const Params& params) const;
	};
}