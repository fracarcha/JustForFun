#pragma once

#include "ShaderCodeBuilder.h"

namespace JFF
{
	class ShaderCodeBuilderShadowCastGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderShadowCastGL();
		virtual ~ShaderCodeBuilderShadowCastGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderShadowCastGL(const ShaderCodeBuilderShadowCastGL& other) = delete;
		ShaderCodeBuilderShadowCastGL& operator=(const ShaderCodeBuilderShadowCastGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderShadowCastGL(ShaderCodeBuilderShadowCastGL&& other) = delete;
		ShaderCodeBuilderShadowCastGL operator=(ShaderCodeBuilderShadowCastGL&& other) = delete;

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