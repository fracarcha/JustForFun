/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "ShaderCodeBuilderUIGL.h"

#include "Log.h"

#include <sstream>
#include <regex>

JFF::ShaderCodeBuilderUIGL::ShaderCodeBuilderUIGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor ShaderCodeBuilderUIGL")
}

JFF::ShaderCodeBuilderUIGL::~ShaderCodeBuilderUIGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor ShaderCodeBuilderUIGL")
}

void JFF::ShaderCodeBuilderUIGL::generateCode(const Params& params, 
	std::string& outVertexShaderCode,
	std::string& outGeometryShaderCode, 
	std::string& outFragmentShaderCode) const
{
	outVertexShaderCode = getVertexShaderCode(params);
	// TODO: geometry
	outFragmentShaderCode = getFragmentShaderCode(params);
}

inline std::string JFF::ShaderCodeBuilderUIGL::getShaderVersionLine(const Params& params) const
{
	return std::string(); // TODO:
}

inline std::string JFF::ShaderCodeBuilderUIGL::getVertexShaderCode(const Params& params) const
{
	return std::string(); // TODO:
}

inline std::string JFF::ShaderCodeBuilderUIGL::getFragmentShaderCode(const Params& params) const
{
	return std::string(); // TODO:
}
