#pragma once
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Types.h"
#include <SmartVector.h>

class MeshChunk : public IMutex {
public:
	struct Vertex
	{
	public:
		uint16_t pos[3];
		float uv[2];
		//normal
		uint16_t dataVertex = 0;
		uint16_t lighting = 0;

		void setPos(float x, float y, float z);
		void setUV(glm::vec2 newUV) {
			uv[0] = newUV.x;
			uv[1] = newUV.y;
		}
		void setNormal(unsigned int index) {
			dataVertex = dataVertex | index << 11;
		}
		void setAO(unsigned char val) {
			lighting = lighting | (val << 8);
		}
		void setLight(unsigned char val) {
			lighting = lighting | val;
		}
	};

	typedef std::vector<Vertex> vertexs_t;
	typedef std::vector<uint32_t> triangles_t;

	MeshChunk();
	~MeshChunk();
	static int triangleGPU;
	// 3D directions
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	//on generate mesh. none saft thread
	SmartVector<Vertex> vertexs;
	SmartVector<uint32_t> triangles;
	//after generate mesh end. saft thread
	std::vector<Vertex> m_vertexsComplete;
	std::vector<uint32_t> m_trianglesComplete;

	void setupMesh();
	void transferToGPU();
	void makeCopyDataJobToComplete();
	void draw();

	void clearOnGPU();
	//is safty thread
	void clearDataOnGenerate();
	void clearDataComplete();
	unsigned int m_lastTriangleOnGPU= 0;
};