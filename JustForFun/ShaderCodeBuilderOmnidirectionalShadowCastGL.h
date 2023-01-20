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
	class ShaderCodeBuilderOmnidirectionalShadowCastGL : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderOmnidirectionalShadowCastGL();
		virtual ~ShaderCodeBuilderOmnidirectionalShadowCastGL();

		// Copy ctor and copy assignment
		ShaderCodeBuilderOmnidirectionalShadowCastGL(const ShaderCodeBuilderOmnidirectionalShadowCastGL& other) = delete;
		ShaderCodeBuilderOmnidirectionalShadowCastGL& operator=(const ShaderCodeBuilderOmnidirectionalShadowCastGL& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderOmnidirectionalShadowCastGL(ShaderCodeBuilderOmnidirectionalShadowCastGL&& other) = delete;
		ShaderCodeBuilderOmnidirectionalShadowCastGL operator=(ShaderCodeBuilderOmnidirectionalShadowCastGL&& other) = delete;

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