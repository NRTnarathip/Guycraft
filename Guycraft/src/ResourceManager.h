#pragma once
#include "Renderer/shaderClass.h"
#include <vector>
#include <unordered_map>
#include <string>
#include "Sprite.h"
#include <TextureAtlas.h>

class ResourceManager {
private:
	static ResourceManager* instance;
public:
    ResourceManager() {
        instance = this;
    }
	static ResourceManager* GetInstance() { return instance; }
    std::unordered_map<std::string, Shader*> m_shaders;
    std::unordered_map<std::string, Sprite*> m_sprites;
    std::unordered_map<std::string, Texture*> m_textures;
    std::unordered_map<std::string, TextureAtlas*> m_textureAtlas;

    Texture* addTexture(const char* pathFile, bool isMipmapping);
    Shader* addShader(std::string pathVertexAndFragment, std::string shaderName);
    Sprite* getSprite(std::string spriteID);
    void loadAllResouces();
};