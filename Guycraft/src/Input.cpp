#include "Input.h"
#include <iostream>
#include "ClientEngine.h"
#define KEYBOARD 0
#define MOUSE 1
#define MOUSE_DOWN 1
#define MOUSE_UP 3
#define BUTTON_DOWN 1
#define BUTTON_UP 3

Input *Input::instance = nullptr;
void Input::initKeyMapping() {
    registerKeyState(GLFW_MOUSE_BUTTON_LEFT, MOUSE);
    registerKeyState(GLFW_MOUSE_BUTTON_RIGHT, MOUSE);
    registerKeyState(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE);
    registerKeyState(GLFW_KEY_R, KEYBOARD);
}
void Input::registerKeyState(int glfwKeycode, int device) {
    std::unordered_map<int, int> *m_refMapping = nullptr;
    std::pair<int, int> pairKeyValueKeyMapping(glfwKeycode, 0);
    if (device == 0) {
        m_refMapping = &m_keyboardKeyState;
    }
    else {
        m_refMapping = &m_mouseKeyState;
    }
    if (m_refMapping->find(glfwKeycode) != m_refMapping->end()) {
        printf("can't register key mapping\n");
        return;
    }
    m_refMapping->insert(pairKeyValueKeyMapping);
}
void Input::update() {
    // Take care of all GLFW events
    glfwPollEvents();

    auto* window = ClientEngine::GetInstance().window->window;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    lastCursor = nowCursor;
    nowCursor.x = xpos;
    nowCursor.y = ypos;
    for (auto& item : m_mouseKeyState) {
        int state = item.second;
        int glfwState = glfwGetMouseButton(window, item.first);
        if (glfwState == GLFW_PRESS and state < 2) {
            state++;
        }
        else if (glfwState == 0 and state >= 2) {
            state++;
            if (state == 4) {
                state = 0;
            }
        }
        item.second = state;
    }
    for (auto& item : m_keyboardKeyState) {
        int state = item.second;
        int glfwState = glfwGetKey(window, item.first);
        if (glfwState == GLFW_PRESS and state < 2) {
            state++;
        }
        else if (glfwState == 0 and state >= 2) {
            state++;
            if (state == 4) {
                state = 0;
            }
        }
        item.second = state;
    }
}
void Input::setMouseMode(int mode) {
    if (mode == 0) {
        glfwSetInputMode(ClientEngine::GetInstance().window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (mode == 1) {
        glfwSetInputMode(ClientEngine::GetInstance().window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}
glm::vec2 Input::mouseAxis() {
    auto* win = ClientEngine::GetInstance().window->window;
    int width, height;
    glfwGetWindowSize(win, &width,&height);
    auto offs = nowCursor - lastCursor;
    auto axis = glm::vec2(offs.x, offs.y);
    return axis;
}

bool Input::onMouseDown(int keycode) {
    return m_mouseKeyState[keycode] == MOUSE_DOWN;
}
bool Input::onMouse(int keycode) {
    auto state = m_mouseKeyState[keycode];
    return state > 0 and state != MOUSE_UP;
}
bool Input::onMouseUp(int keycode) {
    return m_mouseKeyState[keycode] == MOUSE_UP;
}
glm::vec2 Input::getMousePosition() {
    double x, y;
    glfwGetCursorPos(glfwWindow, &x, &y);
    int winWidth, winHeight;
    glfwGetWindowSize(glfwWindow, &winWidth, &winHeight);
    return glm::vec2(x, winHeight - y);
}
bool Input::isKey(int keycode) {
    auto state = m_keyboardKeyState[keycode];
    return state > 0 and state != BUTTON_UP;
}
bool Input::isKeyDown(int keycode) {
    auto state = m_keyboardKeyState[keycode];
    return state == BUTTON_DOWN;
}
bool Input::isKeyUp(int keycode) {
    auto state = m_keyboardKeyState[keycode];
    return state == BUTTON_UP;
}

