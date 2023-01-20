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
#include <map>

namespace JFF
{
	class Engine;

	struct Mesh
	{
		enum class PrimitiveAssemblyMethod : char
		{
			TRIANGLES,
			TRIANGLE_STRIP,
			TRIANGLE_FAN
		};

		Mesh() :
			vertices(nullptr),
			normals(nullptr),
			tangents(nullptr),
			bitangents(nullptr),
			uv(nullptr),

			verticesSize(0ll),
			normalsSize(0ll),
			tangentsSize(0ll),
			bitangentsSize(0ll),
			uvSize(0ll),

			faces(),

			isDataCollapsed(false),

			componentsPerVertex(3),		// in vec3 vertexPosModelSpace
			componentsPerNormal(3),		// in vec3 normalModelSpace
			componentsPerTangent(3),	// in vec3 tangentModelSpace
			componentsPerBitangent(3),	// in vec3 bitangentModelSpace
			componentsPerUV(3),			// in vec3 uvModelSpace (Used vec3 to be compatible with 3D model loaders)

			useNormals(true),
			useTangents(true),
			useBitangents(true),
			useUV(true),
			useFaces(false),

			primitiveAssemblyMethod(PrimitiveAssemblyMethod::TRIANGLES)
		{}
		virtual ~Mesh() 
		{
			free();
		}

		void reserve(size_t numVertices)
		{
			if (vertices != nullptr) // Free previously allocated memory
				free();

			if (isDataCollapsed)
			{
				short totalComponents = componentsPerVertex;
				totalComponents += useNormals ? componentsPerNormal : 0;
				totalComponents += useTangents ? componentsPerTangent : 0;
				totalComponents += useBitangents ? componentsPerBitangent : 0;
				totalComponents += useUV ? componentsPerUV : 0;

				verticesSize = numVertices * totalComponents;
				vertices = new float[verticesSize];
			}
			else
			{
				verticesSize = numVertices * componentsPerVertex;
				vertices = new float[verticesSize];

				if (useNormals)
				{
					normalsSize	= numVertices * componentsPerNormal;
					normals	= new float[normalsSize];
				}
				if (useTangents)
				{
					tangentsSize = numVertices * componentsPerTangent;
					tangents = new float[tangentsSize];
				}
				if (useBitangents)
				{
					bitangentsSize = numVertices * componentsPerBitangent;
					bitangents = new float[bitangentsSize];
				}
				if (useUV)
				{
					uvSize = numVertices * componentsPerUV;
					uv = new float[uvSize];
				}
			}
		}

		void free()
		{
			if (vertices == nullptr) // Don't free the same memory twice
				return;

			if (isDataCollapsed)
			{
				delete[] vertices;
				vertices = nullptr;
				verticesSize = 0ll;
			}
			else
			{
				delete[] vertices;
				vertices = nullptr;
				verticesSize = 0ll;			

				if (useNormals)
				{
					delete[] normals;
					normals = nullptr;
					normalsSize = 0ll;		
				}
				if (useTangents)
				{
					delete[] tangents;
					tangents = nullptr;
					tangentsSize = 0ll;			
				}
				if (useBitangents)
				{
					delete[] bitangents;
					bitangents = nullptr;
					bitangentsSize = 0ll;	
				}
				if (useUV)
				{
					delete[] uv;
					uv = nullptr;
					uvSize = 0ll;
				}
			}
		}

		// NOTE: Use native pointers here to increase model loading speed
		float* vertices;
		float* normals;
		float* tangents;
		float* bitangents;
		float* uv; // TODO: Allow multiple UVs (Check Assimp)

		size_t verticesSize; // Size measured in "number of floats", not bytes
		size_t normalsSize; // Size measured in "number of floats", not bytes
		size_t tangentsSize; // Size measured in "number of floats", not bytes
		size_t bitangentsSize; // Size measured in "number of floats", not bytes
		size_t uvSize; // Size measured in "number of floats", not bytes

		std::multimap<PrimitiveAssemblyMethod, std::vector<unsigned int>> faces; // Indices segmented by primitive assembly (allow key repetitions)
	
		bool isDataCollapsed; // If true, vertex, normal and uv data is included in vertices vector

		const short componentsPerVertex;
		const short componentsPerNormal;
		const short componentsPerTangent;
		const short componentsPerBitangent;
		const short componentsPerUV;

		bool useNormals; // TODO: Implement useNormals
		bool useTangents; // TODO: Implement useTangents
		bool useBitangents; // TODO: Implement useBitangents
		bool useUV; // TODO: Implement
		bool useFaces; // If set, indices stored in faces will be used to draw

		PrimitiveAssemblyMethod primitiveAssemblyMethod; // If useFaces is false, this is the method used to assemble primitives
	};

	struct MeshCube final : public Mesh
	{
		MeshCube();
		virtual ~MeshCube() {}
	};

	struct MeshSphere final : public Mesh
	{
		explicit MeshSphere(Engine* const engine, unsigned int meridians = 60, unsigned int parallels = 60);
		virtual ~MeshSphere() {}
	};

	struct MeshPlane final : public Mesh
	{
		MeshPlane();
		virtual ~MeshPlane() {}
	};
}