#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <mutex>
#include "Types.h"
class MeshChunk {
public:
	// Vertex layout
	class Vertex
	{
	public:
#define TILE_ROW 16
		uint16_t pos[3];
		uint16_t dataUVTNVI= 0;
		uint16_t lighting = 0;

		void SetPos(float x, float y, float z);
		void SetUVTile(uint16_t tileIndex);
		void setVertexIndex(int index) {
			dataUVTNVI = dataUVTNVI | index << 8;
		}
		void SetNormal(unsigned int index) {
			dataUVTNVI = dataUVTNVI | index << 11;
		}
		void SetAO(unsigned char val) {
			lighting = lighting | (val << 8);
		}
	};
	MeshChunk();
	~MeshChunk();
	static int triangleGPU;
	// 3D directions
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	std::vector<Vertex> vertexs;
	std::vector<GLuint> triangles;
	void setupMesh();
	void transferToGPU();
	void draw();
	void clearOnGPU();
	//just clear all vertex, triangle;
	void clearData();
	unsigned int m_lastTriangleOnGPU= 0;
private:
};