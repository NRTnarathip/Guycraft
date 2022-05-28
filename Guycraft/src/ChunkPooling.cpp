#include "ChunkPooling.h"

Chunk* ChunkPooling::get() {
	Chunk* chunk = nullptr;
	m_chunkPooling.lock();
	if (m_chunkPooling.size() == 0) {
		chunk = new Chunk();
		m_chunks.push_back(chunk);
	}
	//have pooling object
	else {
		chunk = m_chunkPooling.getFront();
	}
	m_chunkPooling.unlock();
	return chunk;
}
void ChunkPooling::collectPooling(Chunk* c) {
	m_chunkPooling.pushFrontLock(c);
}