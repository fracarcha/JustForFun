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

#include "MeshObject.h"

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

namespace JFF
{
	class MeshObjectGL : public MeshObject
	{
	public:
		// Ctor & Dtor
		explicit MeshObjectGL(JFF::Engine* const engine, const std::shared_ptr<Mesh>& mesh);
		explicit MeshObjectGL(JFF::Engine* const engine, const BasicMesh& predefinedShape);
		virtual ~MeshObjectGL();

		// Copy ctor and copy assignment
		MeshObjectGL(const MeshObjectGL& other) = delete;
		MeshObjectGL& operator=(const MeshObjectGL& other) = delete;

		// Move ctor and assignment
		MeshObjectGL(MeshObjectGL&& other) = delete;
		MeshObjectGL operator=(MeshObjectGL&& other) = delete;

		// ----------------------------- MESH OBJECT FUNCTIONS ----------------------------- //

		// Build the mesh and store it in VRAM using graphics API
		virtual void cook() override;

		// Enables the GPU buffer where the vertex data of this mesh is stored and execute a draw call
		virtual void draw() override;

	private: // Helper functions
		inline GLuint genVBO();
		inline GLuint genEBO();
		inline void setVertexPointers();
		inline void configureDrawData();

		inline GLenum translatePrimitiveMethodToOpenGL(const Mesh::PrimitiveAssemblyMethod& assemblyMethod) const;

	protected:
		Engine* engine;

		std::shared_ptr<Mesh> mesh;

		GLuint vao;
		DrawData drawData;
	};
}