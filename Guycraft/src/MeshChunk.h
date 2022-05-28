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
		u16 pos[3];
		//for 1 voxel group
		//range 0->31; each x y z

		void SetPos(float x, float y, float z);
		//float position[3];//-125-> 124;
		//use vertex index:2bit, uvTileIndex:4bit, normal table index:2bit
		unsigned short dataUVTNVI= 0;

		unsigned short lighting = 0;

		//range 0000:0000 1111:1111
		void SetUVTile(uint16_t tileIndex);
		//range 0000:0011 0000:0000
		void setVertexIndex(int index) {
			dataUVTNVI = dataUVTNVI | index << 8;
		}
		//range 0001:1100 0000:0000
		void SetNormal(unsigned int index) {
			dataUVTNVI = dataUVTNVI | index << 11;
		}
		
		unsigned char GetSun() {
			return lighting & 15;
		}
		void SetSun(unsigned char val) {
			lighting = lighting | val;
		}
		unsigned char GetLamp() {
			return (lighting >> 4) & 15;
		}
		void SetLamp(unsigned char val) {
			lighting = lighting | (val << 4);
		}
		void SetAO(unsigned char val) {
			lighting = lighting | (val << 8);
		}
		unsigned char GetAO() {
			return (lighting >> 8) & 3;
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
	unsigned int reserveAmount = 0;
private:
	unsigned int triCount = 0;
};