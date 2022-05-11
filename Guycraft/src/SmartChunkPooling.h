#pragma once
#include <vector>
#include <iostream>
#include <SmartChunk.h>
#include <deque>
#include <mutex>
#include <SmartQueue.h>

class SmartChunkPooling {
public:
	SmartDequeChunkGroup listPooling;
	//use main thread
	SmartChunkGroup* makeSmartChunk(glm::ivec2 pos);
	//use on other thread
	//dont forget use mutex
	void collectPooling(SmartChunkGroup* smChunk);
	void collectPoolingSub(Chunk* chunk);
};