#pragma once
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//my components
#include <World.h>
#include <Camera.h>
#include <GenerateMeshChunk.h>
#include <ChunkManager.h>
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Window.h"
class Time;

class Game {
private:
    void inputDebug();
    static Game* instance;
public:
    Game(Window* win);
    static Game* GetInstance() { return instance; }
    SceneManager* sceneManager = nullptr;
    Window* window = nullptr;
    ResourceManager* resourceManager = nullptr;

   /* ChunkManager* chManager = nullptr;
    GenMeshChunk* genMeshChunk = nullptr;*/
    void counterTime();
    //order of event functions visualised
    void init();
    void beforeUpdate();//like fixed update
    void update();
    void lastUpdate();
    //end
    void render();
    void printCounter();
    //exit at lobby scene
    void exit();


    //addon
    void play();
};

class Time {
public:
    static double deltaTime;
    static double lastTime;
    static double lastTimeFPS;
    static float framesPerSecond;
    static int fps;
};
#endif // GAME_H

