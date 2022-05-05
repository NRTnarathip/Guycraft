#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//is child from class ClientEngine;
class Window {
private:
    GLFWimage icon;
public:
    static Window* m_instance;
    static Window* GetInstance();
public:
    Window(const char* newTitle);
    GLFWwindow* window = nullptr;
    void onWindowResize(int width, int height);
    void setupRender3D();
    void clearBuffer();
    unsigned int width = 1280;
    unsigned int height = 720;
    const char* title = "Voxel Engine";
    //set vsync
    void setActiveVsync(bool state);
    void setIcon(GLFWimage image);
};