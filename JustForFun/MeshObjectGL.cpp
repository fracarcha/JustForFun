/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "MeshObjectGL.h"

#include "Log.h"
#include <algorithm>

JFF::MeshObjectGL::MeshObjectGL(JFF::Engine* const engine, const std::shared_ptr<Mesh>& mesh) :
	engine(engine),
	mesh(mesh),
	vao(0u),
	drawData()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor MeshObjectGL")
}

JFF::MeshObjectGL::MeshObjectGL(JFF::Engine* const engine, const BasicMesh& predefinedShape) :
	engine(engine),
	mesh(),
	vao(0u),
	drawData()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor MeshObjectGL")

	switch (predefinedShape)
	{
	case BasicMesh::CUBE:
		mesh = std::make_shared<MeshCube>();
		break;
	case BasicMesh::SPHERE:
		mesh = std::make_shared<MeshSphere>(engine);
		break;
	case BasicMesh::PLANE:
		mesh = std::make_shared<MeshPlane>();
		break;
	}
}

JFF::MeshObjectGL::~MeshObjectGL()
{
	JFF_LOG_INFO_LOW_PRIORITY("Dtor MeshObjectGL")

	// Destroy VAO and free VRAM memory
	glDeleteVertexArrays(1, &vao);
}

void JFF::MeshObjectGL::cook()
{
	// TODO: Ensure mesh integrity (vector 'vertices' cannot be empty)

	// Generate vbo & ebo
	GLuint vbo = genVBO();
	GLuint ebo = genEBO(); // If this mesh doesn't use indices, this function returns 0

	/* Generate VAO(Vertex Array Object). Generating VAO is mandatory in OpenGL Core profile. It stores:
	*  * Calls to glEnableVertexAttribArray()
	*  * Vertex attribute configurations (glVertexAttribPointer())
	*  * Vertex buffer associated to vertex attributes by calls  to glVertexAttribPointer()
	*/
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Bind buffers used in vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // If ebo == 0, this function does nothing interesting

	// Configure the attributes that will appear in vertex shader and connects its data with created buffers
	setVertexPointers();

	// Unbind vao
	glBindVertexArray(0); // Calls glDisableVertexAttribArray(0) internally

	// vbo and ebo are already attached to vao, so there's no need to keep them in memory (NOTE: They will still in memory until VAO is deleted)
	glDeleteBuffers(1, &vbo); // Calls glBindBuffer(GL_ARRAY_BUFFER, 0) internally
	glDeleteBuffers(1, &ebo); // Calls glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) internally. If ebo == 0, this function silently ignores it

	// Fill drawData info before mesh deletion
	configureDrawData();

	// Remove vertex data from CPU
	mesh.reset();
}

void JFF::MeshObjectGL::draw()
{
	// Use vertex data stored in Vertex Array Object
	glBindVertexArray(vao);

	// Extract draw data and use it to select the best way to draw this renderable
	if (drawData.isIndexed)
	{
		std::for_each(drawData.indexedPrimitiveAssemblyMethod.begin(), drawData.indexedPrimitiveAssemblyMethod.end(),
			[this](const auto& pair)
			{
				glDrawElements(
					translatePrimitiveMethodToOpenGL(pair.first),
					pair.second.first,
					GL_UNSIGNED_INT,
					(void*)pair.second.second);
			});
	}
	else
	{
		glDrawArrays(translatePrimitiveMethodToOpenGL(drawData.notIndexedPrimitiveAssemblyMethod), 0, drawData.numVertices);
	}
}

inline GLuint JFF::MeshObjectGL::genVBO()
{
	// Vertex buffer object
	GLuint vbo;

	// Create a new pointer to GPU memory
	glGenBuffers(1, &vbo);

	// Use vbo as active buffer using vertex array target (GL_ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Create and fill the GPU buffer with vertex data
	if (mesh->isDataCollapsed) // If data is collapsed, vertices, normals and uv are contained in one single vector
	{
		// Allocates GPU memory and fill the buffer with vertex, normal and uv data
		glBufferData(GL_ARRAY_BUFFER, mesh->verticesSize * sizeof(float), mesh->vertices, GL_STATIC_DRAW);
	}
	else // If data isn't collapsed, vertices, normals, uv, etc. are contained in separate vectors
	{
		// Get vertex info
		GLsizeiptr vertexSizeBytes = mesh->verticesSize * sizeof(float);
		GLsizeiptr normalSizeBytes = mesh->useNormals ? mesh->normalsSize * sizeof(float) : 0ll;
		GLsizeiptr tangentSizeBytes = mesh->useTangents ? mesh->tangentsSize * sizeof(float) : 0ll;
		GLsizeiptr bitangentSizeBytes = mesh->useBitangents ? mesh->bitangentsSize * sizeof(float) : 0ll;
		GLsizeiptr uvSizeBytes = mesh->useUV ? mesh->uvSize * sizeof(float) : 0ll;

		GLintptr vertexOffsetBytes = 0;
		GLintptr normalOffsetBytes = vertexSizeBytes;
		GLintptr tangentOffsetBytes = vertexSizeBytes + normalSizeBytes;
		GLintptr bitangentOffsetBytes = vertexSizeBytes + normalSizeBytes + tangentSizeBytes;
		GLintptr uvOffsetBytes = vertexSizeBytes + normalSizeBytes + tangentSizeBytes + bitangentSizeBytes;

		// Allocates GPU memory for all vectors, but keep allocated memory empty
		glBufferData(GL_ARRAY_BUFFER, vertexSizeBytes + normalSizeBytes + tangentSizeBytes + bitangentSizeBytes + uvSizeBytes, NULL, GL_STATIC_DRAW);

		// Fill the buffer using vertex, normal and uv data and pack it contiguously
		glBufferSubData(GL_ARRAY_BUFFER, vertexOffsetBytes, vertexSizeBytes, mesh->vertices);

		if (mesh->useNormals)
			glBufferSubData(GL_ARRAY_BUFFER, normalOffsetBytes, normalSizeBytes, mesh->normals);

		if (mesh->useTangents)
			glBufferSubData(GL_ARRAY_BUFFER, tangentOffsetBytes, tangentSizeBytes, mesh->tangents);

		if (mesh->useBitangents)
			glBufferSubData(GL_ARRAY_BUFFER, bitangentOffsetBytes, bitangentSizeBytes, mesh->bitangents);

		if (mesh->useUV)
			glBufferSubData(GL_ARRAY_BUFFER, uvOffsetBytes, uvSizeBytes, mesh->uv);
	}

	// Unbind vbo from vertex array target
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vbo;
}

inline GLuint JFF::MeshObjectGL::genEBO()
{
	// If the mesh doesn't use indices, return
	if (!mesh->useFaces)
		return 0u;

	// Element buffer object (indices)
	GLuint ebo;

	// Create a new pointer to GPU memory
	glGenBuffers(1, &ebo);

	// Use ebo as active buffer using vertex array indices target (GL_ELEMENT_ARRAY_BUFFER)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// Get indexes info
	GLsizeiptr facesSizeBytes = 0;
	std::for_each(mesh->faces.begin(), mesh->faces.end(), [&facesSizeBytes](const auto& pair)
		{
			facesSizeBytes += pair.second.size();
		});
	facesSizeBytes *= sizeof(unsigned int);

	// Allocates GPU memory for all indices, but keep allocated memory empty
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesSizeBytes, NULL, GL_STATIC_DRAW);

	// Fill the buffer using faces data and pack it contiguously
	GLintptr offsetAccumBytes = 0;
	std::for_each(mesh->faces.begin(), mesh->faces.end(), [&offsetAccumBytes](const auto& pair)
		{
			GLsizeiptr facesSizeBytes = pair.second.size() * sizeof(unsigned int);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetAccumBytes, facesSizeBytes, pair.second.data());
			offsetAccumBytes += facesSizeBytes;
		});

	// Unbind ebo from vertex array indices target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return ebo;
}

inline void JFF::MeshObjectGL::setVertexPointers()
{
	// layout (location = 0) in vec3 vertexPosModelSpace;
	GLuint vertexLocation = 0;
	GLint vertexNumComponents = mesh->componentsPerVertex; // Num components per vertex attribute in shader (vec3)
	GLenum vertexType = GL_FLOAT; // Because vec3 has 3 floats
	GLboolean normalizeVertex = GL_FALSE;

	// layout (location = 1) in vec3 normalModelSpace;
	GLuint normalLocation = 1;
	GLint normalNumComponents = mesh->useNormals ? mesh->componentsPerNormal : 0; // Num components per normal attribute in shader (vec3)
	GLenum normalType = GL_FLOAT; // Because vec3 has 3 floats
	GLboolean normalizeNormals = GL_FALSE;

	// layout(location = 2) in vec3 tangentModelSpace;
	GLuint tangentLocation = 2;
	GLint tangentNumComponents = mesh->useTangents ? mesh->componentsPerTangent : 0; // Num components per tangent attribute in shader (vec3)
	GLenum tangentType = GL_FLOAT; // Because vec3 has 3 floats
	GLboolean normalizeTangents = GL_FALSE;

	// layout(location = 3) in vec3 bitangentModelSpace;
	GLuint bitangentLocation = 3;
	GLint bitangentNumComponents = mesh->useBitangents ? mesh->componentsPerBitangent : 0; // Num components per bitangent attribute in shader (vec3)
	GLenum bitangentType = GL_FLOAT; // Because vec3 has 3 floats
	GLboolean normalizeBitangents = GL_FALSE;

	// layout (location = 4) in vec3 uvModelSpace;
	GLuint uvLocation = 4;
	GLint uvNumComponents = mesh->useUV ? mesh->componentsPerUV : 0; // Num components per uv attribute in shader (vec3)
	GLenum uvType = GL_FLOAT; // Because vec3 has 3 floats
	GLboolean normalizeUV = GL_FALSE;

	/*
	* Depending on the way vertex data is collapsed or not, the way to point to buffer data is different.
	*	* If data is collapsed, each vertex has a position, normal and uv contiguous in memory.
	*	* If data isn't collapsed, all vertices are contiguous in memory; after that, all normals and so on.
	*/
	if (mesh->isDataCollapsed)
	{
		// Define a stride, which is the offset between consecutive vertex attributes (how many bytes should advance to find the next 'normal', for example)
		GLsizei strideBytes = (vertexNumComponents + normalNumComponents + tangentNumComponents + bitangentNumComponents + uvNumComponents) * sizeof(float);

		// Define offsets for each vertex attribute
#pragma warning (disable:26451) // Ignore overflow warning on next lines (overflow will never happen)
		void* vertexOffsetBytes = (void*)0;
		void* normalOffsetBytes = (void*)(sizeof(float) * vertexNumComponents);
		void* tangentOffsetBytes = (void*)(sizeof(float) * (vertexNumComponents + normalNumComponents));
		void* bitangentOffsetBytes = (void*)(sizeof(float) * (vertexNumComponents + normalNumComponents + tangentNumComponents));
		void* uvOffsetBytes = (void*)(sizeof(float) * (vertexNumComponents + normalNumComponents + tangentNumComponents + bitangentNumComponents));
#pragma warning (default:26451)

		// Define pointers and enable the vertex shader attribute. With these functions, shader variables will receive the values stored in vbo
		glVertexAttribPointer(vertexLocation, vertexNumComponents, vertexType, normalizeVertex, strideBytes, vertexOffsetBytes);
		glEnableVertexAttribArray(vertexLocation);

		if (mesh->useNormals)
		{
			glVertexAttribPointer(normalLocation, normalNumComponents, normalType, normalizeNormals, strideBytes, normalOffsetBytes);
			glEnableVertexAttribArray(normalLocation);
		}
		if (mesh->useTangents)
		{
			glVertexAttribPointer(tangentLocation, tangentNumComponents, tangentType, normalizeTangents, strideBytes, tangentOffsetBytes);
			glEnableVertexAttribArray(tangentLocation);
		}
		if (mesh->useBitangents)
		{
			glVertexAttribPointer(bitangentLocation, bitangentNumComponents, bitangentType, normalizeBitangents, strideBytes, bitangentOffsetBytes);
			glEnableVertexAttribArray(bitangentLocation);
		}
		if (mesh->useUV)
		{
			glVertexAttribPointer(uvLocation, uvNumComponents, uvType, normalizeUV, strideBytes, uvOffsetBytes);
			glEnableVertexAttribArray(uvLocation);
		}
	}
	else
	{
		// Define a stride, which is the offset between consecutive vertex attributes (how many bytes should advance to find the next 'normal', for example)
		GLsizei strideBytes = 0;

		// Gets the size of each vertex component
		size_t numVertices = mesh->verticesSize;
		size_t numNormals = mesh->useNormals ? mesh->normalsSize : 0ull;
		size_t numTangents = mesh->useTangents ? mesh->tangentsSize : 0ull;
		size_t numBitangents = mesh->useBitangents ? mesh->bitangentsSize : 0ull;

		// Define offsets for each vertex attribute
		void* vertexOffsetBytes = (void*)0;
		void* normalOffsetBytes = (void*)(numVertices * sizeof(float));
		void* tangentOffsetBytes = (void*)((numVertices + numNormals) * sizeof(float));
		void* bitangentOffsetBytes = (void*)((numVertices + numNormals + numTangents) * sizeof(float));
		void* uvOffsetBytes = (void*)((numVertices + numNormals + numTangents + numBitangents) * sizeof(float));

		// Define pointers and enable the vertex shader attribute. With these functions, shader variables will receive the values stored in vbo
		glVertexAttribPointer(vertexLocation, vertexNumComponents, vertexType, normalizeVertex, strideBytes, vertexOffsetBytes);
		glEnableVertexAttribArray(vertexLocation);

		if (mesh->useNormals)
		{
			glVertexAttribPointer(normalLocation, normalNumComponents, normalType, normalizeNormals, strideBytes, normalOffsetBytes);
			glEnableVertexAttribArray(normalLocation);
		}
		if (mesh->useTangents)
		{
			glVertexAttribPointer(tangentLocation, tangentNumComponents, tangentType, normalizeTangents, strideBytes, tangentOffsetBytes);
			glEnableVertexAttribArray(tangentLocation);
		}
		if (mesh->useBitangents)
		{
			glVertexAttribPointer(bitangentLocation, bitangentNumComponents, bitangentType, normalizeBitangents, strideBytes, bitangentOffsetBytes);
			glEnableVertexAttribArray(bitangentLocation);
		}
		if (mesh->useUV)
		{
			glVertexAttribPointer(uvLocation, uvNumComponents, uvType, normalizeUV, strideBytes, uvOffsetBytes);
			glEnableVertexAttribArray(uvLocation);
		}
	}
}

inline void JFF::MeshObjectGL::configureDrawData()
{
	short componentsPerNormal = mesh->useNormals ? mesh->componentsPerNormal : 0;
	short componentsPerTangent = mesh->useTangents ? mesh->componentsPerTangent : 0;
	short componentsPerBitangent = mesh->useBitangents ? mesh->componentsPerBitangent : 0;
	short componentsPerUV = mesh->useUV ? mesh->componentsPerUV : 0;

	short componentsPerVertex = mesh->componentsPerVertex;
	if (mesh->isDataCollapsed)
		componentsPerVertex += componentsPerNormal + componentsPerTangent + componentsPerBitangent + componentsPerUV;

	drawData.numVertices = static_cast<int>(mesh->verticesSize) / componentsPerVertex;
	drawData.isIndexed = mesh->useFaces;
	drawData.notIndexedPrimitiveAssemblyMethod = mesh->primitiveAssemblyMethod;

	size_t indexOffset = 0;
	std::for_each(mesh->faces.begin(), mesh->faces.end(), [this, &indexOffset](const auto& pair)
		{
			auto indicesLocation = std::pair<int, size_t>(static_cast<int>(pair.second.size()), indexOffset);
			auto entry = std::pair<Mesh::PrimitiveAssemblyMethod, std::pair<int, size_t>>(pair.first, indicesLocation);
			drawData.indexedPrimitiveAssemblyMethod.insert(entry);
			indexOffset += pair.second.size() * sizeof(unsigned int);
		});
}

inline GLenum JFF::MeshObjectGL::translatePrimitiveMethodToOpenGL(const Mesh::PrimitiveAssemblyMethod& assemblyMethod) const
{
	switch (assemblyMethod)
	{
	case Mesh::PrimitiveAssemblyMethod::TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	case Mesh::PrimitiveAssemblyMethod::TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;
	case Mesh::PrimitiveAssemblyMethod::TRIANGLES:
	default:
		return GL_TRIANGLES;
	}
}
