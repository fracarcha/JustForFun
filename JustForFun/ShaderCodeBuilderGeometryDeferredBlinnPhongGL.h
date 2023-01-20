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
	class ShaderCodeBuilderGeometryDeferredBlinnPhong : public ShaderCodeBuilder
	{
	public:
		// Ctor & Dtor
		ShaderCodeBuilderGeometryDeferredBlinnPhong();
		virtual ~ShaderCodeBuilderGeometryDeferredBlinnPhong();

		// Copy ctor and copy assignment
		ShaderCodeBuilderGeometryDeferredBlinnPhong(const ShaderCodeBuilderGeometryDeferredBlinnPhong& other) = delete;
		ShaderCodeBuilderGeometryDeferredBlinnPhong& operator=(const ShaderCodeBuilderGeometryDeferredBlinnPhong& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilderGeometryDeferredBlinnPhong(ShaderCodeBuilderGeometryDeferredBlinnPhong&& other) = delete;
		ShaderCodeBuilderGeometryDeferredBlinnPhong operator=(ShaderCodeBuilderGeometryDeferredBlinnPhong&& other) = delete;

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