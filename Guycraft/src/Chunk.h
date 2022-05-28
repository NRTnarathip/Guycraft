#pragma once

#include <Voxel.h>
#include <MeshChunkVoxelGroup.h>
#include <Renderer/shaderClass.h>
#include <glm/glm.hpp>
#include <mutex>
#include "Types.h"

#define CHUNK_SIZE 16
#define CHUNK_SIZE_INDEX 15
#define CHUNK_SIZE_SQUARED 256
#define CHUNK_SIZE_BLOCK 4096
#define CHUNK_BLOCK_ALL 65536
#define CHUNK_HEIGHT_MAX 256
#define CHUNK_HEIGHT_INDEX 255
#define CHUNK_TOP_HEIGHT 224
#define VOXELGROUP_COUNT 16
#define VOXELGROUP_INDEX 15

struct JobPopulate {
	glm::ivec2 pos;
	//2 byte * 262144;
	Voxel voxels[CHUNK_BLOCK_ALL];
};
struct JobPopulateSub {
	glm::ivec2 pos;
	u8 groupVoxel;
	Voxel voxels[CHUNK_SIZE_BLOCK];
};
//none safe thread
class Chunk {
public:
	bool isShouldUnload = false;
	bool isLoad = false;
	std::mutex mutex;
	std::mutex mutexNeighbor;
	//bitwise light lamp and sun;
	unsigned char lightMap[CHUNK_SIZE_BLOCK];
	glm::ivec2 pos;
	Voxel voxels[CHUNK_BLOCK_ALL];
	MeshChunkVoxelGroup meshs[VOXELGROUP_COUNT];
	//chunk neighbor 8 direction
	Chunk* north = nullptr;
	Chunk* northEast = nullptr;
	Chunk* northWest = nullptr;
	Chunk* south = nullptr;
	Chunk* southEast = nullptr;
	Chunk* southWest = nullptr;
	Chunk* east = nullptr;
	Chunk* west = nullptr;
	std::vector<Chunk*> getAllChunkNeighbor();
	int m_allocateChunkNeighbor = 0;

	void changeVoxels(Voxel voxelsOther[CHUNK_BLOCK_ALL]) {
		for (int i = 0; i < CHUNK_BLOCK_ALL; i++) {
			voxels[i] = voxelsOther[i];
		}
	};
	bool isEmpty();
	u16 getBlockCount();
	//for threading
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
	void render();
	void unload();
	int getHasChunkNeighborCount();
	void linkChunkNeighbor(Chunk* chunkNiehgbor[8]);
	Voxel getvoxel(u8 group, u8 x, u8 y, u8 z);
	//for generate mesh
	void generateMesh(int voxelGroup);
	void genMeshCube(MeshChunk* mesh,u8 groupVoxel, char x, char y, char z, Voxel* voxel,
		bool useFuncitonGetVoxelOutChunk);
	void genMeshWater(MeshChunk* mesh, u8 voxelGroup, char x, char y, char z, Voxel* voxel,
		bool useFuncGetVoxelOutChunk);
	void genMeshStair(MeshChunk* mesh, u8 voxelGroup, int8_t x, int8_t y, int8_t z, Voxel* voxel,
		bool useFunc);
	bool voxelUpIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelDownIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelNorthIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelSouthIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelEastIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelWestIsSolid(u8 group, i8 x, i8 y, i8 z);

	void GetVoxSurround(unsigned char(&args)[27],u8 groupVoxel,unsigned char x, unsigned char y, unsigned char z,
		bool useFuncitonGetVoxelOutChunk);
	unsigned char GetVoxType(u8 groupVoxel,char x, char y, char z);
	void MakeQuadFace(MeshChunk* mesh,unsigned char directFace, unsigned char (&voxSurr)[27], unsigned char (&lightSurr)[27]);
	void GetAO(unsigned char(&refAO)[4], unsigned char directFace, unsigned char(&voxSurr)[27]);
	void GetVertLightMaping(unsigned char(&ref)[4], unsigned char dir, unsigned char(&voxSurr)[27]);
	static unsigned char CalculateAO(unsigned char side1, unsigned char side2, unsigned char corner);
	unsigned char GetLightExtra(int x, int y, int z);
	unsigned char GetSunLight(unsigned short acc);
	unsigned char GetLampLight(unsigned short acc);
	void SetSunLight(unsigned short acc, unsigned char val);
	void SetLampLight(unsigned short acc, unsigned char val);
	static const unsigned char tileCountRow = 16;
	static const float tileSize;
};
