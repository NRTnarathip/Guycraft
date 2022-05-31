#pragma once
#include <Chunk.h>
#include <iostream>
#include <GenerateMap/GenerateMap.h>
#include <Biome.h>
#include <FastNoise/FastNoise.h>
#include <JobPopulateChunk.h>

//tool for generate height, moiseture, heat map
class TerrainGen {
public:
	TerrainGen();
	GenerateMap genmap;
	void populate(JobPopulateChunk* job);
	Block GetVoxelTerrain(float genMapData[3], int y);
	std::vector<Biome> m_biomes;
	FastNoise::SmartNode<FastNoise::Perlin> noise = FastNoise::New<FastNoise::Perlin>();

	int seed = 0;
	float genOctave(float nx, float nz, int octave);
};

