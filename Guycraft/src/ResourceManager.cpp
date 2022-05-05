#include "ResourceManager.h"
#include "ClientEngine.h"
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>

ResourceManager* ResourceManager::instance = nullptr;

Shader* ResourceManager::addShader(std::string pathVertexAndFragment, std::string shaderName) {
    auto pathVert = pathVertexAndFragment + ".vert";
    auto pathFrag = pathVertexAndFragment + ".frag";
    auto newShader = new Shader(pathVert.c_str(), pathFrag.c_str());
    m_shaders.emplace(shaderName, newShader);
    return newShader;
}

Texture* ResourceManager::addTexture(const char* pathFile, bool isMipmapping) {
    auto texture = new Texture(pathFile, isMipmapping, GL_CLAMP_TO_BORDER, GL_NEAREST);
    m_textures.emplace(pathFile, texture);
    return texture;
}
Sprite* ResourceManager::getSprite(std::string spriteID)
{
    return m_sprites[ "assets/textures/" + spriteID];
}
void ResourceManager::loadAllResouces() {
    namespace fs = std::filesystem;
    //load resouce all shader
    std::string pathShaders = "src/Shaders";
    int isFileExist = 0;
    for (const auto& entry : fs::directory_iterator(pathShaders)) {
        if (isFileExist > 0) {
            isFileExist = 0;
            continue;
        }

        std::string filename = entry.path().string();
        std::string pathFileShader = entry.path().string();

        filename.replace(0, pathShaders.length()+1, "");
        filename = filename.substr(0, filename.length() - 5);

        std::replace(pathFileShader.begin(), pathFileShader.end(), '\\', '/');
        pathFileShader = pathFileShader.substr(0, pathFileShader.length() - 5);
        addShader(pathFileShader, filename);
        isFileExist++;
    }
    //auto defaultShader = addShader("src/Shader/defualt", "default");
    //addShader("src/Shader/gameobject", "gameobject");
    //auto shaderRender2D = addShader("src/Shader/sprite", "sprite");
    auto& graphicSetting = ClientEngine::GetInstance().graphicSetting;
   /* defaultShader->Bind();
    defaultShader->SetVar("tex", 0);
    defaultShader->SetFloat("aoStrength", 0.45f);
    defaultShader->SetFloat("fogMin", graphicSetting.fogMin);
    defaultShader->SetFloat("fogMax", graphicSetting.fogMax);
    defaultShader->UnBind();*/
    std::string pathTextureGUI = "assets/textures/gui";
    for (const auto& entry : fs::directory_iterator(pathTextureGUI)) {
        std::string spriteName = entry.path().string();

        spriteName.replace(0, pathTextureGUI.length() + 1, "");
        spriteName = spriteName.substr(0, spriteName.length() - 4);

        std::string pathFile = entry.path().string();
        std::replace(pathFile.begin(), pathFile.end(), '\\', '/');
        auto texture = addTexture(pathFile.c_str(), false);

        auto spriteKeyName = pathTextureGUI + "/" + spriteName;

        auto newSprite = new Sprite(texture);
        newSprite->setupRenderData();
        m_sprites.emplace(spriteKeyName, newSprite);
    }
}