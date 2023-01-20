/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "CameraComponentGL.h"

#include "Log.h"
#include "Engine.h"

JFF::CameraComponentGL::CameraComponentGL(GameObject* const gameObject, const char* name, bool initiallyEnabled, bool activeCameraOnStart) :
	CameraComponent(gameObject, name, initiallyEnabled),
	activeCameraOnStart(activeCameraOnStart),
	projectionMatrix(),
	viewMatrix(),
	ubo(0u),

	dirtyProjectionMatrix(true),

	viewMatrixOffset(0),
	projectionMatrixOffset(0),
	camWorldPosOffset(0)
{
	JFF_LOG_INFO("Ctor CameraComponentGL")
}

JFF::CameraComponentGL::~CameraComponentGL()
{
	JFF_LOG_INFO("Dtor CameraComponentGL")
}

void JFF::CameraComponentGL::onStart()
{
	// Create UBO to store camera uniforms in VRAM
	createUBO();

	// Subscribe as active camera
	if (activeCameraOnStart)
		setActiveCamera();
}

void JFF::CameraComponentGL::onUpdate()
{
	generateViewMatrix();

	// ----------------------------------- Update UBO data ----------------------------------- //

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);

	GLsizeiptr sizeMat4 = 64; // 4 * 4 matrix * 4 bytes per float
	GLsizeiptr sizeVec3 = 12; // 3 float * 4 bytes per float

	glBufferSubData(GL_UNIFORM_BUFFER, viewMatrixOffset, sizeMat4, *viewMatrix);

	Vec3 camWorldPos = gameObject->transform.getWorldPos();
	glBufferSubData(GL_UNIFORM_BUFFER, camWorldPosOffset, sizeVec3, *camWorldPos);

	if (dirtyProjectionMatrix)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, projectionMatrixOffset, sizeMat4, *projectionMatrix);
		dirtyProjectionMatrix = false;
	}
}

void JFF::CameraComponentGL::onDestroy() noexcept
{
	// Delete UBO
	glDeleteBuffers(1, &ubo);

	// Unsubscribe if this camera was active
	auto cameraManager = gameObject->engine->camera.lock();
	if (cameraManager->isCameraActive(this))
		cameraManager->resetActiveCamera();
}

void JFF::CameraComponentGL::setActiveCamera()
{
	gameObject->engine->camera.lock()->setActiveCamera(this);

	// Bind this buffer to its binding point. This uniform block will use binding point 0
	GLuint cameraParamsBindingPoint = 0u; // Check MaterialGL to ensure shaders use the same binding point for camera params
	glBindBufferBase(GL_UNIFORM_BUFFER, cameraParamsBindingPoint, ubo);
}

void JFF::CameraComponentGL::setOrthographicProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	projectionMatrix = gameObject->engine->math.lock()->ortho(left, right, bottom, top, zNear, zFar);
	dirtyProjectionMatrix = true;
}

void JFF::CameraComponentGL::setPerspectiveProjection(float FOVDeg, float aspectRatio, float zNear, float zFar)
{
	auto math = gameObject->engine->math.lock();
	projectionMatrix = math->perspective(math->radians(FOVDeg), aspectRatio, zNear, zFar);
	dirtyProjectionMatrix = true;
}

JFF::Mat4 JFF::CameraComponentGL::getViewMatrix() const
{
	return viewMatrix;
}

JFF::Mat4 JFF::CameraComponentGL::getProjectionMatrix() const
{
	return projectionMatrix;
}

// ------------------------ HELPER FUNCTIONS ------------------------ //

inline void JFF::CameraComponentGL::createUBO()
{
	/*
	* This buffer is used to avoid passing the same uniform multiple times to many shaders. A perfect example of this
	* are view and projection matrices. This buffer is read automatically by the shader once configured.
	*/

	// ----------------------------------- UNIFORM BLOCK DATA ----------------------------------- //
	
	// Use uniform block for uniforms that doesn't change between programs
	/* More info :	https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
					https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object#:~:text=A%20Buffer%20Object%20that%20is,for%20the%20same%20program%20object.
					https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Uniform_blocks
	*/

	/*
	layout(std140) uniform CameraParams
	{
											// Base alignment (BA) (bytes)			// Alignment offset (AO) (bytes)
		mat4 viewMatrix;					// 16 (size of one vec4) (1st column)	// 0 (First offset is zero)
											// 16 (2nd column)						// 16 (This is the first multiple of BA_current of the result of AO_prev + BA_prev)
											// 16 (3rd column)						// 32
											// 16 (4th column)						// 48
		mat4 projectionMatrix;				// 16 (1st column)						// 64						
											// 16 (2nd column)						// 80
											// 16 (3rd column)						// 96
											// 16 (4th column)						// 112
		vec3 cameraPosWorldSpace;			// 16 (vec3 is treated as vec4)			// 128
																					// TOTAL: 144 bytes (AO_last + BA_last)
	};
	*/
	GLsizeiptr UBOSizeBytes = 144;
	viewMatrixOffset = 0;
	projectionMatrixOffset = 64;
	camWorldPosOffset = 128;

	// Generate a Uniform Buffer Object
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, UBOSizeBytes, NULL, GL_STATIC_DRAW); // Reserve memory but not fill it
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

inline void JFF::CameraComponentGL::generateViewMatrix()
{
	// Get world position of this component
	Vec3 eye = gameObject->transform.getWorldPos();

	// Get world rotation 
	Vec4 gazeDir4 = gameObject->transform.getRotationMatrix() * Vec4::FORWARD;
	Vec3 gazeDir(gazeDir4.pitch, gazeDir4.yaw, gazeDir4.roll);

	viewMatrix = gameObject->engine->math.lock()->lookAt(eye, eye + gazeDir, Vec3::UP);
}
