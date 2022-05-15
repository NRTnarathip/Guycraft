#pragma once
#include <iostream>
#include <Chunk.h>
#include <SmartQueue.h>
#include <ChunkContainer.h>

class ChunkPooling {
public:
	SmartQueue<Chunk*> m_chunkPooling;
	std::vector<Chunk*> m_chunks;
	//use main thread
	Chunk* get();
	//use on other thread
	//dont forget use mutex
	void collectPooling(Chunk* chunk);
};