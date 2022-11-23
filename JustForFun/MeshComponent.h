#pragma once

#include "Component.h"
#include "Mesh.h"

namespace JFF
{
	class MeshComponent : public Component
	{
	public:
		enum class BasicMesh
		{
			CUBE,
			SPHERE,
			PLANE,
			// TODO: More basic shapes here
		};

		// This struct holds info needed when calling glDrawArrays or glDrawElements
		struct DrawData
		{
			DrawData() :
				numVertices(0),
				isIndexed(false),
				notIndexedPrimitiveAssemblyMethod(Mesh::PrimitiveAssemblyMethod::TRIANGLES),
				indexedPrimitiveAssemblyMethod()
			{
			}

			int numVertices;
			bool isIndexed;

			Mesh::PrimitiveAssemblyMethod notIndexedPrimitiveAssemblyMethod;

			// Pair first: numIndices | Pair second: starting offset in Bytes. Allow multiple keys
			std::multimap<Mesh::PrimitiveAssemblyMethod, std::pair<int, size_t>> indexedPrimitiveAssemblyMethod;
		};

		// Ctor & Dtor
		MeshComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
			Component(gameObject, name, initiallyEnabled)
		{}
		virtual ~MeshComponent() {}

		// Copy ctor and copy assignment
		MeshComponent(const MeshComponent& other) = delete;
		MeshComponent& operator=(const MeshComponent& other) = delete;

		// Move ctor and assignment
		MeshComponent(MeshComponent&& other) = delete;
		MeshComponent operator=(MeshComponent&& other) = delete;

		// ----------------------------- MESH FUNCTIONS ----------------------------- //
		
		// Enables the GPU buffer where the vertex data of this mesh is stored and execute a draw call
		virtual void draw() = 0;
	};
}