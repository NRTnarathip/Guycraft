#pragma once
#ifndef CHUNK_H
#define CHUNK_H

#include <Renderer/shaderClass.h>
#include <glm/glm.hpp>
#include <mutex>
#include "Types.h"
#include "SmartUnorderMap.h"
#include "BlockSystem/Block.h"
#include <ChunkSection.h>
#include <ChunkMarco.h>

//none safe thread
class Chunk {
public:
	bool isShouldUnload = false;
	bool isLoad = false;
	std::mutex mutex;
	std::mutex mutexNeighbor;
	uint8_t m_light[CHUNK_BLOCK_ALL];
	glm::ivec2 pos;
	ChunkSection *m_chunks[CHUNK_SIZE];
	SmartUnorderMap<int, MeshChunkSection*> m_meshsActive;
	//chunk neighbor 8 direction
	Chunk* north = nullptr;
	Chunk* northEast = nullptr;
	Chunk* northWest = nullptr;
	Chunk* south = nullptr;
	Chunk* southEast = nullptr;
	Chunk* southWest = nullptr;
	Chunk* east = nullptr;
	Chunk* west = nullptr;
	void changeBlocks(Block blocks[CHUNK_BLOCK_ALL]);
	Block* getBlock(glm::ivec3 pos);
	Block* getBlock(int x, int y, int z) {
		return getBlock({ x,y,z });
	}
	void setBlock(glm::ivec3 pos, Block block);
	//setup when first new create object
	void init();
	void newSetupViaChunkPooling(glm::ivec2 newPos);
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
	void render(Shader* shaders[2]);
	void unload();
	void onload();
	int getHasChunkNeighborCount();
	void linkChunkNeighbor(Chunk* chunkNiehgbor[8]);

	std::vector<Chunk*> getAllChunkNeighbor();
};
#endif // !CHUNK_H
