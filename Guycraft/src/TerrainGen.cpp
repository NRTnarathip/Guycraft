#include <TerrainGen.h>
#include <Chunk.h>
#include <GenerateMap/GenerateMap.h>
#include "Biomes/Plant.h"
#include "Biomes/Biomes.h"
#include <FastNoise/FastNoise.h>

TerrainGen::TerrainGen()
{
    auto boPlant = Biome();
    auto boDesert = Biome();
    auto boBeach = Biome();
    auto boSnow = Biome();
    boPlant.name = "Plant";

    m_biomes.push_back(boPlant);
}

void TerrainGen::populate(Chunk* chunk) {
    float heightMap[CHUNK_SIZE_SQUARED] = {};
    float moisetureMap[CHUNK_SIZE_SQUARED] = {};
    float heatMap[CHUNK_SIZE_SQUARED] = {};


    float chunkSize = CHUNK_SIZE;
    float scale = .18f;
    auto noise = FastNoise::New<FastNoise::Perlin>();

    auto pos = chunk->pos;
    //gen all noise map
    for (int k = 0; k < CHUNK_SIZE; k++) {
        int z = k + pos.y;
        float nz = ((z / chunkSize) - 0.5f) * scale;

        for (int i = 0; i < CHUNK_SIZE; i++) {
            int accMap = i + (k << BS_CH);
            int x = i + pos.x;
            float nx = ((x / chunkSize) - 0.5f) * scale;

            //gen height map
            float n = noise->GenSingle2D(nx, nz, seed);
            n = (n + 1) / 2.f;
            heightMap[accMap] = n;
            //gen moiseture map
            float nMoiseture = noise->GenSingle2D(nx, nz, seed);
            nMoiseture = (nMoiseture + 1) / 2.f;
            moisetureMap[accMap] = nMoiseture;
        }
    }
    //place biome block
    for (u8 group = 0; group < 8; group++) {
        for (unsigned char z = 0; z < CHUNK_SIZE; z++) {
            for (unsigned char x = 0; x < CHUNK_SIZE; x++) {
                int accDataMap = x + (z << BS_CH);
                float noiseData[3] = { heightMap[accDataMap],0,0 };
                for (unsigned char y = 0; y < CHUNK_SIZE; y++) {
                    int height = y + (group * 32);
                    chunk->voxels[x + (y << BS_CH) + (z << BS_CH2) + (group * CHUNK_SIZE_BLOCK)] = GetVoxelTerrain(chunk, noiseData, height);
                }
            }
        }
    }
}
Voxel TerrainGen::GetVoxelTerrain(Chunk*c, float noiseData[3], int y) {
    Voxel voxTemp = { 0, 0};
    int height = (int)(noiseData[0] * 255);
    //range -20 -> 50;
    int temperature =  (int)( noiseData[1] * 70) - 20;
    //int precipitation = (int)(noiseData[1] * 50);
    if (y == height) {
        //dirt
        voxTemp.type = 1;
    }
    else if (y < height) {
        voxTemp.type = 2;//stone
    }

    return voxTemp;
}
