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
	class ShaderCodeBuilderBRDFIntegrationMapGeneratorGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderBRDFIntegrationMapGeneratorGL();
		virtual ~ShaderCodeBuilderBRDFIntegrationMapGeneratorGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderBRDFIntegrationMapGeneratorGL(const ShaderCodeBuilderBRDFIntegrationMapGeneratorGL& other) = delete;
		ShaderCodeBuilderBRDFIntegrationMapGeneratorGL& operator=(const ShaderCodeBuilderBRDFIntegrationMapGeneratorGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderBRDFIntegrationMapGeneratorGL(ShaderCodeBuilderBRDFIntegrationMapGeneratorGL&& other) = delete;
		ShaderCodeBuilderBRDFIntegrationMapGeneratorGL operator=(ShaderCodeBuilderBRDFIntegrationMapGeneratorGL&& other) = delete;

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