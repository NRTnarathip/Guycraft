#include "ChunkPooling.h"

Chunk* ChunkPooling::makeObject(glm::ivec2 pos) {
	Chunk* chunk = nullptr;
	m_chunkPooling.lock();
	if (m_chunkPooling.size() == 0) {
		chunk = new Chunk(pos);
	}
	//have pooling object
	else {
		chunk = m_chunkPooling.getFront();
		chunk->pos = pos;
	}
	m_chunkPooling.unlock();
	return chunk;
}
void ChunkPooling::collectPooling(Chunk* c) {
	m_chunkPooling.lock();
	m_chunkPooling.push(c);
	m_chunkPooling.unlock();
}