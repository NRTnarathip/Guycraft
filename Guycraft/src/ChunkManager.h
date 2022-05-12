#pragma once
#include <iostream>
#include <map>
#include <queue>
#include <iterator>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <TerrainGen.h>
#include <ChunkGroup.h>

#include <thread>
#include <SmartQueue.h>
#include <vector>
#include <SmartChunkPooling.h>
#include "ChunkMeshBuilding.h"
#include "ChunkLoader.h"

#include "SmartMutex.h"
#include "ChunkGroupContainer.h"

class ChunkManager {
private:
	static ChunkManager* m_instance;
public:
	ChunkManager() { m_instance = this; }
	static ChunkManager* GetInstance() { return m_instance; }
	ChunkMeshBuilding chunkMeshBuilding;
	ChunkLoader chunkLoader;

	std::queue<SmartChunkGroup*> m_queueNewChunkGroup;
	SmartChunkPooling chunkPooling;//dont del smChunk object on heap

	std::vector<std::thread> listThread;
	TerrainGen terrainGen;
	void init();
	void render();
	void update();

	static bool ChunkInRange(glm::ivec2 posPlayer, glm::ivec2 chunkPos);

	void initChunkNear(SmartChunkGroup* cg, SmartChunkGroup* cgNearNorth,
		SmartChunkGroup* cgNearSouth, SmartChunkGroup* cgNearEast, SmartChunkGroup* cgNearWest);
	glm::ivec3 ToChunkPosition(glm::vec3 pos) const;
	glm::ivec3 lastViewPos;

public: //none safe thread
	ChunkGroupContainer chunkGroups;
	SmartChunkGroup* getChunkGroup(glm::ivec2 pos);
};