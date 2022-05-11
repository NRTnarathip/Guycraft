#include "SmartChunkPooling.h"

SmartChunkGroup* SmartChunkPooling::makeSmartChunk(glm::ivec2 pos) {
	SmartChunkGroup* sm = nullptr;
	if (listPooling.size() == 0) {
		//create new SmartChunk*
		sm = new SmartChunkGroup(pos);
		auto posChunk = glm::ivec3(pos.x, 0, pos.y);
		for (int y = 0; y < 8; y++) {
			posChunk.y = y << BS_CH;
			sm->get()->chunks[y] = new Chunk(posChunk);
		}
	}
	//have pooling object
	else {
		sm = listPooling.getFront();
		//setup chunk group data
		auto chunkGroup = sm->get();
		chunkGroup->pos = pos;
		for (auto c : chunkGroup->chunks) {
			c->pos.x = pos.x;
			c->pos.z = pos.y;
		}
	}
	sm->onSetup();
	return sm;
}
void SmartChunkPooling::collectPoolingSub(Chunk* chunk) {
}
void SmartChunkPooling::collectPooling(SmartChunkGroup* smChunk) {
	//clear chunk all
	smChunk->onDelete();
	for (auto c : smChunk->get()->chunks) {
		c->lock();
		c->clearAll();
		c->unlock();
	}
	listPooling.push_back(smChunk);
}