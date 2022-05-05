#pragma once
#include <iostream>
#include <map>
#include <Mesh.h>
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
class ChunkManager {
public:
	typedef std::unordered_map<glm::ivec2, SmartChunkGroup* > ChunkGroupContainer;
	ChunkGroupContainer chunkGroups;
	SmartQueueChunkGroup queSpawnChunkGroup;
	SmartQueueChunkGroup queNeedPopulate;
	SmartQueueChunkGroup queNeedGenMeshChunkGroup;
	SmartQueueChunkGroup queDeleteChunk;

	SmartChunkPooling chunkPooling;//dont del smChunk object on heap

	std::vector<std::thread> listThread;
	TerrainGen terrainGen;
	void init(glm::vec3 posCamera);
	void render();
	void update();
	SmartChunkGroup* newChunkGroup(int x, int z);
	bool ChunkInRange(glm::vec3 posCamera, glm::ivec2 chunkPos, int distRender) const;
	bool checkShouldNewChunk(glm::ivec3 posCamera);
	void initChunkNear(SmartChunkGroup* cg, SmartChunkGroup* cgNearNorth,
		SmartChunkGroup* cgNearSouth, SmartChunkGroup* cgNearEast, SmartChunkGroup* cgNearWest);
	glm::ivec3 ToChunkPosition(glm::vec3 pos) const;
	SmartChunkGroup* getChunkGroup(int x, int z);
	glm::ivec3 lastViewPos;
};