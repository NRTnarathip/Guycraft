#include <Game.h>
#include <ChunkManager.h>
#include <Texture.h>
#include <PlayerController.h>
#include <Mesh.h>
#include <stb/stb_image.h>
#include <ClientEngine.h>
#include "Input.h"
#include <entt/entt.hpp>
#include <CameraManager.h>

#include "Scenes/SceneMainGame.h"
#include "Scenes/SceneMainMenu.h"
#include "GUI.h"



//functin prototype
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//static variable
Game* Game::instance = nullptr;

//class Time
double Time::deltaTime = 0.0;
double Time::lastTime = 0.0;
int Time::fps = 0;
double Time::lastTimeFPS = 0;
float Time::framesPerSecond = 0.f;

Game::Game(Window * win) {
    instance = this;
    window = win;
}
void Game::init() {
    //setup core base all
    sceneManager = new SceneManager();
    sceneManager->init();
    //setup core addond

    //setup scene main menu, game
    auto mainMenu = new SceneMainMenu("Main Menu");
    sceneManager->addExistScene(mainMenu);
    mainMenu->setupMainMenu();
}
void Game::render() {
    sceneManager->render();
}
void Game::printCounter() {
    printf("FPS %d\n", Time::fps);
    printf("Render Triangle Count %d\n", Mesh::triangleGPU);
}
void Game::counterTime() { 
    //update deltatime
    double currentTime = glfwGetTime();
    Time::deltaTime = currentTime - Time::lastTime;
    Time::lastTime = currentTime;
    ++Time::framesPerSecond;

    if (currentTime - Time::lastTimeFPS > 1.0f)
    {
        Time::lastTimeFPS = currentTime;
        Time::fps = (int)Time::framesPerSecond;
        Time::framesPerSecond = 0;

        printCounter();
    }
}
void Game::beforeUpdate() {
    inputDebug();
    sceneManager->beforeUpdate();

}
void Game::inputDebug() {
    int scancode = glfwGetKeyScancode(GLFW_KEY_X);
    int stateReloadTexture = glfwGetKey(window->window, GLFW_KEY_F3);
    if (stateReloadTexture == GLFW_PRESS)
    {
        printf("f3 keydown\n");
    }
}
void Game::lastUpdate() {
    sceneManager->lastUpdate();
}
void Game::update() { //update every frame
    auto glWindow = window->window;
    if (glfwGetKey(glWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        exit();
        return;
    }
    sceneManager->update();
}
void Game::exit() {
    glfwSetWindowShouldClose(window->window, true);
}
//play singleplayer
void Game::play() {
    auto scMainGame = new SceneMainGame("Main Game");
    sceneManager->addExistScene(scMainGame);
    sceneManager->changeScene(1);
    scMainGame->playGame();
}