#pragma once
#include <iostream>
#include <BlockSystem/Block.h>
#include <BlockSystem/BlockModel.h>
#include <ChunkMarco.h>
#include <MeshChunkSection.h>

class Chunk;
typedef Block blocks_t[CHUNK_SIZE_BLOCK];

class ChunkSection {
public:
	Chunk* chunk = nullptr;
	glm::ivec3 pos;
	uint32_t m_index;

	blocks_t m_blocks = {0, 0};

	MeshChunkSection *m_mesh = nullptr;
	uint16_t m_light[CHUNK_SIZE_BLOCK];
	bool m_isBlocksEmpty = true;
	void changeBlocks(blocks_t blocks);
	void setBlock(uint8_t x, uint8_t y, uint8_t z, Block block);
	void setBlock(int key, Block block);
	void unLoad();
	uint8_t getSunLight(uint8_t x, uint8_t y, uint8_t z);
	void setSunLight(uint8_t x, uint8_t y, uint8_t z);
	uint8_t getTourchLight(uint8_t x, uint8_t y, uint8_t z);
	void setTourchLight(uint8_t x, uint8_t y, uint8_t z);

	void generateMesh();
	void genMeshSolid(Block* block, int8_t x, int8_t y, int8_t z,
		bool useFuncGetOutChunk);
	void genMeshWater(Block* block, int8_t x, int8_t y, int8_t z,
		bool useFuncGetOutChunk);

	//helper for generate mesh
	void makeFaceBlock(MeshChunk* mesh, uint8_t directFace,
		uint8_t(&voxSurr)[27], uint8_t(&lightSurr)[27]);
	void getBlockSurround(uint8_t(&blocks)[27], uint8_t x, uint8_t y, uint8_t z,
		bool useFuncAccessOutChunk);
	void getLightSurround(uint8_t(&lights)[27], uint8_t x, uint8_t y, uint8_t z,
		bool useFuncAccessOutChunk);
	void getAO(uint8_t(&refAO)[4], uint8_t directFace, uint8_t(&blockSurround)[27]);
	void getVertexLightMaping(uint8_t(&refLight)[4], uint8_t directFace, uint8_t(&lightSurround)[27]);
	bool checkBlockIsShape(uint8_t shapeType, int direction,
		int x,int y,int z);
	Block* tryGetBlock(int direction, int x, int y, int z);
	BlockModel* tryGetBlockModel(int direction, int x, int y, int z);
	uint8_t calculateAO(uint8_t side1, uint8_t side2, uint8_t corner);
};