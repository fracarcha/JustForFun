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

#include "Mesh.h"
#include <memory>

namespace JFF
{
	class Engine;

	// Representation of a mesh in a graphics API
	class MeshObject
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
		explicit MeshObject() {}
		virtual ~MeshObject() {}

		// Copy ctor and copy assignment
		MeshObject(const MeshObject& other) = delete;
		MeshObject& operator=(const MeshObject& other) = delete;

		// Move ctor and assignment
		MeshObject(MeshObject&& other) = delete;
		MeshObject operator=(MeshObject&& other) = delete;

		// ----------------------------- MESH OBJECT FUNCTIONS ----------------------------- //

		// Build the mesh and store it in VRAM using graphics API
		virtual void cook() = 0;

		// Enables the GPU buffer where the vertex data of this mesh is stored and execute a draw call
		virtual void draw() = 0;
	};
}