#pragma once
#ifndef GUI_H
#define GUI_H
#include <string>
#include <map>
#include "UIMenu.h"
#include <glm/glm.hpp>
#include "TextRenderer.h"

class GUI {
private:
	static GUI* m_instance;
public:
	static GUI& GetInstance() { return *m_instance; }
	static void Begin() {};
	static void End() {};
public:
	GLFWwindow* window = nullptr;
	GUI(GLFWwindow* window);
	glm::mat4 projection;
	void update();
	glm::vec2 getWindowSize();
};
#endif