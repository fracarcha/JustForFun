/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "MaterialGL.h"

#include "Log.h"
#include "Engine.h"
#include "ShaderCodeBuilder.h"
#include "FileSystemSetup.h"

#include <stdexcept>
#include <memory>
#include <sstream>
#include <regex>
#include <algorithm>

extern std::shared_ptr<JFF::Texture> createTexture(JFF::Engine* const engine, const char* name, const char* assetFilePath);
extern std::shared_ptr<JFF::Cubemap> createCubemap(JFF::Engine* const engine, const char* name, const char* assetFilePath);
extern std::shared_ptr<JFF::ShaderCodeBuilder> createShaderCodeBuilder(
	JFF::Renderer::RenderPath renderPath, JFF::Material::MaterialDomain domain, JFF::Material::LightModel lightModel);

JFF::MaterialGL::MaterialGL(JFF::Engine* const engine, const char* name, const char* assetFilePath) :
	engine(engine),
	cooked(false),
	isDestroyed(false),

	name(name),
	program(0u),
	domain(MaterialDomain::SURFACE),
	lightModel(LightModel::GOURAUD),
	side(Side::FRONT),
	useNormalMap(false),
	pbrWorkflow(PBRWorkflow::METALLIC),

	debugDisplay(DebugDisplay::NO_DISPLAY),

	postProcessParams(),

	environmentMaps(),
	textures(),
	cubemaps(),
	postProcessingTextures(),
	directionalLightShadowMaps(),
	pointLightShadowCubemaps(),
	spotLightShadowMaps(),
	customCode(),
	textureUnit(0)
{
	JFF_LOG_INFO("Ctor MaterialGL from file")

	std::shared_ptr<INIFile> iniFile = engine->io.lock()->loadINIFile(assetFilePath); // Load filename
	extractMaterialOptionsFromFile(iniFile);		// Extract material options
	extractMaterialDebugOptionsFromFile(iniFile);	// Extract material debug options
	extractPostProcessOptionsFromFile(iniFile);		// Extract material post process options
	loadTexturesFromFile(iniFile, engine);			// Load all associated textures
	loadCubemapsFromFile(iniFile, engine);			// Load all associated cubemaps
	extractCustomCodeFromFile(iniFile);				// Extract custom code
	cook(); // Cook the material
}

JFF::MaterialGL::MaterialGL(JFF::Engine* const engine, const char* name) :
	engine(engine),
	cooked(false),
	isDestroyed(false),

	name(name),
	program(0u),
	domain(MaterialDomain::SURFACE),
	lightModel(LightModel::GOURAUD),
	side(Side::FRONT),
	useNormalMap(false),
	pbrWorkflow(PBRWorkflow::METALLIC),

	debugDisplay(DebugDisplay::NO_DISPLAY),

	postProcessParams(),

	environmentMaps(),
	textures(),
	cubemaps(),
	postProcessingTextures(),
	directionalLightShadowMaps(),
	pointLightShadowCubemaps(),
	spotLightShadowMaps(),
	customCode(),
	textureUnit(0)
{
	JFF_LOG_INFO("Ctor MaterialGL")
}

JFF::MaterialGL::~MaterialGL()
{
	JFF_LOG_INFO("Dtor MaterialGL")

	// Ensure the material GPU memory is destroyed
	if (!isDestroyed)
	{
		JFF_LOG_WARNING("Material GPU memory successfully destroyed on Material's destructor. You should call destroy() before destructor is called")
		destroy();
	}
}

void JFF::MaterialGL::setName(const char* name)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot set name on a cooked material. Aborted")
		return;
	}
	this->name = name;
}

void JFF::MaterialGL::setDomain(MaterialDomain domain)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot set domain on a cooked material. Aborted")
		return;
	}
	this->domain = domain;
}

void JFF::MaterialGL::setLightModel(LightModel lightModel)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot set light model on a cooked material. Aborted")
		return;
	}
	this->lightModel = lightModel;
}

void JFF::MaterialGL::setPBRWorkflow(PBRWorkflow pbrWorkflow)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot set PBR workflow on a cooked material. Aborted")
		return;
	}
	this->pbrWorkflow = pbrWorkflow;
}

void JFF::MaterialGL::setSide(Side side)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot set drawable side on a cooked material. Aborted")
		return;
	}
	this->side = side;
}

void JFF::MaterialGL::setUseNormalMap(bool useNormalMap)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot set use of normal map on a cooked material. Aborted")
		return;
	}
	this->useNormalMap = useNormalMap;
}

void JFF::MaterialGL::addTexture(const std::shared_ptr<Texture>& texture)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot add new textures in a cooked material. Aborted")
		return;
	}

	// Don't add a repeated texture
	auto predicate = [&texture](std::tuple<int, std::string, std::shared_ptr<Texture>>& tuple)
	{
		std::string texName = std::get<1>(tuple);
		return texName == texture->getName();
	};
	if (std::find_if(textures.begin(), textures.end(), predicate) != textures.end())
		return;

	// Load and add the texture to texture list
	textures.push_back(std::tuple<int, std::string, std::shared_ptr<Texture>>(textureUnit, texture->getName(), texture));
	++textureUnit;
}

void JFF::MaterialGL::cook(const std::string& externalCustomCode)
{
	if (cooked)
	{
		JFF_LOG_WARNING("Cannot cook an already cooked material. Aborted")
		return;
	}

	// ------------------------------------ ADD EXTERNAL CUSTOM CODE IF APPLICABLE ------------------------------- //

	customCode << externalCustomCode;

	// ------------------------------------ EXTRACT SPECIAL TEXTURES IF APPLICABLE ------------------------------- //

	extractEnvironmentMaps();
	extractPostProcessingTextures();
	extractShadowMaps();

	// ------------------------------------ COMBINE ALL BLOCKS OF SHADER CODE ------------------------------------ //

	auto context = engine->context.lock();
	auto renderer = engine->renderer.lock();

	// Gather all params needed to build shader code
	ShaderCodeBuilder::Params shaderCodeParams;

	shaderCodeParams.shaderVersionMajor = context->getGraphicsAPIVersionMajor();
	shaderCodeParams.shaderVersionMinor = context->getGraphicsAPIVersionMinor();
	shaderCodeParams.shaderVersionRevision = context->getGraphicsAPIRevisionNumber();
	shaderCodeParams.shaderProfile = context->getGraphicsAPIContexProfile();

	shaderCodeParams.maxDirLights = renderer->getForwardShadingMaxDirectionalLights();
	shaderCodeParams.maxPointLights = renderer->getForwardShadingMaxPointLights();
	shaderCodeParams.maxSpotLights = renderer->getForwardShadingMaxSpotLights();

	for (const auto& tuple : textures)
	{
		std::string texName = std::get<1>(tuple);
		shaderCodeParams.textures.push_back(texName);
	}

	for (const auto& tuple : cubemaps)
	{
		std::string cubemapName = std::get<1>(tuple);
		shaderCodeParams.cubemaps.push_back(cubemapName);
	}

	shaderCodeParams.customCode = customCode.str();
	shaderCodeParams.useNormalMap = useNormalMap;
	shaderCodeParams.pbrWorkflow = pbrWorkflow == PBRWorkflow::METALLIC ? 
		ShaderCodeBuilder::PBRWorkflow::METALLIC : ShaderCodeBuilder::PBRWorkflow::SPECULAR;

	switch (debugDisplay)
	{
	case JFF::Material::DebugDisplay::NO_DISPLAY:
		shaderCodeParams.debugDisplay = ShaderCodeBuilder::DebugDisplay::NO_DISPLAY;
		break;
	case JFF::Material::DebugDisplay::NORMALS:
		shaderCodeParams.debugDisplay = ShaderCodeBuilder::DebugDisplay::NORMALS;
		break;
	case JFF::Material::DebugDisplay::POLYGONS:
		shaderCodeParams.debugDisplay = ShaderCodeBuilder::DebugDisplay::POLYGONS;
		break;
	default:
		break;
	}

	// -------------------------------- SHADER CODE GENERATION -------------------------------- //

	auto shaderCodeBuilder = createShaderCodeBuilder(renderer->getRenderPath(), domain, lightModel);

	std::string vertexShaderCode;
	std::string geometryShaderCode;
	std::string fragmentShaderCode;
	shaderCodeBuilder->generateCode(shaderCodeParams, vertexShaderCode, geometryShaderCode, fragmentShaderCode);

	const char* vertexShaderCode_c_str = vertexShaderCode.c_str();
	const char* geometryShaderCode_c_str = geometryShaderCode.c_str();
	const char* fragmentShaderCode_c_str = fragmentShaderCode.c_str();

	// ------------------------------------- SHADER COMPILATION ------------------------------------- //

	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertexShader, (GLsizei) vertexSrc.size(), vertexSrc.data(), nullptr); // Alternative using arrays
	glShaderSource(vertexShader, 1, &vertexShaderCode_c_str, NULL);
	glCompileShader(vertexShader);
	checkShaderCompilation(vertexShader);

	// Geometry shader (Optional)
	GLuint geometryShader = 0;
	if (!geometryShaderCode.empty())
	{
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryShaderCode_c_str, NULL);
		glCompileShader(geometryShader);
		checkShaderCompilation(geometryShader);
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader, (GLsizei) fragmentSrc.size(), fragmentSrc.data(), nullptr); // Alternative using arrays
	glShaderSource(fragmentShader, 1, &fragmentShaderCode_c_str, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompilation(fragmentShader);

	// Program link
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	if (!geometryShaderCode.empty()) glAttachShader(program, geometryShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	checkProgramLinkStatus(program);

	// Flag shaders for deletion when program is destroyed
	glDeleteShader(vertexShader);
	if (!geometryShaderCode.empty()) glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	// Link CameraParams uniform block to the corresponding binding point
	GLuint cameraParamsBindingPoint = 0u; // Check CameraComponentGL to ensure cameras use the same binding point for camera params
	GLuint cameraParamsUniformBlockIndex = glGetUniformBlockIndex(program, "CameraParams");
	if (cameraParamsUniformBlockIndex != GL_INVALID_INDEX)
	{
		glUniformBlockBinding(program, cameraParamsUniformBlockIndex, cameraParamsBindingPoint);
	}

	// Clean temp attributes
	customCode.clear(); 

	// Flag this material as cooked
	cooked = true;
}

void JFF::MaterialGL::use()
{
	// Enables the program
	glUseProgram(program);

	// Use all associated textures
	std::for_each(textures.begin(), textures.end(), [this](const auto& tuple)
		{
			int texUnit = std::get<0>(tuple);
			std::string texName = std::get<1>(tuple);
			std::shared_ptr<Texture> texture = std::get<2>(tuple);

			texture->use(texUnit);
			sendTexture(texName.c_str(), texUnit);
		});

	// Use all associated cubemaps
	std::for_each(cubemaps.begin(), cubemaps.end(), [this](const auto& tuple)
		{
			int texUnit = std::get<0>(tuple);
			std::string cubemapName = std::get<1>(tuple);
			std::shared_ptr<Cubemap> cubemap = std::get<2>(tuple);

			cubemap->use(texUnit);
			sendTexture(cubemapName.c_str(), texUnit);
		});
}

void JFF::MaterialGL::sendMat4(const char* variableName, const Mat4& matrix)
{
	GLint location = glGetUniformLocation(program, variableName);
	GLsizei numMatricesSent = 1;
	GLboolean shouldTranspose = GL_FALSE;
	glUniformMatrix4fv(location, numMatricesSent, shouldTranspose, *matrix);
}

void JFF::MaterialGL::sendMat3(const char* variableName, const Mat3& matrix)
{
	GLint location = glGetUniformLocation(program, variableName);
	GLsizei numMatricesSent = 1;
	GLboolean shouldTranspose = GL_FALSE;
	glUniformMatrix3fv(location, numMatricesSent, shouldTranspose, *matrix);
}

void JFF::MaterialGL::sendVec2(const char* variableName, const Vec2& vec)
{
	GLint location = glGetUniformLocation(program, variableName);
	GLsizei numVectorsSent = 1;
	glUniform2fv(location, numVectorsSent, *vec);
}

void JFF::MaterialGL::sendVec3(const char* variableName, const Vec3& vec)
{
	GLint location = glGetUniformLocation(program, variableName);
	GLsizei numVectorsSent = 1;
	glUniform3fv(location, numVectorsSent, *vec);
}

void JFF::MaterialGL::sendVec4(const char* variableName, const Vec4& vec)
{
	GLint location = glGetUniformLocation(program, variableName);
	GLsizei numVectorsSent = 1;
	glUniform4fv(location, numVectorsSent, *vec);
}

void JFF::MaterialGL::sendFloat(const char* variableName, float f)
{
	GLint location = glGetUniformLocation(program, variableName);
	glUniform1f(location, f);
}

void JFF::MaterialGL::sendInt(const char* variableName, int i)
{
	GLint location = glGetUniformLocation(program, variableName);
	glUniform1i(location, i);
}

void JFF::MaterialGL::sendEnvironmentMap(
	const std::shared_ptr<Cubemap>& envMap,
	const std::shared_ptr<Cubemap>& irradianceMap,
	const std::shared_ptr<Cubemap>& preFilteredMap,
	const std::shared_ptr<Texture>& BRDFIntegrationMap)
{
	switch (domain)
	{
	case JFF::Material::MaterialDomain::SURFACE:
	case JFF::Material::MaterialDomain::TRANSLUCENT:
		switch (lightModel)
		{
		case JFF::Material::LightModel::PHONG:
		case JFF::Material::LightModel::BLINN_PHONG:
			{
				// Extract environment shader variable name and texture unit
				auto tuple = environmentMaps[0]; // Use the first (there's only one) environment map info
				int texUnit = std::get<0>(tuple);
				std::string envName = std::get<1>(tuple);

				if (envMap)
				{
					envMap->use(texUnit); // Send cubemap to the shader
				}
				else
				{
					// Send empty cubemap to the shader
					glActiveTexture(GL_TEXTURE0 + texUnit);
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Bind default texture // TODO: Check
				}

				sendTexture(envName.c_str(), texUnit);
			}
			break;
		case JFF::Material::LightModel::PBR:
			{
				// Extract irradiance map shader variable name and texture unit
				auto tuple = environmentMaps[0];
				int texUnit = std::get<0>(tuple);
				std::string varName = std::get<1>(tuple);

				if (irradianceMap)
				{
					irradianceMap->use(texUnit); // Send cubemap to the shader
				}
				else
				{
					// Send empty cubemap to the shader
					glActiveTexture(GL_TEXTURE0 + texUnit);
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Bind default texture // TODO: Check
				}

				sendTexture(varName.c_str(), texUnit);
			}
			{
				// Extract pre-filtered map shader variable name and texture unit
				auto tuple = environmentMaps[1]; 
				int texUnit = std::get<0>(tuple);
				std::string varName = std::get<1>(tuple);

				if (preFilteredMap)
				{
					preFilteredMap->use(texUnit);
				}
				else
				{
					// Send empty cubemap to the shader
					glActiveTexture(GL_TEXTURE0 + texUnit);
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Bind default texture // TODO: Check
				}

				sendTexture(varName.c_str(), texUnit);
			}
			{
				// Extract BRDF integration map shader variable name and texture unit
				auto tuple = environmentMaps[2];
				int texUnit = std::get<0>(tuple);
				std::string varName = std::get<1>(tuple);

				if (BRDFIntegrationMap)
				{
					BRDFIntegrationMap->use(texUnit);
				}
				else
				{
					// Send empty texture to the shader
					glActiveTexture(GL_TEXTURE0 + texUnit);
					glBindTexture(GL_TEXTURE_2D, 0); // Bind default texture // TODO: Check
				}
				
				sendTexture(varName.c_str(), texUnit);
			}
			break;
		case JFF::Material::LightModel::GOURAUD:
		case JFF::Material::LightModel::UNLIT:
		default:
			break;
		}
		break;
	case JFF::Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED:
	case JFF::Material::MaterialDomain::IRRADIANCE_GENERATOR:
	case JFF::Material::MaterialDomain::PRE_FILTERED_ENVIRONMENT_MAP_GENERATOR:
		{
			// Extract environment shader variable name and texture unit
			auto tuple = environmentMaps[0]; // Use the first (there's only one) environment map info
			int texUnit = std::get<0>(tuple);
			std::string envName = std::get<1>(tuple);

			if (envMap)
			{
				envMap->use(texUnit); // Send cubemap to the shader
			}
			else
			{
				// Send empty cubemap to the shader
				glActiveTexture(GL_TEXTURE0 + texUnit);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Bind default texture // TODO: Check
			}

			sendTexture(envName.c_str(), texUnit);
		}
		break;
	default:
		break;
	}
}

void JFF::MaterialGL::sendDirLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer> shadowMapFBO)
{
	switch (domain)
	{
	case JFF::Material::MaterialDomain::SURFACE:
	case JFF::Material::MaterialDomain::TRANSLUCENT:
		switch (lightModel)
		{
		case JFF::Material::LightModel::GOURAUD: // TODO: Does Gouraud receive shadow maps ???
		case JFF::Material::LightModel::PHONG:
		case JFF::Material::LightModel::BLINN_PHONG:
		case JFF::Material::LightModel::PBR:
			{
				// Extract directional light shadow map variable name and texture unit
				auto tuple = directionalLightShadowMaps[index];

				int texUnit = std::get<0>(tuple);
				std::string shadowMapName = std::get<1>(tuple);
				Framebuffer::AttachmentPoint attachmentPoint = std::get<2>(tuple);

				// Send texture to shader
				if (shadowMapFBO.expired())
				{
					glActiveTexture(GL_TEXTURE0 + texUnit);
					glBindTexture(GL_TEXTURE_2D, 0); // Bind default texture // TODO: check
				}
				else
				{
					shadowMapFBO.lock()->useTexture(attachmentPoint, texUnit);
				}
				sendTexture(shadowMapName.c_str(), texUnit);							
			}
			break;
		case JFF::Material::LightModel::UNLIT:
		default:
			break;
		}
		break;
	case JFF::Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED:
		{
			// Extract directional light shadow map variable name and texture unit
			auto tuple = directionalLightShadowMaps[index];

			int texUnit = std::get<0>(tuple);
			std::string shadowMapName = std::get<1>(tuple);
			Framebuffer::AttachmentPoint attachmentPoint = std::get<2>(tuple);

			// Send texture to shader
			if (shadowMapFBO.expired())
			{
				glActiveTexture(GL_TEXTURE0 + texUnit);
				glBindTexture(GL_TEXTURE_2D, 0); // Bind default texture // TODO: check
			}
			else
			{
				shadowMapFBO.lock()->useTexture(attachmentPoint, texUnit);
			}
			sendTexture(shadowMapName.c_str(), texUnit);
		}
		break;
	default:
		break;
	}
}

void JFF::MaterialGL::sendPointLightShadowCubemap(unsigned int index, const std::weak_ptr<Framebuffer>& shadowCubemapFBO)
{
	switch (domain)
	{
	case JFF::Material::MaterialDomain::SURFACE:
	case JFF::Material::MaterialDomain::TRANSLUCENT:
		switch (lightModel)
		{
		case JFF::Material::LightModel::GOURAUD:
		case JFF::Material::LightModel::PHONG:
		case JFF::Material::LightModel::BLINN_PHONG:
		case JFF::Material::LightModel::PBR:
			{
				// Extract point light shadow cubemap variable name and texture unit
				auto tuple = pointLightShadowCubemaps[index];

				int texUnit = std::get<0>(tuple);
				std::string shadowCubemapName = std::get<1>(tuple);
				Framebuffer::AttachmentPoint attachmentPoint = std::get<2>(tuple);

				// Send texture to shader
				if (shadowCubemapFBO.expired())
				{
					glActiveTexture(GL_TEXTURE0 + texUnit);
					glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Bind default cubemap // TODO: check
				}
				else
				{
					shadowCubemapFBO.lock()->useTexture(attachmentPoint, texUnit);
				}
				sendTexture(shadowCubemapName.c_str(), texUnit);
			}
			break;
		case JFF::Material::LightModel::UNLIT:
		default:
			break;
		}
		break;
	case JFF::Material::MaterialDomain::POINT_LIGHTING_DEFERRED:
		{
			// Extract point light shadow cubemap variable name and texture unit
			auto tuple = pointLightShadowCubemaps[index];

			int texUnit = std::get<0>(tuple);
			std::string shadowCubemapName = std::get<1>(tuple);
			Framebuffer::AttachmentPoint attachmentPoint = std::get<2>(tuple);

			// Send texture to shader
			if (shadowCubemapFBO.expired())
			{
				glActiveTexture(GL_TEXTURE0 + texUnit);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Bind default cubemap // TODO: check
			}
			else
			{
				shadowCubemapFBO.lock()->useTexture(attachmentPoint, texUnit);
			}
			sendTexture(shadowCubemapName.c_str(), texUnit);
		}
		break;
	default:
		break;
	}
}

void JFF::MaterialGL::sendSpotLightShadowMap(unsigned int index, const std::weak_ptr<Framebuffer> shadowMapFBO)
{
	switch (domain)
	{
	case JFF::Material::MaterialDomain::SURFACE:
	case JFF::Material::MaterialDomain::TRANSLUCENT:
		switch (lightModel)
		{
		case JFF::Material::LightModel::GOURAUD:
		case JFF::Material::LightModel::PHONG:
		case JFF::Material::LightModel::BLINN_PHONG:
		case JFF::Material::LightModel::PBR:
			{
				// Extract spot light shadow map variable name and texture unit
				auto tuple = spotLightShadowMaps[index];

				int texUnit = std::get<0>(tuple);
				std::string shadowMapName = std::get<1>(tuple);
				Framebuffer::AttachmentPoint attachmentPoint = std::get<2>(tuple);

				// Send texture to shader
				if (shadowMapFBO.expired())
				{
					glActiveTexture(GL_TEXTURE0 + texUnit);
					glBindTexture(GL_TEXTURE_2D, 0); // Bind default texture // TODO: check
				}
				else
				{
					shadowMapFBO.lock()->useTexture(attachmentPoint, texUnit);
				}
				sendTexture(shadowMapName.c_str(), texUnit);				
			}
			break;
		case JFF::Material::LightModel::UNLIT:
		default:
			break;
		}
		break;
	case JFF::Material::MaterialDomain::SPOT_LIGHTING_DEFERRED:
		{
			// Extract spot light shadow map variable name and texture unit
			auto tuple = spotLightShadowMaps[index];

			int texUnit = std::get<0>(tuple);
			std::string shadowMapName = std::get<1>(tuple);
			Framebuffer::AttachmentPoint attachmentPoint = std::get<2>(tuple);

			// Send texture to shader
			if (shadowMapFBO.expired())
			{
				glActiveTexture(GL_TEXTURE0 + texUnit);
				glBindTexture(GL_TEXTURE_2D, 0); // Bind default texture // TODO: check
			}
			else
			{
				shadowMapFBO.lock()->useTexture(attachmentPoint, texUnit);
			}
			sendTexture(shadowMapName.c_str(), texUnit);
		}
		break;
	default:
		break;
	}
}

void JFF::MaterialGL::sendPostProcessingTextures(const std::weak_ptr<Framebuffer>& ppFBO, const std::weak_ptr<Framebuffer>& ppFBO2)
{
	std::for_each(postProcessingTextures.begin(), postProcessingTextures.end(), [this, &ppFBO, &ppFBO2](const auto& tuple)
		{
			int texUnit = std::get<0>(tuple);
			std::string texName = std::get<1>(tuple);
			Framebuffer::AttachmentPoint attachmentPoint = std::get<2>(tuple);
			int usedFBO = std::get<3>(tuple);

			if (usedFBO <= 0)
				ppFBO.lock()->useTexture(attachmentPoint, texUnit);
			else
				ppFBO2.lock()->useTexture(attachmentPoint, texUnit);
			
			sendTexture(texName.c_str(), texUnit);
		});
}

void JFF::MaterialGL::destroy()
{
	// Delete program from memory. Because shaders were deleted before, this effectively deletes linked shaders too
	glDeleteProgram(program);

	// Destroy textures
	std::for_each(textures.begin(), textures.end(), [this](const auto& tuple) 
		{
			std::shared_ptr<Texture> texture = std::get<2>(tuple);
			texture->destroy(); // TODO: Careful trying to destroy a cached texture
		});

	// Destroy cubemaps
	std::for_each(cubemaps.begin(), cubemaps.end(), [this](const auto& tuple)
		{
			std::shared_ptr<Cubemap> cubemap = std::get<2>(tuple);
			cubemap->destroy();  // TODO: Careful trying to destroy a cached cubemap
		});

	isDestroyed = true;
}

inline bool JFF::MaterialGL::checkShaderCompilation(GLuint shader)
{
	int shaderSuccess;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderSuccess);

	int infoStringLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoStringLength);
	std::vector<char> shaderInfoLog(infoStringLength, '\0');

	if (!shaderSuccess)
	{
		glGetShaderInfoLog(shader, (GLsizei) shaderInfoLog.size(), nullptr, shaderInfoLog.data());
		JFF_LOG_ERROR("Shader compilation failed: " << shaderInfoLog.data())
		return false;
	}

	return true;
}

inline bool JFF::MaterialGL::checkProgramLinkStatus(GLuint program)
{
	int programLinkSuccess;
	glGetProgramiv(program, GL_LINK_STATUS, &programLinkSuccess);
	
	int infoStringLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoStringLength);
	std::vector<char> programLinkInfoLog(infoStringLength, '\0');

	if (!programLinkSuccess)
	{
		glGetProgramInfoLog(program, (GLsizei) programLinkInfoLog.size(), nullptr, programLinkInfoLog.data());
		JFF_LOG_ERROR("Program link failed: " << programLinkInfoLog.data())
		return false;
	}

	return true;
}

void JFF::MaterialGL::sendTexture(const char* variableName, int textureUnit)
{
	GLint uniformLocation = glGetUniformLocation(program, variableName);
	glUniform1i(uniformLocation, textureUnit); // Uses currently active program. Remember to call glUseProgram() first
}

inline void JFF::MaterialGL::extractMaterialOptionsFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	if(iniFile->has("config", "material-domain"))
		extractMaterialDomain(iniFile->getString("config", "material-domain"));

	if(iniFile->has("config", "light-model"))
		extractLightingModel(iniFile->getString("config", "light-model"));

	if(iniFile->has("config", "side"))
		extractSide(iniFile->getString("config", "side"));

	if (iniFile->has("config", "use-normal-map"))
		extractUseNormalMap(iniFile->getString("config", "use-normal-map"));

	if (iniFile->has("config", "pbr-workflow"))
		extractPBRWorkflow(iniFile->getString("config", "pbr-workflow"));

	// TODO: Add more options here
}

inline void JFF::MaterialGL::extractMaterialDomain(const std::string& option)
{
	if (option == "SURFACE")
		domain = MaterialDomain::SURFACE;
	else if (option == "BACKGROUND")
		domain = MaterialDomain::BACKGROUND;
	else if (option == "TRANSLUCENT")
		domain = MaterialDomain::TRANSLUCENT;
	else if (option == "DEBUG")
		domain = MaterialDomain::DEBUG;
	else if (option == "POST_PROCESS")
		domain = MaterialDomain::POST_PROCESS;
	else if (option == "UI")
		domain = MaterialDomain::UI;
}

inline void JFF::MaterialGL::extractLightingModel(const std::string& option)
{
	if (option == "PHONG")
		lightModel = LightModel::PHONG;
	else if (option == "BLINN_PHONG")
		lightModel = LightModel::BLINN_PHONG;
	else if (option == "PBR")
		lightModel = LightModel::PBR;
	else if (option == "GOURAUD")
		lightModel = LightModel::GOURAUD;
	else if (option == "UNLIT")
		lightModel = LightModel::UNLIT;
}

inline void JFF::MaterialGL::extractSide(const std::string& option)
{
	if (option == "FRONT")
		side = Side::FRONT;
	else if (option == "BACK")
		side = Side::BACK;
	else if (option == "TWO_SIDED")
		side = Side::TWO_SIDED;
}

inline void JFF::MaterialGL::extractUseNormalMap(const std::string& option)
{
	useNormalMap = option == "true";
}

inline void JFF::MaterialGL::extractPBRWorkflow(const std::string& option)
{
	if (option == "METALLIC")
		pbrWorkflow = PBRWorkflow::METALLIC;
	else if (option == "SPECULAR")
		pbrWorkflow = PBRWorkflow::SPECULAR;
}

inline void JFF::MaterialGL::extractMaterialDebugOptionsFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	// Return if this material domain isn't DEBUG
	if (domain != MaterialDomain::DEBUG)
		return;

	extractDebugDisplay(iniFile->getString("debug", "display"));
	// TODO: Add more debug options here
}

inline void JFF::MaterialGL::extractDebugDisplay(const std::string& option)
{
	if (option == "NORMALS")
		debugDisplay = DebugDisplay::NORMALS;
	else if (option == "POLYGONS")
		debugDisplay = DebugDisplay::POLYGONS;

}

inline void JFF::MaterialGL::extractPostProcessOptionsFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	// Return if this material domain isn't post process
	if (domain != MaterialDomain::POST_PROCESS)
		return;

	if (iniFile->has("post-process", "bloom"))
		extractBloomEnabled(iniFile->getString("post-process", "bloom"));

	if (iniFile->has("post-process", "bloom-threshold"))
		extractBloomThreshold(iniFile->getString("post-process", "bloom-threshold"));

	if (iniFile->has("post-process", "bloom-intensity"))
		extractBloomIntensity(iniFile->getString("post-process", "bloom-intensity"));


	if (iniFile->has("post-process", "ssao"))
		extractSSAOEnabled(iniFile->getString("post-process", "ssao"));

	if (iniFile->has("post-process", "ssao-intensity"))
		extractSSAOIntensity(iniFile->getFloat("post-process", "ssao-intensity"));

	if (iniFile->has("post-process", "ssao-num-samples"))
		extractSSAONumSamples(iniFile->getInt("post-process", "ssao-num-samples"));

	if (iniFile->has("post-process", "ssao-sample-hemisphere-radius"))
		extractSSAOSampleHemisphereRadius(iniFile->getFloat("post-process", "ssao-sample-hemisphere-radius"));

	if (iniFile->has("post-process", "ssao-num-blur-steps"))
		extractSSAONumBlurSteps(iniFile->getInt("post-process", "ssao-num-blur-steps"));
}

inline void JFF::MaterialGL::extractBloomEnabled(const std::string& option)
{
	postProcessParams.bloomEnabled = option == "true";
}

inline void JFF::MaterialGL::extractBloomThreshold(const std::string& option)
{
	try
	{
		float threshold = std::stof(option);
		postProcessParams.bloomThreshold = threshold;
	}
	catch (std::invalid_argument e)
	{
		JFF_LOG_WARNING("Invalid bloom-threshold value. This field is of type float")
	}
	catch (std::out_of_range e)
	{
		JFF_LOG_WARNING("Invalid bloom-threshold value. Value entered is out of range")
	}
}

inline void JFF::MaterialGL::extractBloomIntensity(const std::string& option)
{
	try
	{
		float intensity = std::stof(option);
		postProcessParams.bloomIntensity = intensity;
	}
	catch (std::invalid_argument e)
	{
		JFF_LOG_WARNING("Invalid bloom-intensity value. This field is of type float")
	}
	catch (std::out_of_range e)
	{
		JFF_LOG_WARNING("Invalid bloom-intensity value. Value entered is out of range")
	}
}

inline void JFF::MaterialGL::extractSSAOEnabled(const std::string& option)
{
	postProcessParams.SSAOEnabled = option == "true";
}

inline void JFF::MaterialGL::extractSSAOIntensity(float option)
{
	postProcessParams.SSAOIntensity = option;
}

inline void JFF::MaterialGL::extractSSAONumSamples(int option)
{
	postProcessParams.SSAONumSamples = (unsigned int)option;
}

inline void JFF::MaterialGL::extractSSAOSampleHemisphereRadius(float option)
{
	postProcessParams.SSAOSampleHemisphereRadius = option;
}

inline void JFF::MaterialGL::extractSSAONumBlurSteps(int option)
{
	postProcessParams.SSAONumBlurSteps = (unsigned int)option;
}

inline void JFF::MaterialGL::loadTexturesFromFile(const std::shared_ptr<INIFile>& iniFile, Engine* const engine)
{
	iniFile->visitKeyValuePairs("textures", [this, &engine](const std::pair<std::string, std::string>& pair)
		{
			std::string assetFullPath = std::regex_replace(pair.second, std::regex(R"raw(/)raw"), JFF_SLASH_STRING);
			std::shared_ptr<Texture> texture = createTexture(engine, pair.first.c_str(), assetFullPath.c_str());
			textures.push_back(std::tuple<int, std::string, std::shared_ptr<Texture>>(textureUnit, texture->getName(), texture));
			++textureUnit;
		});
}

inline void JFF::MaterialGL::loadCubemapsFromFile(const std::shared_ptr<INIFile>& iniFile, Engine* const engine)
{
	iniFile->visitKeyValuePairs("cubemaps", [this, &engine](const std::pair<std::string, std::string>& pair)
		{
			std::string assetFullPath = std::regex_replace(pair.second, std::regex(R"raw(/)raw"), JFF_SLASH_STRING);
			std::shared_ptr<Cubemap> cubemap = createCubemap(engine, pair.first.c_str(), assetFullPath.c_str());
			cubemaps.push_back(std::tuple<int, std::string, std::shared_ptr<Cubemap>>(textureUnit, cubemap->getName(), cubemap));
			++textureUnit;
		});
}

inline void JFF::MaterialGL::extractPostProcessingTextures()
{
	switch (domain)
	{
	case JFF::Material::MaterialDomain::POST_PROCESS:
	case JFF::Material::MaterialDomain::UI:
		{
			auto ppTex0 = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_OUTPUT_COLOR, 
				Framebuffer::AttachmentPoint::COLOR_0, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTex0);
			++textureUnit;

			// TODO: Add here more G-buffer textures
		}
		break;
	case JFF::Material::MaterialDomain::GAUSSIAN_BLUR_HORIZONTAL:
	case JFF::Material::MaterialDomain::GAUSSIAN_BLUR_VERTICAL:
	case JFF::Material::MaterialDomain::HIGH_PASS_FILTER:
	case JFF::Material::MaterialDomain::COLOR_COPY:
	case JFF::Material::MaterialDomain::RENDER_TO_SCREEN:
		{
			auto ppTex = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_OUTPUT_COLOR, 
				Framebuffer::AttachmentPoint::COLOR_0, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTex);
			++textureUnit;
		}
		break;
	case JFF::Material::MaterialDomain::SSAO:
		{
			// TODO: This is only valid in deferred shading #################################################################

			auto ppTexWorldPositions = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_FRAGMENT_WORLD_POS,
				Framebuffer::AttachmentPoint::COLOR_0, /* Using Framebuffer 1 */ 1);
			postProcessingTextures.push_back(ppTexWorldPositions);
			++textureUnit;

			auto ppTexWorldNormals = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_NORMAL_WORLD_DIR,
				Framebuffer::AttachmentPoint::COLOR_1, /* Using Framebuffer 1 */ 1);
			postProcessingTextures.push_back(ppTexWorldNormals);
			++textureUnit;
		}
		break;
	case JFF::Material::MaterialDomain::COLOR_ADDITION:
		{
			auto ppTex0 = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_OUTPUT_COLOR,
				Framebuffer::AttachmentPoint::COLOR_0, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTex0);
			++textureUnit;

			auto ppTex1 = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_OUTPUT_COLOR_2,
				Framebuffer::AttachmentPoint::COLOR_0, /* Using Framebuffer 1 */ 1);
			postProcessingTextures.push_back(ppTex1);
			++textureUnit;
		}
		break;
	case JFF::Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED:
	case JFF::Material::MaterialDomain::POINT_LIGHTING_DEFERRED:
	case JFF::Material::MaterialDomain::SPOT_LIGHTING_DEFERRED:
	case JFF::Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED:
	case JFF::Material::MaterialDomain::EMISSIVE_LIGHTING_DEFERRED:
		{
			auto ppTexWorldPositions = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_FRAGMENT_WORLD_POS,
				Framebuffer::AttachmentPoint::COLOR_0, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTexWorldPositions);
			++textureUnit;

			auto ppTexWorldNormals = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_NORMAL_WORLD_DIR,
				Framebuffer::AttachmentPoint::COLOR_1, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTexWorldNormals);
			++textureUnit;

			auto ppTexAlbedoSpecular = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_ALBEDO_SPECULAR,
				Framebuffer::AttachmentPoint::COLOR_2, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTexAlbedoSpecular);
			++textureUnit;

			auto ppTexAmbientShininess = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_AMBIENT_SHININESS,
				Framebuffer::AttachmentPoint::COLOR_3, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTexAmbientShininess);
			++textureUnit;

			auto ppTexReflection = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_REFLECTION,
				Framebuffer::AttachmentPoint::COLOR_4, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTexReflection);
			++textureUnit;

			auto ppTexEmissive = std::tuple<int, std::string, Framebuffer::AttachmentPoint, int>(
				textureUnit, ShaderCodeBuilder::POST_PROCESSING_EMISSIVE,
				Framebuffer::AttachmentPoint::COLOR_5, /* Using Framebuffer 0 */ 0);
			postProcessingTextures.push_back(ppTexEmissive);
			++textureUnit;

			// TODO: Add here more G-buffer textures
		}
		break;
	default:
		// Don't add post processing textures here
		break;
	}
}

inline void JFF::MaterialGL::extractEnvironmentMaps()
{
	switch (domain)
	{
	case JFF::Material::MaterialDomain::SURFACE:
	case JFF::Material::MaterialDomain::TRANSLUCENT:
		switch (lightModel)
		{
		case JFF::Material::LightModel::PHONG:
		case JFF::Material::LightModel::BLINN_PHONG:
			environmentMaps.push_back(std::tuple<int, std::string>(textureUnit, ShaderCodeBuilder::ENVIRONMENT_MAP));
			++textureUnit;
			break;
		case JFF::Material::LightModel::PBR:
			environmentMaps.push_back(std::tuple<int, std::string>(textureUnit, ShaderCodeBuilder::IRRADIANCE_MAP));
			++textureUnit;
			environmentMaps.push_back(std::tuple<int, std::string>(textureUnit, ShaderCodeBuilder::PRE_FILTERED_MAP));
			++textureUnit;
			environmentMaps.push_back(std::tuple<int, std::string>(textureUnit, ShaderCodeBuilder::BRDF_INTEGRATION_MAP));
			++textureUnit;
			break;
		case JFF::Material::LightModel::GOURAUD:
		case JFF::Material::LightModel::UNLIT:
		default:
			break;
		}
		break;
	case JFF::Material::MaterialDomain::ENVIRONMENT_LIGHTING_DEFERRED:
	case JFF::Material::MaterialDomain::IRRADIANCE_GENERATOR:
	case JFF::Material::MaterialDomain::PRE_FILTERED_ENVIRONMENT_MAP_GENERATOR:
		environmentMaps.push_back(std::tuple<int, std::string>(textureUnit, ShaderCodeBuilder::ENVIRONMENT_MAP));
		++textureUnit;
		break;
	default:
		break;
	}
}

inline void JFF::MaterialGL::extractShadowMaps()
{
	switch (domain)
	{
	case JFF::Material::MaterialDomain::SURFACE:
	case JFF::Material::MaterialDomain::TRANSLUCENT:
		switch (lightModel)
		{
		case JFF::Material::LightModel::GOURAUD:
		case JFF::Material::LightModel::PHONG:
		case JFF::Material::LightModel::BLINN_PHONG:
		case JFF::Material::LightModel::PBR:
			for (int i = 0; i < engine->renderer.lock()->getForwardShadingMaxDirectionalLights(); ++i)
			{
				std::string emptyString;
				std::ostringstream ss(emptyString);
				ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT_ARRAY << "[" << i << "]." << ShaderCodeBuilder::DIR_LIGHT_SHADOW_MAP;

				auto shadowTuple = std::tuple<int, std::string, Framebuffer::AttachmentPoint>(textureUnit, ss.str(), Framebuffer::AttachmentPoint::DEPTH);
				directionalLightShadowMaps.push_back(shadowTuple);
				++textureUnit;
			}

			for (int i = 0; i < engine->renderer.lock()->getForwardShadingMaxPointLights(); ++i)
			{
				std::string emptyString;
				std::ostringstream ss(emptyString);
				ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT_ARRAY << "[" << i << "]." << ShaderCodeBuilder::POINT_LIGHT_SHADOW_MAP;

				auto shadowTuple = std::tuple<int, std::string, Framebuffer::AttachmentPoint>(textureUnit, ss.str(), Framebuffer::AttachmentPoint::DEPTH);
				pointLightShadowCubemaps.push_back(shadowTuple);
				++textureUnit;
			}

			for (int i = 0; i < engine->renderer.lock()->getForwardShadingMaxSpotLights(); ++i)
			{
				std::string emptyString;
				std::ostringstream ss(emptyString);
				ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT_ARRAY << "[" << i << "]." << ShaderCodeBuilder::SPOT_LIGHT_SHADOW_MAP;

				auto shadowTuple = std::tuple<int, std::string, Framebuffer::AttachmentPoint>(textureUnit, ss.str(), Framebuffer::AttachmentPoint::DEPTH);
				spotLightShadowMaps.push_back(shadowTuple);
				++textureUnit;
			}
			break;
		case JFF::Material::LightModel::UNLIT:
		default:
			break;
		}
		break;
	case JFF::Material::MaterialDomain::DIRECTIONAL_LIGHTING_DEFERRED:
		{
			std::string emptyString;
			std::ostringstream ss(emptyString);
			ss << ShaderCodeBuilder::DIRECTIONAL_LIGHT_STRUCT << "." << ShaderCodeBuilder::DIR_LIGHT_SHADOW_MAP;

			auto shadowTuple = std::tuple<int, std::string, Framebuffer::AttachmentPoint>(textureUnit, ss.str(), Framebuffer::AttachmentPoint::DEPTH);
			directionalLightShadowMaps.push_back(shadowTuple);
			++textureUnit;
		}
		break;
	case JFF::Material::MaterialDomain::POINT_LIGHTING_DEFERRED:
		{
			std::string emptyString;
			std::ostringstream ss(emptyString);
			ss << ShaderCodeBuilder::POINT_LIGHT_STRUCT << "." << ShaderCodeBuilder::POINT_LIGHT_SHADOW_MAP;

			auto shadowTuple = std::tuple<int, std::string, Framebuffer::AttachmentPoint>(textureUnit, ss.str(), Framebuffer::AttachmentPoint::DEPTH);
			pointLightShadowCubemaps.push_back(shadowTuple);
			++textureUnit;
		}
		break;
	case JFF::Material::MaterialDomain::SPOT_LIGHTING_DEFERRED:
		{
			std::string emptyString;
			std::ostringstream ss(emptyString);
			ss << ShaderCodeBuilder::SPOT_LIGHT_STRUCT << "." << ShaderCodeBuilder::SPOT_LIGHT_SHADOW_MAP;

			auto shadowTuple = std::tuple<int, std::string, Framebuffer::AttachmentPoint>(textureUnit, ss.str(), Framebuffer::AttachmentPoint::DEPTH);
			spotLightShadowMaps.push_back(shadowTuple);
			++textureUnit;
		}
		break;
	default:
		break;
	}
}

inline void JFF::MaterialGL::extractCustomCodeFromFile(const std::shared_ptr<INIFile>& iniFile)
{
	// NOTE: a function called 'material' must be present
	iniFile->visitKeyValuePairs("material", [this](const std::pair<std::string, std::string>& pair)
		{
			customCode << pair.second;
		});

	/* 
	* Add an additional material function called materialOverrides() functionand leave it empty.
	* The functionality of this function is reserved for materials built from model loader.
	* To know more about this, check IO::loadModel()
	*/
	customCode << ShaderCodeBuilder::MATERIAL_OVERRIDES_EMPTY_FUNCTION;
}