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
	SmartChunkGroup* makeSmartChunk(glm::ivec2 pos) {

		SmartChunkGroup* sm = NULL;
		if (listPooling.size() == 0) {
			//create new SmartChunk*
			//dont add to list pooling
			sm = new SmartChunkGroup(pos);
			auto posChunk = glm::ivec3(pos.x, 0, pos.y);
			for (int y = 0; y < 8; y++) {
				posChunk.y = y << 5;
				sm->get()->chunks[y] = new Chunk(posChunk);
			}
		}
		//have pooling object
		else {
			sm = listPooling.getFront();

			//setup chunk group data
			auto chunkGroup = sm->get();
			chunkGroup->pos = pos;

			//setup chunk sub data
			auto posChunk = glm::ivec3(pos.x, 0, pos.y);
			for (int y = 0; y < 8; y++) {
				posChunk.y = y << 5;
				chunkGroup->chunks[y]->pos = posChunk;

				//dont gen vao,vbo,ebo again
				//memory leak
				//chunkGroup->chunks[y]->mesh.setupMesh();
			}
		}
		sm->onSetup();
		return sm;
	}
	//use on other thread
	//dont forget use mutex
	void collectPooling(SmartChunkGroup* smChunk) {
		//clear chunk all
		smChunk->onDelete();
		for (auto c : smChunk->get()->chunks) {
			c->mesh.clearOnGPU();
		}
		listPooling.push_back(smChunk);
	}
};