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

#include "Vec.h"
#include <sstream>

namespace JFF
{
	class MaterialFunctionCodeBuilder
	{
	public:
		enum class Aplication : char
		{
			NO_APPLICATION,

			/*
			* Legacy applications (Phong)
			* NOTE: If any of these values are applied to a texture and this material is configured as GOURAUD or UNLIT,
			* this will automatically switch LightModel as BLINN_PHONG
			*/

			PHONG_DIFFUSE,
			PHONG_SPECULAR,
			PHONG_AMBIENT,
			PHONG_EMISSIVE,
			PHONG_HEIGHT, // The whiter colors, the taller the pixel
			PHONG_NORMAL,
			PHONG_SHININESS, // Specular exponent
			PHONG_OPACITY, // White: opaque, Black: transparent.
			PHONG_DISPLACEMENT, // Higher color values stand for higher vertex displacements
			PHONG_LIGHTMAP, // Ambient occlusion
			PHONG_REFLECTION, // Contains the color of a perfect mirror reflection (rarely used)

			/*
			* PBR applications
			* NOTE: If any of these values are applied to a texture and this material is configured as GOURAUD, PHONG or UNLIT,
			* this will automatically switch LightModel as PBR
			*/

			PBR_BASE_COLOR,
			PBR_NORMAL_CAMERA, // TODO: Look for more info about this parameter: aiTextureType_NORMAL_CAMERA (Assimp)
			PBR_EMISSION_COLOR,
			PBR_METALNESS,
			PBR_DIFFUSE_ROUGHNESS,
			PBR_AMBIENT_OCCLUSION,

			// Advanced PBR parameters

			PBR_SHEEN, // Generally used to simulate textiles that are covered in a layer of microfibers (e.g. velvet)
			PBR_CLEARCOAT, // Simulates a layer of 'polish' or 'laquer' layered on top of a PBR substrate
			PBR_TRANSMISSION, // Simulates transmission through the surface. May include further information such as wall thickness
		};

		// Defines how to extract texels from texture. The most common way is using UV, but other methods are allowed
		enum class TextureMapping : char
		{
			UV,
			SPHERE, // TODO: Not implemented
			CYLINDER, // TODO: Not implemented
			BOX, // TODO: Not implemented
			PLANE, // TODO: Not implemented
			OTHER, // TODO: WTF!?
		};

		// Defines the texture operation to be performed between this texture and the previous texture
		enum class TextureOp :char
		{
			NONE,
			MULTIPLY,	// T = T1 * T2
			ADD,		// T = T1 + T2
			SUBTRACT,	// T = T1 - T2
			DIVIDE,		// T = T1 / T2
			SMOOTH_ADD,	// T = (T1 + T2) - (T1 * T2)
			SIGNED_ADD,	// T = T1 + (T2 - 0.5)
		};

		struct Params
		{
			// ----------------- PARALLAX MAPPING LINES ----------------- //

			std::ostringstream lineHeight;
			std::ostringstream lineDisplacement;
			std::ostringstream lineParallaxIntensity;
			std::ostringstream lineParallaxFunctionCall;

			// ----------------- PHONG TEXTURE LINES ----------------- //

			std::ostringstream lineDiffuse;
			std::ostringstream lineSpecular;
			std::ostringstream lineAmbient;
			std::ostringstream lineEmissive;
			std::ostringstream lineNormal;
			std::ostringstream lineShininess;
			std::ostringstream lineOpacity;
			std::ostringstream lineLightmap;
			std::ostringstream lineReflection;

			// ----------------- PBR TEXTURE LINES ----------------- //

			// Metallic workflow
			std::ostringstream lineBaseColor;
			std::ostringstream lineMetalness;
			std::ostringstream lineDiffuseRoughness;

			// Specular workflow
			// NOTE: This class does nothing when PBR workflow is Specular. The work has to be done in materialOverrides() function

			// Common paramters
			std::ostringstream lineNormalCamera;
			std::ostringstream lineEmissionColor;
			std::ostringstream lineAmbientOcclusion;
				 
			// Special parameters
			std::ostringstream lineSheen;
			std::ostringstream lineClearCoat;
			std::ostringstream lineTransmission;

			// ----------------- materialOverrides() FUNCTION CODE ----------------- //

			std::string materialOverridesCode;
		};

		// Ctor & Dtor
		MaterialFunctionCodeBuilder() {}
		virtual ~MaterialFunctionCodeBuilder() {}

		// Copy ctor and copy assignment
		MaterialFunctionCodeBuilder(const MaterialFunctionCodeBuilder& other) = delete;
		MaterialFunctionCodeBuilder& operator=(const MaterialFunctionCodeBuilder& other) = delete;

		// Move ctor and assignment
		MaterialFunctionCodeBuilder(MaterialFunctionCodeBuilder&& other) = delete;
		MaterialFunctionCodeBuilder operator=(MaterialFunctionCodeBuilder&& other) = delete;

		// --------------------------- SHADER CODE BUILDER INTERFACE --------------------------- //

		// Adds a texture line to params
		virtual void addTextureLine(
			const std::string& texName,
			Aplication texApplication,
			TextureMapping textureMapping,
			const std::string& uvVariableNameUsed,
			float blendFactor,
			TextureOp texOp) = 0;

		// Adds a constant line to params
		virtual void addConstantLine(const Vec4& value, Aplication texApplication) = 0;

		// Adds an additional function called materialOverrides() function below material() function definition
		virtual void addMaterialOverrideFunction(const std::string& fn) = 0;

		// Generate a material() function code from params
		virtual void generateCode(std::string& outMaterialFunctionCode, bool useParallaxFunction, bool isPBR) = 0;

	};
}