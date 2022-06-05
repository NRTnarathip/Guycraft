#include "BlockModel.h"
#include "BlockDatabase.h"
#include "ResourceManager.h"

void BlockModel::addTexture(FaceDirection dir, std::string textureID) {
	auto uv = ResourceManager::GetInstance()->m_textureAtlas["chunk"]->m_textures[textureID];
	BlockTexture blockTexture;
	blockTexture.uv = { uv.start, uv.end };
	blockTexture.dir = dir;
	blockTexture.textureID = textureID;

	m_textures.push_back(blockTexture);
}