#pragma once

#include <Voxel.h>
#include <MeshChunk.h>
#include <Renderer/shaderClass.h>
#include <glm/glm.hpp>
#include <mutex>
#include "Types.h"
#define Bitsift_ChunkSize << 5
#define Bitsift_ChunkSizeSquared << 10
//BitShift Chunk Size
#define BS_CH 5 
//BitShift Chunk Size Squared
#define BS_CH2 10 
//BitShift Chunk Size Block
#define BS_CH4 15 
#define CHUNK_SIZE 32
#define CHUNK_SIZE_INDEX 31
#define CHUNK_SIZE_SQUARED 1024
#define CHUNK_SIZE_BLOCK 32768
#define CHUNK_BLOCK_ALL 262144
#define CHUNK_HEIGHT_MAX 256
#define CHUNK_HEIGHT_INDEX 255
#define CHUNK_TOP_HEIGHT 224
#define VOXEL_GROUP_COUNT 8
#define VOXEL_GROUP_INDEX 7
//none safe thread
class Chunk {
private:
	bool isLoad = false;
public:
	std::mutex mutex;
	Chunk(glm::ivec2 pos);
	//bitwise light lamp and sun;
	unsigned char lightMap[CHUNK_SIZE_BLOCK];
	glm::ivec2 pos;
	Voxel voxels[CHUNK_BLOCK_ALL];
	//mesh block solid 8 mesh, block fluid 8 mesh
	MeshChunk meshs[VOXEL_GROUP_COUNT * 2];
	bool isEmpty();
	u16 getBlockCount();
	bool isNeedGenerateMesh = false;
	//for threading
	void lock() { mutex.lock(); }
	void unlock() { mutex.unlock(); }
	Chunk* north = nullptr;
	Chunk* south = nullptr;
	Chunk* east = nullptr;
	Chunk* west = nullptr;
	void render();
	void unload();
	void onLoad();
	void linkChunkNeighbor(Chunk* [4]);
	void unlinkChunkNeighbhor();
	Voxel getvoxel(u8 group, u8 x, u8 y, u8 z);
	//for generate mesh
	void generateMeshChunk();
	void genMeshCube(MeshChunk* mesh,u8 groupVoxel, char x, char y, char z, Voxel vox,
		bool useFuncitonGetVoxelOutChunk);
	void genMeshWater(u8 voxelGroup, char x, char y, char z, Voxel voxel, 
		bool useFuncGetVoxelOutChunk);
	bool voxelUpIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelDownIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelNorthIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelSouthIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelEastIsSolid(u8 group, i8 x, i8 y, i8 z);
	bool voxelWestIsSolid(u8 group, i8 x, i8 y, i8 z);

	void GetVoxSurround(unsigned char(&args)[27],u8 groupVoxel,unsigned char x, unsigned char y, unsigned char z,
		bool useFuncitonGetVoxelOutChunk);
	unsigned char GetVoxType(u8 groupVoxel,char x, char y, char z);
	void MakeQuadFace(MeshChunk* mesh,Voxel voxel, unsigned char directFace, unsigned char (&voxSurr)[27], unsigned char (&lightSurr)[27]);
	void GetAO(unsigned char(&refAO)[4], unsigned char directFace, unsigned char(&voxSurr)[27]);
	void GetVertLightMaping(unsigned char(&ref)[4], unsigned char dir, unsigned char(&voxSurr)[27]);
	static unsigned char CalculateAO(unsigned char side1, unsigned char side2, unsigned char corner);
	unsigned char GetLightExtra(int x, int y, int z);
	unsigned char GetSunLight(unsigned short acc);
	unsigned char GetLampLight(unsigned short acc);
	void SetSunLight(unsigned short acc, unsigned char val);
	void SetLampLight(unsigned short acc, unsigned char val);
	void GetLightingSurround(unsigned char(&refLightMap)[27], int _x, int _y, int _z);
	static const unsigned char tileCountRow = 16;
	static const float tileSize;
};
