#pragma once
#include "BlockModel.h"
#include "Block.h"
#include <unordered_map>

class BlockDatabase {
private:
	static BlockDatabase* m_instance;
public:
	BlockDatabase() {
		m_instance = this;
	}
	static BlockDatabase* GetInstance() { return m_instance; }
	std::unordered_map<blocktype_t, BlockModel*> m_models;
	std::unordered_map<std::string, uint16_t> m_texturesTileIndex;

	void init();
	void addModel(BlockModel* model);
	void addModels(std::vector<BlockModel*> models);
};