#include "GUI.h"
#include "ClientEngine.h"
#include "Input.h"

GUI* GUI::m_instance = nullptr;

GUI::GUI(GLFWwindow* window) {
	m_instance = this;
	this->window = window;
}
glm::vec2 GUI::getWindowSize()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return glm::vec2(width, height);
}
void GUI::update() {
	auto winSize = getWindowSize();
	projection = glm::mat4(1.f);
	projection = glm::ortho(0.f, winSize.x, 0.f, winSize.y);
}

