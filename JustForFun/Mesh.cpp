/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "Mesh.h"

#include "Engine.h"
#include "Log.h"
#include "Vec.h"

JFF::MeshCube::MeshCube()
{
	isDataCollapsed = true;

	verticesSize = /* num vertices */ (size_t) 36 * ((size_t) componentsPerVertex + componentsPerNormal + componentsPerTangent + componentsPerBitangent + componentsPerUV);
	vertices = new float[]
	{
		// Vertices				Normals					Tangents				Bitangents				TexUV
		-0.5f,	-0.5f,	-0.5f,	0.0f,	0.0f,  -1.0f,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,
		0.5f,	 0.5f,	-0.5f,	0.0f,	0.0f,  -1.0f,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,	-0.5f,	0.0f,	0.0f,  -1.0f,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
		0.5f,	 0.5f,	-0.5f,	0.0f,	0.0f,  -1.0f,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		-0.5f,  -0.5f,	-0.5f,	0.0f,	0.0f,  -1.0f,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,
		-0.5f,	 0.5f,	-0.5f,	0.0f,	0.0f,  -1.0f,	-1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f,	0.0f,
																													
		-0.5f,  -0.5f,	 0.5f, 	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
		0.5f,  -0.5f,	 0.5f, 	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,
		0.5f,	 0.5f,	 0.5f, 	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f,	0.0f,
		0.5f,	 0.5f,	 0.5f, 	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f,	0.0f,
		-0.5f,	 0.5f,	 0.5f, 	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,	 0.5f, 	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
																													
		-0.5f,	 0.5f,	 0.5f, -1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f,	0.0f,
		-0.5f,	 0.5f,	-0.5f, -1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f, -1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f, -1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
		-0.5f,	-0.5f,	 0.5f, -1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,
		-0.5f,	 0.5f,	 0.5f, -1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f,	0.0f,
																														
		0.5f,	 0.5f,	 0.5f, 	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,	-0.5f, 	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,
		0.5f,	 0.5f,	-0.5f, 	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,	1.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,	-0.5f, 	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,
		0.5f,	 0.5f,	 0.5f, 	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,	 0.5f, 	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,
																														
		-0.5f,	-0.5f,	-0.5f, 	0.0f,  -1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,
		0.5f,	-0.5f,	-0.5f, 	0.0f,  -1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	0.0f,	0.0f,
		0.5f,	-0.5f,	 0.5f, 	0.0f,  -1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,	 0.5f, 	0.0f,  -1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	1.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,	 0.5f, 	0.0f,  -1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f, 	0.0f,  -1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,
																														
		-0.5f,	 0.5f,	-0.5f, 	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,
		0.5f,	 0.5f,	 0.5f, 	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f,	0.0f,
		0.5f,	 0.5f,	-0.5f, 	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	1.0f,	1.0f,	0.0f,
		0.5f,	 0.5f,	 0.5f, 	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	1.0f,	0.0f,	0.0f,
		-0.5f,	 0.5f,	-0.5f, 	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	1.0f,	0.0f,
		-0.5f,	 0.5f,	 0.5f, 	0.0f,	1.0f,	0.0f,	1.0f,	0.0f,	0.0f,	0.0f,	0.0f,	-1.0f,	0.0f,	0.0f,	0.0f,
	};

}

JFF::MeshSphere::MeshSphere(Engine* const engine, unsigned int meridians, unsigned int parallels)
{
	if (meridians < 2 || parallels < 3)
	{
		JFF_LOG_ERROR("A minimum of 2 meridians and 3 parallels are needed to build a sphere")
			return;
	}

	// Set Mesh parameters
	useFaces = true;

	size_t numVertices = ((size_t) meridians + 2) * ((size_t) parallels + 1); // +2: north and south poles; +1: one last parallel to match the first one

	verticesSize	= numVertices * componentsPerVertex;
	normalsSize		= numVertices * componentsPerNormal;
	tangentsSize	= numVertices * componentsPerTangent;
	bitangentsSize	= numVertices * componentsPerBitangent;
	uvSize			= numVertices * componentsPerUV;

	vertices	= new float[verticesSize];
	normals		= new float[normalsSize];
	tangents	= new float[tangentsSize];
	bitangents	= new float[bitangentsSize];
	uv			= new float[uvSize];

	// Calculate sphere subdivisions by meridians and parallels
	float interMeridianAngle = 180.0f / (meridians + 1.0f);
	float interParallelAngle = 360.0f / parallels;

	// Calculate UV subdivisions by meridinas and parallels
	float UVChunkX = 1.0f / parallels;
	float UVChunkY = 1.0f / (meridians + 1.0f);

	// -------------- Add meridian and parallel vertices, including north and south pole. Build from buttom up -------------- //

	auto math = engine->math.lock();

	meridians += 2; // Add two more meridians: One for north pole and another for the south pole
	for (unsigned int meridian = 0; meridian < meridians; ++meridian)
	{
		unsigned int meridianIndexVertex	= meridian * (parallels + 1) * componentsPerVertex;
		unsigned int meridianIndexNormal	= meridian * (parallels + 1) * componentsPerNormal;
		unsigned int meridianIndexTangent	= meridian * (parallels + 1) * componentsPerTangent;
		unsigned int meridianIndexBitangent	= meridian * (parallels + 1) * componentsPerBitangent;
		unsigned int meridianIndexUV		= meridian * (parallels + 1) * componentsPerUV;

		float pitchRad = math->radians(-90.0f + interMeridianAngle * meridian);
		for (unsigned int parallel = 0; parallel < parallels; ++parallel)
		{
			unsigned int parallelIndexVertex	= parallel * componentsPerVertex;
			unsigned int parallelIndexNormal	= parallel * componentsPerNormal;
			unsigned int parallelIndexTangent	= parallel * componentsPerTangent;
			unsigned int parallelIndexBitangent = parallel * componentsPerBitangent;
			unsigned int parallelIndexUV		= parallel * componentsPerUV;

			float yawRad = -math->radians(interParallelAngle * parallel); // Negative angle to correct winding order

			// Polar coordinates (useful for vertex position and normals)
			float x = math->cos(pitchRad) * math->cos(yawRad);
			float y = math->sin(pitchRad);
			float z = math->cos(pitchRad) * math->sin(yawRad);

			// Tangents (90º degrees from normal on XZ plane). Note that pitch isn't part of the formulas
			float yawOrthogonalRad = yawRad - math->radians(90.0f); // Negative angle to correct winding order
			float tanX = math->cos(yawOrthogonalRad);
			float tanY = 0.0f;
			float tanZ = math->sin(yawOrthogonalRad);

			// Bitangents (orthogonal to normal and bitangent)
			Vec3 bitangent = math->cross(Vec3(x, y, z), Vec3(tanX, tanY, tanZ)); // cross(Normal,Tangent)

			// UV coordinates
			float u = UVChunkX * parallel;
			float v = UVChunkY * meridian;

#pragma warning (disable:6386) // Disable buffer overrun writing warning. I'm sure I won't overrun this memory
			vertices[meridianIndexVertex + parallelIndexVertex + 0] = x;
			vertices[meridianIndexVertex + parallelIndexVertex + 1] = y;
			vertices[meridianIndexVertex + parallelIndexVertex + 2] = z;

			normals[meridianIndexNormal + parallelIndexNormal + 0] = x;
			normals[meridianIndexNormal + parallelIndexNormal + 1] = y;
			normals[meridianIndexNormal + parallelIndexNormal + 2] = z;

			tangents[meridianIndexTangent + parallelIndexTangent + 0] = tanX;
			tangents[meridianIndexTangent + parallelIndexTangent + 1] = tanY;
			tangents[meridianIndexTangent + parallelIndexTangent + 2] = tanZ;

			bitangents[meridianIndexBitangent + parallelIndexBitangent + 0] = bitangent.x;
			bitangents[meridianIndexBitangent + parallelIndexBitangent + 1] = bitangent.y;
			bitangents[meridianIndexBitangent + parallelIndexBitangent + 2] = bitangent.z;

			uv[meridianIndexUV + parallelIndexUV + 0] = u;
			uv[meridianIndexUV + parallelIndexUV + 1] = v;
			uv[meridianIndexUV + parallelIndexUV + 2] = 0.0f; // Remember: UV uses vec3
#pragma warning (default:6386)
		}

		unsigned int parallelLastIndexVertex	= parallels * componentsPerVertex;
		unsigned int parallelLastIndexNormal	= parallels * componentsPerNormal;
		unsigned int parallelLastIndexTangent	= parallels * componentsPerTangent;
		unsigned int parallelLastIndexBitangent = parallels * componentsPerBitangent;
		unsigned int parallelLastIndexUV		= parallels * componentsPerUV;

		// Add a last parallel point that matches the first one, but with different UV
		float yawRad = 0;

		float x = math->cos(pitchRad) * math->cos(yawRad);
		float y = math->sin(pitchRad);
		float z = math->cos(pitchRad) * math->sin(yawRad);

		// Tangents (90º degrees from normal on XZ plane). Note that pitch isn't part of the formulas
		float yawOrthogonalRad = yawRad - math->radians(90.0f); // Negative angle to correct winding order
		float tanX = math->cos(yawOrthogonalRad);
		float tanY = 0.0f;
		float tanZ = math->sin(yawOrthogonalRad);

		// Bitangents (orthogonal to normal and bitangent)
		Vec3 bitangent = math->cross(Vec3(x, y, z), Vec3(tanX, tanY, tanZ)); // cross(Normal,Tangent)

		float u = 1.0f;
		float v = UVChunkY * meridian;

		vertices[meridianIndexVertex + parallelLastIndexVertex + 0] = x;
		vertices[meridianIndexVertex + parallelLastIndexVertex + 1] = y;
		vertices[meridianIndexVertex + parallelLastIndexVertex + 2] = z;

		normals[meridianIndexNormal + parallelLastIndexNormal + 0] = x;
		normals[meridianIndexNormal + parallelLastIndexNormal + 1] = y;
		normals[meridianIndexNormal + parallelLastIndexNormal + 2] = z;

		tangents[meridianIndexTangent + parallelLastIndexTangent + 0] = tanX;
		tangents[meridianIndexTangent + parallelLastIndexTangent + 1] = tanY;
		tangents[meridianIndexTangent + parallelLastIndexTangent + 2] = tanZ;

		bitangents[meridianIndexBitangent + parallelLastIndexBitangent + 0] = bitangent.x;
		bitangents[meridianIndexBitangent + parallelLastIndexBitangent + 1] = bitangent.y;
		bitangents[meridianIndexBitangent + parallelLastIndexBitangent + 2] = bitangent.z;

		uv[meridianIndexUV + parallelLastIndexUV + 0] = u;
		uv[meridianIndexUV + parallelLastIndexUV + 1] = v;
		uv[meridianIndexUV + parallelLastIndexUV + 2] = 0.0f; // Remember: UV uses vec3
	}

	// Now each meridian has one parallel more
	++parallels;

	// -------------- Fill faces -------------- //

	// Fill sphere body
	for (unsigned int meridianPair = 0; meridianPair < (meridians - 1); ++meridianPair)
	{
		// Gets the indices around Nth and (N+1)th meridians
		unsigned int lowIdx = meridianPair * parallels;
		unsigned int highIdx = (meridianPair + 1) * parallels;

		// Loop over the two meridians and build a stripe
		std::vector<unsigned int> indices;
		indices.reserve((size_t) parallels * 2);
		for (unsigned int i = 0; i < (parallels * 2); ++i)
		{
			if (i % 2 == 0) // pairs index
				indices.push_back(highIdx++); // Insert first, increment later
			else // Odds index
				indices.push_back(lowIdx++); // Insert first, increment later
		}

		faces.insert(std::pair<PrimitiveAssemblyMethod, std::vector<unsigned int>>(PrimitiveAssemblyMethod::TRIANGLE_STRIP, indices));
	}
}

JFF::MeshPlane::MeshPlane()
{
	isDataCollapsed = true;
	primitiveAssemblyMethod = PrimitiveAssemblyMethod::TRIANGLE_STRIP;
	useTangents = false;
	useBitangents = false;

	verticesSize = /* num vertices */ (size_t) 4 * ((size_t) componentsPerVertex + componentsPerNormal + componentsPerUV);
	vertices = new float[]
	{
		// Vertices				Normals					TexUV
		-1.0f,  1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f, 0.0f,
	};
}
