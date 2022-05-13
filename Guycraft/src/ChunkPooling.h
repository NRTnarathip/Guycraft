#pragma once
#include <iostream>
#include <Chunk.h>
#include <SmartQueue.h>
#include <ChunkContainer.h>

class ChunkPooling {
public:
	SmartQueue<Chunk*> m_chunkPooling;
	ChunkContainer m_chunkUsing;
	//use main thread
	Chunk* makeObject(glm::ivec2 pos);
	//use on other thread
	//dont forget use mutex
	void collectPooling(Chunk* chunk);
};