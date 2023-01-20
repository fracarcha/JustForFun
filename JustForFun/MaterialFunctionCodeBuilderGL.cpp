/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "MaterialFunctionCodeBuilderGL.h"

#include "Log.h"
#include "ShaderCodeBuilder.h"

JFF::MaterialFunctionCodeBuilderGL::MaterialFunctionCodeBuilderGL() : 
	params()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor MaterialFunctionCodeBuilderGL")
}

JFF::MaterialFunctionCodeBuilderGL::~MaterialFunctionCodeBuilderGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor MaterialFunctionCodeBuilderGL")
}

void JFF::MaterialFunctionCodeBuilderGL::addTextureLine(
	const std::string& texName, 
	Aplication texApplication, 
	TextureMapping textureMapping,
	const std::string& uvVariableNameUsed,
	float blendFactor, 
	TextureOp texOp)
{
	switch (texApplication)
	{
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_DIFFUSE:
		setTextureLine(&params.lineDiffuse, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_SPECULAR:
		setTextureLine(&params.lineSpecular, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_AMBIENT:
		setTextureLine(&params.lineAmbient, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_EMISSIVE:
		setTextureLine(&params.lineEmissive, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_HEIGHT:
		setTextureHeightLine(texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_NORMAL:
		setTextureLine(&params.lineNormal, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_SHININESS:
		setTextureLine(&params.lineShininess, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_OPACITY:
		setTextureLine(&params.lineOpacity, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_DISPLACEMENT:
		setTextureDisplacementLine(texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_LIGHTMAP:
		setTextureLine(&params.lineLightmap, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_REFLECTION:
		setTextureLine(&params.lineReflection, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_BASE_COLOR:
		setTextureLine(&params.lineBaseColor, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_NORMAL_CAMERA:
		setTextureLine(&params.lineNormalCamera, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_EMISSION_COLOR:
		setTextureLine(&params.lineEmissionColor, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_METALNESS:
		setTextureLine(&params.lineMetalness, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_DIFFUSE_ROUGHNESS:
		setTextureLine(&params.lineDiffuseRoughness, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_AMBIENT_OCCLUSION:
		setTextureLine(&params.lineAmbientOcclusion, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_SHEEN:
		setTextureLine(&params.lineSheen, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_CLEARCOAT:
		setTextureLine(&params.lineClearCoat, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_TRANSMISSION:
		setTextureLine(&params.lineTransmission, texOp, blendFactor, texName, uvVariableNameUsed);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::NO_APPLICATION:
	default:
		JFF_LOG_WARNING("Cannot add texture with name " << texName << "because it doesn't have a valid texture application")
		return;
	}
}

void JFF::MaterialFunctionCodeBuilderGL::addConstantLine(const Vec4& value, Aplication texApplication)
{
	switch (texApplication)
	{
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_DIFFUSE:
		setConstantLine(&params.lineDiffuse, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_SPECULAR:
		setConstantLine(&params.lineSpecular, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_AMBIENT:
		setConstantLine(&params.lineAmbient, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_EMISSIVE:
		setConstantLine(&params.lineEmissive, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_HEIGHT:
		setConstantHeightLine(value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_NORMAL:
		setConstantLine(&params.lineNormal, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_SHININESS:
		setConstantLine(&params.lineShininess, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_OPACITY:
		setConstantLine(&params.lineOpacity, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_DISPLACEMENT:
		setConstantDisplacementLine(value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_LIGHTMAP:
		setConstantLine(&params.lineLightmap, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PHONG_REFLECTION:
		setConstantLine(&params.lineReflection, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_BASE_COLOR:
		setConstantLine(&params.lineBaseColor, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_NORMAL_CAMERA:
		setConstantLine(&params.lineNormalCamera, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_EMISSION_COLOR:
		setConstantLine(&params.lineEmissionColor, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_METALNESS:
		setConstantLine(&params.lineMetalness, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_DIFFUSE_ROUGHNESS:
		setConstantLine(&params.lineDiffuseRoughness, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_AMBIENT_OCCLUSION:
		setConstantLine(&params.lineAmbientOcclusion, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_SHEEN:
		setConstantLine(&params.lineSheen, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_CLEARCOAT:
		setConstantLine(&params.lineClearCoat, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::PBR_TRANSMISSION:
		setConstantLine(&params.lineTransmission, value);
		return;
	case JFF::MaterialFunctionCodeBuilder::Aplication::NO_APPLICATION:
	default:
		JFF_LOG_WARNING("Cannot add constant because it doesn't have a valid texture application")
		return;
	}
}

void JFF::MaterialFunctionCodeBuilderGL::addMaterialOverrideFunction(const std::string& fn)
{
	params.materialOverridesCode = fn;
}

void JFF::MaterialFunctionCodeBuilderGL::generateCode(std::string& outMaterialFunctionCode, bool useParallaxFunction, bool isPBR)
{
	std::ostringstream oss;
	oss << ShaderCodeBuilder::MATERIAL_FUNCTION_HEADER;

	// Parallax mapping lines
	std::string lineHeight				= params.lineHeight.tellp() != 0				? params.lineHeight.str() : "vec4(0)";
	std::string lineDisplacement		= params.lineDisplacement.tellp() != 0			? params.lineDisplacement.str() : "vec4(0)";
	std::string lineParallaxIntensity	= params.lineParallaxIntensity.tellp() != 0		? params.lineParallaxIntensity.str() : "0.0";
	std::string lineParallaxFunction													= params.lineParallaxFunctionCall.str();

	oss << "\t\t\t\t" << ShaderCodeBuilder::HEIGHT				<< " = " << lineHeight				<< ";\n";
	oss << "\t\t\t\t" << ShaderCodeBuilder::DISPLACEMENT		<< " = " << lineDisplacement		<< ";\n";
	oss << "\t\t\t\t" << ShaderCodeBuilder::PARALLAX_INTENSITY	<< " = " << lineParallaxIntensity	<< ";\n";

	if (useParallaxFunction)
		oss << "\t\t\t\t" << lineParallaxFunction << ";\n";

	if (isPBR)
	{
		// NOTE: This class does nothing when PBR workflow is Specular. The work has to be done in materialOverrides() function

		// Some models use Blinn-Phong channels to store PBR info. If PBR line is empty, use their Blinn-Phong counterpart

		std::string lineBaseColor;
		if (params.lineBaseColor.tellp() != 0)
			lineBaseColor = params.lineBaseColor.str();
		else if (params.lineDiffuse.tellp() != 0)
			lineBaseColor = params.lineDiffuse.str();
		else
			lineBaseColor = "vec4(0)";
		
		std::string lineMetalness			= params.lineMetalness.tellp() != 0			? params.lineMetalness.str()		: "vec4(0)";
		std::string lineDiffuseRoughness	= params.lineDiffuseRoughness.tellp() != 0	? params.lineDiffuseRoughness.str() : "vec4(0)";

		std::string lineNormalCamera;
		if (params.lineNormalCamera.tellp() != 0)
			lineNormalCamera = params.lineNormalCamera.str();
		else if (params.lineNormal.tellp() != 0)
			lineNormalCamera = params.lineNormal.str();
		else
			lineNormalCamera = "vec4(0)";

		std::string lineEmissionColor;
		if (params.lineEmissionColor.tellp() != 0)
			lineEmissionColor = params.lineEmissionColor.str();
		else if (params.lineEmissive.tellp() != 0)
			lineEmissionColor = params.lineEmissive.str();
		else
			lineEmissionColor = "vec4(0)";

		std::string lineAmbientOcclusion	= params.lineAmbientOcclusion.tellp() != 0	? params.lineAmbientOcclusion.str() : "vec4(1)"; // Default AO is 1 in PBR		
		std::string lineOpacity				= params.lineOpacity.tellp() != 0			? params.lineOpacity.str()			: "vec4(0)";

		std::string lineSheen				= params.lineSheen.tellp() != 0				? params.lineSheen.str()			: "vec4(0)";
		std::string lineClearCoat			= params.lineClearCoat.tellp() != 0			? params.lineClearCoat.str()		: "vec4(0)";
		std::string lineTransmission		= params.lineTransmission.tellp() != 0		? params.lineTransmission.str()		: "vec4(0)";

		// Add lines to material function stream
		oss << "\t\t\t\t" << ShaderCodeBuilder::BASE_COLOR			<< " = " << lineBaseColor			<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::METALNESS			<< " = " << lineMetalness			<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::DIFFUSE_ROUGHNESS	<< " = " << lineDiffuseRoughness	<< ";\n";

		oss << "\t\t\t\t" << ShaderCodeBuilder::NORMAL_CAMERA		<< " = " << lineNormalCamera		<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::EMISSION_COLOR		<< " = " << lineEmissionColor		<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::AMBIENT_OCCLUSION	<< " = " << lineAmbientOcclusion	<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::OPACITY				<< " = " << lineOpacity				<< ";\n";

		oss << "\t\t\t\t" << ShaderCodeBuilder::SHEEN				<< " = " << lineSheen				<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::CLEAR_COAT			<< " = " << lineClearCoat			<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::TRANSMISSION		<< " = " << lineTransmission		<< ";\n";
	}
	else
	{
		std::string lineDiffuse		= params.lineDiffuse.tellp() != 0		? params.lineDiffuse.str()		: "vec4(0)";
		std::string lineSpecular	= params.lineSpecular.tellp() != 0		? params.lineSpecular.str()		: "vec4(0)";
		std::string lineAmbient		= params.lineAmbient.tellp() != 0		? params.lineAmbient.str()		: "vec4(0)";
		std::string lineEmissive	= params.lineEmissive.tellp() != 0		? params.lineEmissive.str()		: "vec4(0)";
		std::string lineNormal		= params.lineNormal.tellp() != 0		? params.lineNormal.str()		: "vec4(0)";
		std::string lineShininess	= params.lineShininess.tellp() != 0		? params.lineShininess.str()	: "vec4(0)";
		std::string lineOpacity		= params.lineOpacity.tellp() != 0		? params.lineOpacity.str()		: "vec4(0)";
		std::string lineLightmap	= params.lineLightmap.tellp() != 0		? params.lineLightmap.str()		: "vec4(0)";
		std::string lineReflection	= params.lineReflection.tellp() != 0	? params.lineReflection.str()	: "vec4(0)";

		// Add lines to material function stream
		oss << "\t\t\t\t" << ShaderCodeBuilder::DIFFUSE		<< " = " << lineDiffuse		<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::SPECULAR	<< " = " << lineSpecular	<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::AMBIENT		<< " = " << lineAmbient		<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::EMISSIVE	<< " = " << lineEmissive	<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::NORMAL		<< " = " << lineNormal		<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::SHININESS	<< " = " << lineShininess	<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::OPACITY		<< " = " << lineOpacity		<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::LIGHTMAP	<< " = " << lineLightmap	<< ";\n";
		oss << "\t\t\t\t" << ShaderCodeBuilder::REFLECTION	<< " = " << lineReflection	<< ";\n";
	}

	oss << ShaderCodeBuilder::MATERIAL_FUNCTION_CLOSE_BRACKET;

	// Add materialOverrides() function code
	if (params.materialOverridesCode.empty())
	{
		oss << ShaderCodeBuilder::MATERIAL_OVERRIDES_EMPTY_FUNCTION;
	}
	else
	{
		oss << params.materialOverridesCode;
	}

	outMaterialFunctionCode = oss.str();
}

inline void JFF::MaterialFunctionCodeBuilderGL::setTextureLine(
	std::ostringstream* lineStream, 
	TextureOp texOp,
	float blendFactor, 
	const std::string& 
	texName, 
	const std::string& uvVariableNameUsed)
{
	// If this is the first line added, it doesn't have any operator on its left
	if (lineStream->tellp() == 0)
		texOp = TextureOp::NONE;

	// TODO: Extract texture mapping. For the moment it's not implemented
	switch (texOp)
	{
	case JFF::MaterialFunctionCodeBuilder::TextureOp::NONE: // T = T2
		(*lineStream) << blendFactor << " * texture(" << texName << "," << uvVariableNameUsed << ")";
		break;
	case JFF::MaterialFunctionCodeBuilder::TextureOp::MULTIPLY: // T = T1 * T2
		(*lineStream) << " * " << blendFactor << " * texture(" << texName << "," << uvVariableNameUsed << ")";
		break;
	case JFF::MaterialFunctionCodeBuilder::TextureOp::ADD: // T = T1 + T2
		(*lineStream) << " + " << blendFactor << " * texture(" << texName << "," << uvVariableNameUsed << ")";
		break;
	case JFF::MaterialFunctionCodeBuilder::TextureOp::SUBTRACT: // T = T1 - T2
		(*lineStream) << " - " << blendFactor << " * texture(" << texName << "," << uvVariableNameUsed << ")";
		break;
	case JFF::MaterialFunctionCodeBuilder::TextureOp::DIVIDE: // T = T1 / T2
		(*lineStream) << " / " << blendFactor << " * texture(" << texName << "," << uvVariableNameUsed << ")";
		break;
	case JFF::MaterialFunctionCodeBuilder::TextureOp::SMOOTH_ADD: // T = (T1 + T2) - (T1 * T2)
	{
		std::ostringstream ossT2;
		ossT2 << blendFactor << " * texture(" << texName << "," << uvVariableNameUsed << ")";

		auto T1 = lineStream->str();
		auto T2 = ossT2.str();
		lineStream->str("");
		(*lineStream) << "(" << T1 << "+" << T2 << ") - (" << T1 << "*" << T2 << ")";
	}
	break;
	case JFF::MaterialFunctionCodeBuilder::TextureOp::SIGNED_ADD: // T = T1 + (T2 - 0.5)
		(*lineStream) << "+ (" << blendFactor << " * texture(" << texName << "," << uvVariableNameUsed << ")" << " - vec4(0.5) )";
		break;
	default:
		break;
	}
}

inline void JFF::MaterialFunctionCodeBuilderGL::setTextureHeightLine(
	TextureOp texOp,
	float blendFactor, 
	const std::string& texName,
	const std::string& uvVariableNameUsed)
{
	setTextureLine(&params.lineHeight, texOp, blendFactor, texName, uvVariableNameUsed);

	params.lineParallaxIntensity.str("");
	params.lineParallaxIntensity << blendFactor * 0.1f;

	params.lineParallaxFunctionCall.str("");
	params.lineParallaxFunctionCall << "parallaxMappingHeight(" << texName << ")";
}

inline void JFF::MaterialFunctionCodeBuilderGL::setTextureDisplacementLine(
	TextureOp texOp, 
	float blendFactor,
	const std::string& texName, 
	const std::string& uvVariableNameUsed)
{
	setTextureLine(&params.lineDisplacement, texOp, blendFactor, texName, uvVariableNameUsed);

	params.lineParallaxIntensity.str("");
	params.lineParallaxIntensity << blendFactor * 0.1f;

	params.lineParallaxFunctionCall.str("");
	params.lineParallaxFunctionCall << "parallaxMappingDisplacement(" << texName << ")";
}

inline void JFF::MaterialFunctionCodeBuilderGL::setConstantLine(std::ostringstream* lineStream, const Vec4& value)
{
	lineStream->str("");
	(*lineStream) << "vec4(" << value.r << "," << value.g << "," << value.b << "," << value.a << ")";
}

inline void JFF::MaterialFunctionCodeBuilderGL::setConstantHeightLine(const Vec4& value)
{
	setConstantLine(&params.lineHeight, value);

	if (params.lineParallaxIntensity.tellp() == 0)
		params.lineParallaxIntensity << "0.02";

	if (params.lineParallaxFunctionCall.tellp() == 0)
		params.lineParallaxFunctionCall << "parallaxMapping()";
}

inline void JFF::MaterialFunctionCodeBuilderGL::setConstantDisplacementLine(const Vec4& value)
{
	setConstantLine(&params.lineDisplacement, value);

	if (params.lineParallaxIntensity.tellp() == 0)
		params.lineParallaxIntensity << "0.02";

	if (params.lineParallaxFunctionCall.tellp() == 0)
		params.lineParallaxFunctionCall << "parallaxMapping()";
}
