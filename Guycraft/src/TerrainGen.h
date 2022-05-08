#pragma once
#include <ChunkGroup.h>
#include <iostream>
#include <GenerateMap/GenerateMap.h>

//tool for generate height, moiseture, heat map
class TerrainGen {
public:
	GenerateMap genmap;
	void populate(ChunkGroup* cg);
	Voxel GetVoxelTerrain(Chunk*c, float genMapData[3], int localY);
	int seed = 0;
};

