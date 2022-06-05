#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//my components
#include <World.h>
#include <Camera.h>
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
    Window* window = nullptr;
    SceneManager* sceneManager = nullptr;
    void counterTime();
    void init();
    void beforeUpdate();
    void update();
    void lastUpdate();
    void render();
    void exit();
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

