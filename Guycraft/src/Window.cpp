#include "Window.h"
#include <iostream>

Window* Window::m_instance = nullptr;

Window::Window(const char* title) {
	m_instance = this;
	this->title = title;
	this->window = window;
	m_isFullscreenMode = false;
}
Window* Window::GetInstance() {
	return m_instance;
}
void Window::onWindowResize(int width,int height) {
	this->width = width;
	this->height = height;
}
bool Window::isFullscreenMode() {
	return m_isFullscreenMode;
}
void Window::setFullscreen(bool v)
{

}
void Window::resize(int width, int height)
{
	glViewport(0, 0, width, height);
	glfwSetWindowSize(window, width, height);
	onWindowResize(width, height);
}
void Window::setActiveVsync(bool state) {
	if (state) {
		glfwSwapInterval(1); // Enable vsync
	}
	else {
		glfwSwapInterval(0); // Enable vsync
	}
}
void Window::clearBuffer() {
	glClearColor(0.f, 0.0f, 0.0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Window::setIcon(GLFWimage image) {
	icon = image;
	glfwSetWindowIcon(window, 1, &icon);
}
void Window::setupRender3D() {
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}