#pragma once

#include <Voxel.h>
#include <MeshChunk.h>
#include <Renderer/shaderClass.h>
#include <glm/glm.hpp>
#include <mutex>
#include "Types.h"
#define Bitsift_ChunkSize << 5
#define Bitsift_ChunkSizeSquared << 10
#define BS_CH 5 //bitsift to Chunk Size
#define BS_CH2 10 //bitshift to Chunk Size Squared
#define CHUNK_SIZE 32
#define CHUNK_SIZE_INDEX 31
#define CHUNK_SIZE_SQUARED 1024
#define CHUNK_SIZE_BLOCK 32768
#define CHUNK_BLOCK_ALL 262144
#define CHUNK_HEIGHT_MAX 256
#define CHUNK_HEIGHT_INDEX 255
#define CHUNK_TOP_HEIGHT 224
#define VOXEL_COUNT 8
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
	MeshChunk meshs[VOXEL_COUNT];
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
	//for generate mesh
	void generateMeshChunk();
	void genMeshCube(MeshChunk* mesh,u8 groupVoxel, char x, char y, char z, Voxel vox,
		bool useFuncitonGetVoxelOutChunk);

	bool FaceIsVisableUp(u8 groupSet,char i, char j, char k);
	bool FaceIsVisableDown(u8 groupSet, char i, char j, char k);
	bool FaceIsVisableNorth(u8 groupSet, char i, char j, char k);
	bool FaceIsVisableSouth(u8 groupSet, char i, char j, char k);
	bool FaceIsVisableEast(u8 groupSet, char i, char j, char k);
	bool FaceIsVisableWest(u8 groupSet, char i, char j, char k);
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
