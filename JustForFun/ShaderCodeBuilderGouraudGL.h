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
	class ShaderCodeBuilderGouraudGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderGouraudGL();
		virtual ~ShaderCodeBuilderGouraudGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderGouraudGL(const ShaderCodeBuilderGouraudGL& other) = delete;
		ShaderCodeBuilderGouraudGL& operator=(const ShaderCodeBuilderGouraudGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderGouraudGL(ShaderCodeBuilderGouraudGL&& other) = delete;
		ShaderCodeBuilderGouraudGL operator=(ShaderCodeBuilderGouraudGL&& other) = delete;

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