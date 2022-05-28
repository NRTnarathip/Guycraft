#include <MeshChunk.h>
#include <algorithm>
int MeshChunk::triangleGPU = 0;
void MeshChunk::Vertex::SetPos(float x, float y, float z) {
	pos[0] = u16(x * 16);
	pos[1] = u16(y * 16);
	pos[2] = u16(z * 16);
}
void MeshChunk::Vertex::SetUVTile(uint16_t tileIndex) {
	int x = tileIndex % TILE_ROW;
	int y = tileIndex / TILE_ROW;
	dataUVTNVI = dataUVTNVI | x | y << 4;
}
MeshChunk::MeshChunk() {
	setupMesh();
}
MeshChunk::~MeshChunk() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	clearOnGPU();
}
void MeshChunk::clearOnGPU() {
	//remove buffer object
	MeshChunk::triangleGPU -= triCount;
	triCount = 0;
	// Remove cpu data
	vertexs.clear();
	vertexs.shrink_to_fit();
	triangles.clear();
	triangles.shrink_to_fit();
}
void MeshChunk::setupMesh() {
	//gen buffer 3 type
	// VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// EBO
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	// position 3 axis with bitmask
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, pos));
	// uv tile; uv index of table texcoord; normal index of table normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, dataUVTNVI));
	// lighting ao,sun,lamp 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_UNSIGNED_SHORT, GL_FALSE, sizeof(Vertex), 
		(void*)offsetof(Vertex, lighting));

	
	//UnBine
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void MeshChunk::transferToGPU() {
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

	MeshChunk::triangleGPU += triCount;
}
void MeshChunk::draw() {
	// Mesh must be on gpu to draw
	if (triCount == 0) return;

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}