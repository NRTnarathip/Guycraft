#include "MeshBlockHighlight.h"

void MeshBlockHighlight::setupMesh() {
	vao.genBuffer();
	vbo.genBuffer();
	ebo.genBuffer();
}
void MeshBlockHighlight::genMeshCube(glm::vec3 pos) {
	vertices.clear();
	triangles.clear();
	Vertex p0 = { pos + glm::vec3(0, 1, 0) };
	Vertex p1 = { pos + glm::vec3(0, 1, 1) };
	Vertex p2 = { pos + glm::vec3(1, 1, 1) };
	Vertex p3 = { pos + glm::vec3(1, 1, 0) };

	Vertex p4 = { pos };
	Vertex p5 = { pos + glm::vec3(0, 0, 1) };
	Vertex p6 = { pos + glm::vec3(1, 0, 1) };
	Vertex p7 = { pos + glm::vec3(1, 0, 0) };

	//face up
	vertices.push_back(p4);
	vertices.push_back(p5);
	vertices.push_back(p6);
	vertices.push_back(p7);
	genQuadFaceCube();

	//face down
	vertices.push_back(p1);
	vertices.push_back(p0);
	vertices.push_back(p3);
	vertices.push_back(p2);
	genQuadFaceCube();
	printf("vertices size %d\n",vertices.size());
}
void MeshBlockHighlight::genQuadFaceCube()
{
	uint32_t vertCount = vertices.size();
	uint32_t vertCountMinus_1 = vertCount - 1;//index 3
	uint32_t vertCountMinus_2 = vertCount - 2;//index 2
	uint32_t vertCountMinus_3 = vertCount - 3;//index 1
	uint32_t vertCountMinus_4 = vertCount - 4;//index 0

	triangles.push_back(vertCountMinus_4);
	triangles.push_back(vertCountMinus_3);
	triangles.push_back(vertCountMinus_1);

	triangles.push_back(vertCountMinus_2);
	triangles.push_back(vertCountMinus_1);
	triangles.push_back(vertCountMinus_3);
}
void MeshBlockHighlight::transferMeshToGPU() {
	if (triangles.size() == 0) return;

	// Send VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(*vertices.data()), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Send EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(*triangles.data()), triangles.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void MeshBlockHighlight::draw() {
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, triangles.size());
	vao.unbind();
}