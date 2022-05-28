#include "MeshBlockHighlight.h"
#include "ResourceManager.h"
#include "CameraManager.h"
#include "SceneManager.h"

void MeshBlockHighlight::setupMesh() {
	vao.genBuffer();
	vbo.genBuffer();
	ebo.genBuffer();

	vao.bind();
	vbo.bind();
	ebo.bind();

	//bind layout position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, Vertex::position));

	vao.unbind();
	vbo.unbind();
	ebo.unbind();
}
void MeshBlockHighlight::genMeshCube(glm::vec3 pos) {
	Vertex p0 = { pos + glm::vec3(0, 1, 0) };
	Vertex p1 = { pos + glm::vec3(0, 1, 1) };
	Vertex p2 = { pos + glm::vec3(1, 1, 1) };
	Vertex p3 = { pos + glm::vec3(1, 1, 0) };

	Vertex p4 = { pos };
	Vertex p5 = { pos + glm::vec3(0, 0, 1) };
	Vertex p6 = { pos + glm::vec3(1, 0, 1) };
	Vertex p7 = { pos + glm::vec3(1, 0, 0) };

	//face up
	vertices.push_back(p0);
	vertices.push_back(p1);
	vertices.push_back(p2);
	vertices.push_back(p3);
	genQuadFaceCube();
	//face down
	vertices.push_back(p5);
	vertices.push_back(p4);
	vertices.push_back(p7);
	vertices.push_back(p6);
	genQuadFaceCube();
	//face north
	vertices.push_back(p6);
	vertices.push_back(p2);
	vertices.push_back(p1);
	vertices.push_back(p5);
	genQuadFaceCube();
	//face south
	vertices.push_back(p4);
	vertices.push_back(p0);
	vertices.push_back(p3);
	vertices.push_back(p7);
	genQuadFaceCube();
	//face east
	vertices.push_back(p7);
	vertices.push_back(p3);
	vertices.push_back(p2);
	vertices.push_back(p6);
	genQuadFaceCube();
	//face west
	vertices.push_back(p5);
	vertices.push_back(p1);
	vertices.push_back(p0);
	vertices.push_back(p4);
	genQuadFaceCube();
	

	transferMeshToGPU();
}
void MeshBlockHighlight::genQuadFaceCube()
{
	uint32_t vertCount = vertices.size();
	uint32_t vertIndex0 = vertCount - 4;//index 0
	uint32_t vertIndex1 = vertCount - 3;//index 1
	uint32_t vertIndex2 = vertCount - 2;//index 2
	uint32_t vertIndex3 = vertCount - 1;//index 3

	triangles.push_back(vertIndex0);
	triangles.push_back(vertIndex1);
	triangles.push_back(vertIndex3);

	triangles.push_back(vertIndex2);
	triangles.push_back(vertIndex3);
	triangles.push_back(vertIndex1);
}
void MeshBlockHighlight::transferMeshToGPU() {
	if (triangles.size() == 0) return;

	// Send VBO
	vbo.bind();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), 
		&vertices[0], GL_DYNAMIC_DRAW);
	vbo.bind();

	// Send EBO
	ebo.bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint), 
		&triangles[0], GL_DYNAMIC_DRAW);
	ebo.unbind();

	triCount = triangles.size();
	vertices.clear();
	triangles.clear();
}
void MeshBlockHighlight::render() {
	if (triCount == 0 or not isActive) return;
	//check scene is renderer on without depth test

	//setup shader
	auto shaderOutline = ResourceManager::GetInstance()->m_shaders["block_outline"];
	CameraManager::GetInstance().uploadCameraMatrixToShader(shaderOutline);
	shaderOutline->Bind();
	vao.bind();

	auto came = CameraManager::GetCurrentCamera();
	came->setRenderMode(1);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(4);
	glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, 0);
	came->setRenderMode(0);
	glEnable(GL_DEPTH_TEST);

	vao.unbind();
	shaderOutline->UnBind();
}