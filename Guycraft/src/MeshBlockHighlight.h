#pragma once
#include "OpenGL Class/VAO.h"
#include "OpenGL Class/VBOAndEBO.h"
#include <vector>
#include <glm/glm.hpp>

class MeshBlockHighlight {
public:
	struct Vertex {
		Vertex(glm::vec3 pos) {
			position = pos;
		}
		glm::vec3 position;
	};
	std::vector<Vertex> vertices;
	std::vector<unsigned int> triangles;

	VAO vao;
	VBO vbo;
	EBO ebo;
	void genMeshCube(glm::vec3 pos);
	void setupMesh();
	void draw();

private:
	void genQuadFaceCube();
	void transferMeshToGPU();
};