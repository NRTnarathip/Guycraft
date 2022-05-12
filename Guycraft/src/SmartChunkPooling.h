#pragma once
#include <iostream>
#include <SmartChunk.h>
#include <mutex>
#include <queue>

class SmartChunkPooling {
public:
	std::queue<SmartChunkGroup*> listPooling;
	//use main thread
	SmartChunkGroup* makeSmartChunk(glm::ivec2 pos);
	//use on other thread
	//dont forget use mutex
	void collectPooling(SmartChunkGroup* smChunk);
};