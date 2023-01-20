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

#include "MaterialFunctionCodeBuilder.h"

namespace JFF
{
	class MaterialFunctionCodeBuilderGL : public MaterialFunctionCodeBuilder
	{
	public:
		// Ctor & Dtor
		MaterialFunctionCodeBuilderGL();
		virtual ~MaterialFunctionCodeBuilderGL();

		// Copy ctor and copy assignment
		MaterialFunctionCodeBuilderGL(const MaterialFunctionCodeBuilderGL& other) = delete;
		MaterialFunctionCodeBuilderGL& operator=(const MaterialFunctionCodeBuilderGL& other) = delete;

		// Move ctor and assignment
		MaterialFunctionCodeBuilderGL(MaterialFunctionCodeBuilderGL&& other) = delete;
		MaterialFunctionCodeBuilderGL operator=(MaterialFunctionCodeBuilderGL&& other) = delete;

		// --------------------------- SHADER CODE BUILDER OVERRIDES --------------------------- //

		// Adds a texture line to params
		virtual void addTextureLine(
			const std::string& texName,
			Aplication texApplication,
			TextureMapping textureMapping,
			const std::string& uvVariableNameUsed,
			float blendFactor,
			TextureOp texOp) override;

		// Adds a constant line to params
		virtual void addConstantLine(const Vec4& value, Aplication texApplication) override;

		// Adds an additional function called materialOverrides() function below material() function definition
		virtual void addMaterialOverrideFunction(const std::string& fn) override;

		// Generate a material() function code from params
		virtual void generateCode(std::string& outMaterialFunctionCode, bool useParallaxFunction, bool isPBR) override;

	private:
		// Generic helper for addTextureLine() function
		inline void setTextureLine(
			std::ostringstream* lineStream, 
			TextureOp texOp, 
			float blendFactor, 
			const std::string& texName, 
			const std::string& uvVariableNameUsed);

		// Concrete helper for height and displacement channel in addTextureLine() function
		inline void setTextureHeightLine(
			TextureOp texOp,
			float blendFactor,
			const std::string& texName,
			const std::string& uvVariableNameUsed);
		inline void setTextureDisplacementLine(
			TextureOp texOp,
			float blendFactor,
			const std::string& texName,
			const std::string& uvVariableNameUsed);

		// Generic helper for addConstantLine() function
		inline void setConstantLine(std::ostringstream* lineStream, const Vec4& value);

		// Concrete helper for height and displacement channel in addConstantLine() function
		inline void setConstantHeightLine(const Vec4& value);
		inline void setConstantDisplacementLine(const Vec4& value);

	protected:
		Params params;
	};
}