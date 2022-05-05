#include <Mesh.h>
#include <algorithm>
int Mesh::triangleGPU = 0;

void Vertex::SetUVTile(unsigned int voxID, unsigned int voxType, unsigned int tileRow) {
	//use 12bit
	//each axis use 6bit range 0->63;
	//bit int 
	// 0000 0000 0000 0000 0000 0000 0000 0000
	unsigned int index = voxID + voxType;
	unsigned int x = (index % tileRow);
	unsigned int y = (index / tileRow);
	uvTileAndNormal = uvTileAndNormal | x;
	uvTileAndNormal = uvTileAndNormal | (y << 6); //mask
}
unsigned int Vertex::GetUVTile() {
	unsigned int x = uvTileAndNormal & 63;//mask 63;
	unsigned int y = (uvTileAndNormal >> 6) & 63;
	return x + y;
}
void Vertex::SetUVIndex(unsigned int index) {
	//use 0->3 array index; 2bit
	uvTileAndNormal = uvTileAndNormal | (index << 12);//range 13->14
}
unsigned int Vertex::GetUVIndex() {
	return (uvTileAndNormal >> 12) & 3;
}
void Vertex::SetNormal(unsigned int index) {
	//use 3bit range 0-7;
	uvTileAndNormal = uvTileAndNormal | (index << 14); //range 15->17
}
Mesh::Mesh() {
	setupMesh();
}
Mesh::~Mesh() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	clearOnGPU();
}

void Mesh::clearOnGPU() {
	//remove buffer object
	Mesh::triangleGPU -= triCount;
	triCount = 0;
	//should remove data vertex and triangle on gpu memory
	//or you can dont draw this if chunk is non gen mesh success;
	/*glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);*/

	// Remove cpu data
	vertexs.clear();
	vertexs.shrink_to_fit();
	triangles.clear();
	triangles.shrink_to_fit();
}
void Mesh::setupMesh() {
	// VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// position 3 axis with bitmask
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_BYTE, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, pos));
	// uv tile; uv index of table texcoord; normal index of table normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, uvTileAndNormal));
	// lighting ao,sun,lamp 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(Vertex), 
		(void*)offsetof(Vertex, lighting));

	// EBO
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	//UnBine
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Mesh::transferToGPU() {
	if (triangles.size() == 0) return;

	// Send VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(*vertexs.data()), vertexs.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Remove cpu data
	vertexs.clear();
	vertexs.shrink_to_fit();

	// Send EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(*triangles.data()), triangles.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Remove cpu data
	triCount = triangles.size();

	triangles.clear();
	triangles.shrink_to_fit();

	Mesh::triangleGPU += triCount;
}
void Mesh::draw() {
	// Mesh must be on gpu to draw
	if (triCount == 0) return;
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}