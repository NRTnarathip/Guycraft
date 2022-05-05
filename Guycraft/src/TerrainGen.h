#pragma once
#include <ChunkGroup.h>
#include <iostream>

class TerrainGen {
public:
	int GetHeight(glm::vec2 pos);
	void populate(ChunkGroup* cg);
	void getHeightMap(ChunkGroup * cg);
	Voxel GetVoxelTerrain(Chunk*c,unsigned char heightMapVar,int x, int y, int z);
	int seed = 0;
};

