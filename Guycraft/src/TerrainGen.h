#pragma once
#include <Chunk.h>
#include <iostream>
#include <GenerateMap/GenerateMap.h>
#include <Biome.h>
//tool for generate height, moiseture, heat map
class TerrainGen {
public:
	TerrainGen();
	GenerateMap genmap;
	void populate(Chunk* chunk);
	Voxel GetVoxelTerrain(Chunk*c, float genMapData[3], int y);
	std::vector<Biome> m_biomes;
	int seed = 0;
};

