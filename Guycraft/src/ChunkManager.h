#pragma once
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

class ChunkManager {
private:
	static ChunkManager* m_instance;
public:
	ChunkManager() { m_instance = this; }
	static ChunkManager* GetInstance() { return m_instance; }
	ChunkMeshBuilding chunkMeshBuilding;
	ChunkLoader chunkLoader;
	ChunkPooling chunkPooling;
	void init();
	void render();
	void update();

	static bool ChunkInRange(glm::ivec2 posPlayer, glm::ivec2 chunkPos);
	glm::ivec3 ToChunkPosition(glm::vec3 pos) const;
	glm::ivec3 lastViewPos;

public: //none safe thread
	ChunkContainer chunks;
	Chunk* getChunk(glm::ivec2 pos);
};