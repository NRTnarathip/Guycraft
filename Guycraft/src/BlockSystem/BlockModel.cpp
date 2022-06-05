#include "BlockModel.h"
#include "BlockDatabase.h"
#include "ResourceManager.h"

void BlockModel::GetBlockTextureUV(BlockModel*model, TextureUV (&uvs)[6])
{
    using FaceDir = FaceDirection;

    auto &atlasTextures = model->m_atlas->m_textures;
    for (auto blockTexture : model->m_textures) {
        auto dir = blockTexture.dir;
        std::string textureID = blockTexture.textureID;
        auto uv = atlasTextures[textureID];

        if (dir == FaceDir::All) {
            uvs[0] = uv;
            uvs[1] = uv;
            uvs[2] = uv;
            uvs[3] = uv;
            uvs[4] = uv;
            uvs[5] = uv;
        }
        else if (dir == FaceDir::Side) {
            uvs[2] = uv;
            uvs[3] = uv;
            uvs[4] = uv;
            uvs[5] = uv;
        }
        else if (dir == FaceDir::Top) {
            uvs[0] = uv;
        }
        else if (dir == FaceDir::Buttom) {
            uvs[1] = uv;
        }
    }
}

void BlockModel::addTexture(FaceDirection dir, std::string textureID) {
	auto uv = ResourceManager::GetInstance()->m_textureAtlas["chunk"]->m_textures[textureID];
	BlockTexture blockTexture;
	blockTexture.uv = { uv.start, uv.end };
	blockTexture.dir = dir;
	blockTexture.textureID = textureID;

	m_textures.push_back(blockTexture);
}