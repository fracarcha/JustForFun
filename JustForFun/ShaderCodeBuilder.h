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

#include <vector>
#include <string>

namespace JFF
{
	class ShaderCodeBuilder
	{
	public:
		enum class DebugDisplay : char
		{
			NO_DISPLAY,
			NORMALS,
			POLYGONS,
		};

		enum class PBRWorkflow : char
		{
			METALLIC,
			SPECULAR,
		};

		struct Params // Variable parameters in a shader code
		{
			int shaderVersionMajor;
			int shaderVersionMinor;
			int shaderVersionRevision;
			std::string shaderProfile;

			int maxDirLights;
			int maxPointLights;
			int maxSpotLights;

			std::vector<std::string> textures;
			std::vector<std::string> cubemaps;
			std::string customCode;

			DebugDisplay debugDisplay;

			bool useNormalMap;

			PBRWorkflow pbrWorkflow;
		};

		// Ctor & Dtor
		ShaderCodeBuilder() {}
		virtual ~ShaderCodeBuilder() {}

		// Copy ctor and copy assignment
		ShaderCodeBuilder(const ShaderCodeBuilder& other) = delete;
		ShaderCodeBuilder& operator=(const ShaderCodeBuilder& other) = delete;

		// Move ctor and assignment
		ShaderCodeBuilder(ShaderCodeBuilder&& other) = delete;
		ShaderCodeBuilder operator=(ShaderCodeBuilder&& other) = delete;

		// --------------------------- SHADER CODE BUILDER INTERFACE --------------------------- //

		// Generate a compilable shader code from params
		virtual void generateCode(const Params& params, 
			std::string& outVertexShaderCode,
			std::string& outGeometryShaderCode,
			std::string& outFragmentShaderCode) const = 0;

		// --------------------------- SHADER CODE CONSTANT NAMES ---------------------------//

	public:
		// Matrices
		static const std::string MODEL_MATRIX;
		static const std::string VIEW_MATRIX;
		static const std::string PROJECTION_MATRIX;
		static const std::string NORMAL_MATRIX;
		static const std::string CUBEMAP_VIEW_MATRICES;

		// UVs
		static const std::string INPUT_UV_0;

		// Material output variables
		static const std::string BASE_COLOR;
		static const std::string NORMAL_CAMERA;
		static const std::string EMISSION_COLOR;
		static const std::string METALNESS;
		static const std::string DIFFUSE_ROUGHNESS;
		static const std::string AMBIENT_OCCLUSION;
		static const std::string SHEEN;
		static const std::string CLEAR_COAT;
		static const std::string TRANSMISSION;

		static const std::string HEIGHT;
		static const std::string DISPLACEMENT;
		static const std::string PARALLAX_INTENSITY;
		static const std::string DIFFUSE;
		static const std::string SPECULAR;
		static const std::string AMBIENT;
		static const std::string EMISSIVE;
		static const std::string NORMAL;
		static const std::string SHININESS;
		static const std::string OPACITY;
		static const std::string LIGHTMAP;
		static const std::string REFLECTION;

		// Framebuffer and helper samplers
		static const std::string POST_PROCESSING_OUTPUT_COLOR;
		static const std::string POST_PROCESSING_OUTPUT_COLOR_2;
		static const std::string POST_PROCESSING_FRAGMENT_WORLD_POS;
		static const std::string POST_PROCESSING_NORMAL_WORLD_DIR;
		static const std::string POST_PROCESSING_ALBEDO_SPECULAR;
		static const std::string POST_PROCESSING_AMBIENT_SHININESS;
		static const std::string POST_PROCESSING_REFLECTION;
		static const std::string POST_PROCESSING_EMISSIVE;
		static const std::string MIPMAP_LEVEL;
		static const std::string MIPMAP_LEVELS;
		static const std::string THRESHOLD;
		static const std::string INTENSITY;
		static const std::string RANDOM_ROTATED_TANGENTS;
		static const std::string HEMISPHERE_RADIUS;
		static const std::string NUM_HEMISPHERE_SAMPLES;
		static const std::string HEMISPHERE_SAMPLES;
		static const std::string EQUIRECTANGULAR_TEX;
		static const std::string ROUGHNESS;
		static const std::string ENVIRONMENT_MAP_FACE_WIDTH;

		// Environment map samplers
		static const std::string ENVIRONMENT_MAP;
		static const std::string IRRADIANCE_MAP;
		static const std::string PRE_FILTERED_MAP;
		static const std::string BRDF_INTEGRATION_MAP;

		// Light params
		static const std::string DIRECTIONAL_LIGHT_MATRIX;
		static const std::string DIRECTIONAL_LIGHT_STRUCT;
		static const std::string DIRECTIONAL_LIGHT_MATRICES;
		static const std::string DIRECTIONAL_LIGHT_STRUCT_ARRAY;
			static const std::string DIR_LIGHT_DIRECTION;
			static const std::string DIR_LIGHT_COLOR;
			static const std::string DIR_LIGHT_INTENSITY;
			static const std::string DIR_LIGHT_CAST_SHADOWS;
			static const std::string DIR_LIGHT_SHADOW_MAP;

		static const std::string POINT_LIGHT_STRUCT;
		static const std::string POINT_LIGHT_STRUCT_ARRAY;
			static const std::string POINT_LIGHT_POSITION;
			static const std::string POINT_LIGHT_COLOR;
			static const std::string POINT_LIGHT_INTENSITY;
			static const std::string POINT_LIGHT_LINEAR_ATTENUATION_FACTOR;
			static const std::string POINT_LIGHT_QUADRATIC_ATTENUATION_FACTOR;
			static const std::string POINT_LIGHT_CAST_SHADOWS;
			static const std::string POINT_LIGHT_SHADOW_MAP;
			static const std::string POINT_LIGHT_FAR_PLANE;

		static const std::string SPOT_LIGHT_MATRIX;
		static const std::string SPOT_LIGHT_STRUCT;
		static const std::string SPOT_LIGHT_MATRICES;
		static const std::string SPOT_LIGHT_STRUCT_ARRAY;
			static const std::string SPOT_LIGHT_POSITION;
			static const std::string SPOT_LIGHT_DIRECTION;
			static const std::string SPOT_LIGHT_COLOR;
			static const std::string SPOT_LIGHT_INTENSITY;
			static const std::string SPOT_LIGHT_LINEAR_ATTENUATION_FACTOR;
			static const std::string SPOT_LIGHT_QUADRATIC_ATTENUATION_FACTOR;
			static const std::string SPOT_LIGHT_INNER_HALF_ANGLE_CUTOFF;
			static const std::string SPOT_LIGHT_OUTER_HALF_ANGLE_CUTOFF;
			static const std::string SPOT_LIGHT_CAST_SHADOWS;
			static const std::string SPOT_LIGHT_SHADOW_MAP;

		static const std::string LIGHT_POSITION;
		static const std::string LIGHT_FAR_PLANE;

		// Material function definitions
		static const std::string MATERIAL_FUNCTION_HEADER;
		static const std::string MATERIAL_FUNCTION_CLOSE_BRACKET;
		static const std::string MATERIAL_OVERRIDES_EMPTY_FUNCTION;
	};
}