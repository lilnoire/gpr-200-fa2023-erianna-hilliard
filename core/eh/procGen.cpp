/*
	Author: Eric Winebrenner
*/


#include "procGen.h"
#include <stdlib.h>

namespace eh {
	/// <summary>
	/// Helper function for createCube. Note that this is not meant to be used standalone
	/// </summary>
	/// <param name="normal">Normal direction of the face</param>
	/// <param name="size">Width/height of the face</param>
	/// <param name="mesh">MeshData struct to fill</param>
	static void createCubeFace(eh::Vec3 normal, float size, MeshData* mesh) {
		unsigned int startVertex = mesh->vertices.size();
		eh::Vec3 a = eh::Vec3(normal.z, normal.x, normal.y); //U axis
		eh::Vec3 b = eh::Cross(normal, a); //V axis
		for (int i = 0; i < 4; i++)
		{
			int col = i % 2;
			int row = i / 2;

			eh::Vec3 pos = normal * size * 0.5f;
			pos -= (a + b) * size * 0.5f;
			pos += (a * col + b * row) * size;
			Vertex vertex;// = &mesh->vertices[mesh->vertices.size()];
			vertex.pos = pos;
			vertex.normal = normal;
			vertex.uv = eh::Vec2(col, row);
			mesh->vertices.push_back(vertex);
		}

		//Indices
		mesh->indices.push_back(startVertex);
		mesh->indices.push_back(startVertex + 1);
		mesh->indices.push_back(startVertex + 3);
		mesh->indices.push_back(startVertex + 3);
		mesh->indices.push_back(startVertex + 2);
		mesh->indices.push_back(startVertex);
	}
	/// <summary>
	/// Creates a cube of uniform size
	/// </summary>
	/// <param name="size">Total width, height, depth</param>
	/// <param name="mesh">MeshData struct to fill. Will be cleared.</param>
	MeshData createCube(float size) {
		MeshData mesh;
		mesh.vertices.reserve(24); //6 x 4 vertices
		mesh.indices.reserve(36); //6 x 6 indices
		createCubeFace(eh::Vec3{ +0.0f,+0.0f,+1.0f }, size, &mesh); //Front
		createCubeFace(eh::Vec3{ +1.0f,+0.0f,+0.0f }, size, &mesh); //Right
		createCubeFace(eh::Vec3{ +0.0f,+1.0f,+0.0f }, size, &mesh); //Top
		createCubeFace(eh::Vec3{ -1.0f,+0.0f,+0.0f }, size, &mesh); //Left
		createCubeFace(eh::Vec3{ +0.0f,-1.0f,+0.0f }, size, &mesh); //Bottom
		createCubeFace(eh::Vec3{ +0.0f,+0.0f,-1.0f }, size, &mesh); //Back
		return mesh;
	}
}