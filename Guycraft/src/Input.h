#pragma once
#include <glm/vec2.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class Input
{
public:
    Input(GLFWwindow* window) {
        instance = this;
        glfwWindow = window;
    };
    //Singletons should not be cloneable.
    Input(Input& other) = delete;
    //Singletons should not be assignable.
    void operator=(const Input&) = delete;

    static Input& GetInstance() { return *instance; }
    void initKeyMapping();
    void update();
    bool onMouseUp(int mouseButton);
    bool onMouseDown(int mouseButton);
    bool onMouse(int mouseButton);
    void setMouseMode(int mode);//mode 0 == player control, 1 UI control
    GLFWwindow* glfwWindow=nullptr;
    glm::vec2 getMousePosition();
    glm::vec2 mouseAxis();
private:
    void registerKeyState(int glfwKeycode, int device);

    std::unordered_map<int, int> m_keyboardKeyState;
    std::unordered_map<int, int> m_mouseKeyState;

    static Input* instance;
    glm::vec2 lastCursor = glm::vec2(0, 0);
    glm::vec2 nowCursor = glm::vec2(0, 0);
};