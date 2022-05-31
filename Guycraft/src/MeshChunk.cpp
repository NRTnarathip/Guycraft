#include <MeshChunk.h>
#include <algorithm>
int MeshChunk::triangleGPU = 0;
void MeshChunk::Vertex::setPos(float x, float y, float z) {
	pos[0] = u16(x * 16);
	pos[1] = u16(y * 16);
	pos[2] = u16(z * 16);
}
void MeshChunk::Vertex::setUVTile(uint16_t tileIndex) {
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
void MeshChunk::clearDataOnGenerate() {
	triangles.lock();
	triangles.m_vector.clear();
	triangles.m_vector.shrink_to_fit();
	triangles.unlock();

	vertexs.lock();
	vertexs.m_vector.clear();
	vertexs.m_vector.shrink_to_fit();
	vertexs.unlock();
}
void MeshChunk::clearDataComplete() {
	m_trianglesComplete.clear();
	m_trianglesComplete.shrink_to_fit();
	m_vertexsComplete.clear();
	m_vertexsComplete.shrink_to_fit();
}
void MeshChunk::clearOnGPU() {
	//remove buffer object on gpu
	{
		triangles_t temptriangles;
		// rewrite VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, temptriangles.size() * sizeof(*temptriangles.data()),
			temptriangles.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	{
		vertexs_t tempVertexs;
		// rewrite EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			tempVertexs.size() * sizeof(*tempVertexs.data()),
			tempVertexs.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	MeshChunk::triangleGPU -= m_lastTriangleOnGPU;
	m_lastTriangleOnGPU = 0;
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
	if (m_vertexsComplete.size() == 0) return;

	// Send VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertexsComplete.size() * sizeof(*m_vertexsComplete.data()), 
		m_vertexsComplete.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Remove cpu data
	m_vertexsComplete.clear();
	m_vertexsComplete.shrink_to_fit();

	// Send EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		m_trianglesComplete.size() * sizeof(*m_trianglesComplete.data()), 
		m_trianglesComplete.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Remove cpu data
	MeshChunk::triangleGPU -= m_lastTriangleOnGPU;
	m_lastTriangleOnGPU = m_trianglesComplete.size();
	MeshChunk::triangleGPU += m_lastTriangleOnGPU;

	m_trianglesComplete.clear();
	m_trianglesComplete.shrink_to_fit();

}
void MeshChunk::makeCopyDataJobToComplete()
{
	triangles.lock();
	triangles.transferData(m_trianglesComplete);
	triangles.unlock();

	vertexs.lock();
	vertexs.transferData(m_vertexsComplete);
	vertexs.unlock();
}
void MeshChunk::draw() {
	// Mesh must be on gpu to draw
	if (m_lastTriangleOnGPU == 0) return;

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, m_lastTriangleOnGPU, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}