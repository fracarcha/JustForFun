#pragma once

#include "MeshComponent.h"

#define GLEW_STATIC // Used when linked against GLEW static library
#include "GL/glew.h"

#include <memory>

namespace JFF
{
	class MeshComponentGL : public MeshComponent
	{
	public:
		// Ctor & Dtor
		MeshComponentGL(GameObject* const gameObject, const char* name, bool initiallyEnabled, const std::shared_ptr<Mesh>& mesh);
		MeshComponentGL(GameObject* const gameObject, const char* name, bool initiallyEnabled, const BasicMesh& predefinedShape);
		virtual ~MeshComponentGL();

		// Copy ctor and copy assignment
		MeshComponentGL(const MeshComponentGL& other) = delete;
		MeshComponentGL& operator=(const MeshComponentGL& other) = delete;

		// Move ctor and assignment
		MeshComponentGL(MeshComponentGL&& other) = delete;
		MeshComponentGL operator=(MeshComponentGL&& other) = delete;

		// ----------------------------- COMPONENT OVERRIDES ----------------------------- //

		virtual void onStart() override;
		virtual void onDestroy() noexcept override;

		// ----------------------------- MESH FUNCTIONS ----------------------------- //

		// Enables the GPU buffer where the vertex data of this mesh is stored and execute a draw call
		virtual void draw() override;

	private: // Helper functions
		inline GLuint genVBO();
		inline GLuint genEBO();
		inline void setVertexPointers();
		inline void configureDrawData();

		inline GLenum translatePrimitiveMethodToOpenGL(const Mesh::PrimitiveAssemblyMethod& assemblyMethod) const;

	protected:
		std::shared_ptr<Mesh> mesh;

		GLuint vao;
		DrawData drawData;
	};
}