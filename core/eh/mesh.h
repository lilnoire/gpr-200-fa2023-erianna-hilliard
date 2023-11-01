/*
	Author: Eric Winebrenner
*/

#pragma once
#include "ehMath/ehMath.h"

namespace eh {
	struct Vertex {
		eh::Vec3 pos;
		eh::Vec3 normal;
		eh::Vec2 uv;
	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	class Mesh {
	public:
		Mesh() {};
		Mesh(const MeshData& meshData);
		void load(const MeshData& meshData);
		void draw()const;
		inline int getNumVertices()const { return m_numVertices; }
		inline int getNumIndices()const { return m_numIndices; }
	private:
		bool m_initialized = false;
		unsigned int m_vao = 0;
		unsigned int m_vbo = 0;
		unsigned int m_ebo = 0;
		int m_numVertices = 0;
		int m_numIndices = 0;
	};
}