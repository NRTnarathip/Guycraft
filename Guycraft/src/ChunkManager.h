#pragma once
#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H
#include <iostream>
#include <map>
#include <queue>
#include <iterator>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <TerrainGen.h>
#include <Chunk.h>
#include <SmartQueue.h>
#include <vector>
#include <ChunkPooling.h>
#include "ChunkMeshBuilding.h"
#include "ChunkLoader.h"

#include "SmartMutex.h"
#include "ChunkContainer.h"
#include "JobUpdateVoxel.h"

class ChunkManager {
private:
	static ChunkManager* m_instance;
public:
	ChunkManager() { m_instance = this; }
	static ChunkManager* GetInstance() { return m_instance; }
	ChunkMeshBuilding chunkMeshBuilding;
	ChunkLoader chunkLoader;
	ChunkPooling chunkPooling;

	SmartQueue<glm::ivec3> m_queueDestroyBlock;
	SmartQueue<JobUpdateVoxel> m_queueAddBlock;

	void init();
	void render();
	void update();
	void addQueueDestroyBlock(glm::ivec3 worldPos);


	static bool ChunkInRange(glm::ivec2 posPlayer, glm::ivec2 chunkPos);
	glm::vec3 ToChunkPosition(glm::vec3 pos) const;
	glm::ivec3 lastViewPos;

public: //none safe thread
	ChunkContainer chunks;
	Chunk* getChunk(glm::ivec2 pos);
};
#endif // !CHUNKMANAGER_H
