#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

// Vertex layout
struct Vertex
{
	//char position[3];//-125-> 124;
	//each axis use 6bit all use 18bit
	//unsigned int pos = 0;
	char pos[3];
	void SetPos(int x, int y, int z) {
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	}
	//float position[3];//-125-> 124;
	//int == 4byte 32bit;
	unsigned int uvTileAndNormal = 0;
	unsigned int GetUVTile();
	unsigned int GetUVIndex();
	void SetUVTile(unsigned int voxID, unsigned int voxType,unsigned int tileRow);
	void SetUVIndex(unsigned int i);
	void SetNormal(unsigned int index);
	unsigned short lighting = 0;//
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
class DirectionHelper{
public:
	const glm::ivec3 directionVectors[6] = {
	glm::ivec3(0,1,0),
	glm::ivec3(0,-1,0),
	glm::ivec3(0,0,1),
	glm::ivec3(0,0,-1),
	glm::ivec3(1,0,0),
	glm::ivec3(-1,0,0),
	};
};
enum Direction
{
	up, down,
	forward, backward,
	left, right,
	count,
};
struct Mesh {
public:
	Mesh();
	~Mesh();

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
	bool isHaveData() {
		if (triCount > 0) return true;
		return false;
	}
	unsigned int reserveAmount = 0;
private:
	unsigned int triCount = 0;
};