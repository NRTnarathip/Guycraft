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

void TerrainGen::populate(JobPopulate* job) {
    float heightMap[CHUNK_SIZE_SQUARED] = {};
    float moisetureMap[CHUNK_SIZE_SQUARED] = {};
    float heatMap[CHUNK_SIZE_SQUARED] = {};


    float chunkSize = CHUNK_SIZE;
    float scale = .08f;
    auto noise = FastNoise::New<FastNoise::Perlin>();

    auto pos = job->pos;
    //gen all noise map
    for (int k = 0; k < CHUNK_SIZE; k++) {
        int z = k + pos.y;
        float nz = (z / chunkSize) * scale;
        for (int i = 0; i < CHUNK_SIZE; i++) {
            int accMap = i + (k << 4);
            int x = i + pos.x;
            float nx = (x / chunkSize) * scale;

            //gen height map
            float n = genOctave(nx, nz, 3);
            heightMap[accMap] = n;

            //gen moiseture map
            moisetureMap[accMap] = genOctave(nx + 0.05f, nz + 0.025f, 3);
        }
    }
    //place biome block
    for (unsigned char z = 0; z < CHUNK_SIZE; z++) {
        for (unsigned char x = 0; x < CHUNK_SIZE; x++) {
            int accDataMap = x + (z << 4);
            float noiseData[3] = { heightMap[accDataMap],0,0 };

            for (int worldY = 0; worldY < CHUNK_HEIGHT_MAX; worldY++) {
                int y = worldY % CHUNK_SIZE;
                int voxelGroup = worldY / CHUNK_SIZE;
                job->voxels[x+(y<<4)+(z<<8)+(voxelGroup<<12)]=GetVoxelTerrain(noiseData, worldY);
            }
        }
    }
}
Voxel TerrainGen::GetVoxelTerrain(float noiseData[3], int y) {
    Voxel voxel = { 0, 0};
    int height = floor(noiseData[0] * CHUNK_HEIGHT_MAX);
    if (y == height) {
        //dirt
        voxel.type = 1;
    }
    else if (y < height) {
        voxel.type = 2;//stone
    }
    //water
    if (y > height and y <= 64) {
        voxel.type = 4;
    }

    return voxel;
}

float TerrainGen::genOctave(float nx, float nz, int octave)
{
    auto noise = FastNoise::New<FastNoise::Perlin>();

    float n = 0.f;
    int scale = 1;
    float sumScale = 0.f;
    for (u8 i = 0; i < octave; i++) {
        float e = noise->GenSingle2D(nx * scale, nz * scale, seed);
        float subScale = 1 / scale;
        n += ((e + 1.f) / 2.f) * subScale;

        sumScale += subScale;
        scale *= 2;
    }
    return n / sumScale;
}
