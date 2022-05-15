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
	void populate(JobPopulate* job);
	Voxel GetVoxelTerrain(float genMapData[3], int y);
	std::vector<Biome> m_biomes;
	int seed = 0;
	float genOctave(float nx, float nz, int octave);
};

