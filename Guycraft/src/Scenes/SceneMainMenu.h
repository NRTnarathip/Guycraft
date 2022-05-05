#pragma once
#include "Scene.h"

//forward
class Camera;

class SceneMainMenu : public Scene {
public:
	SceneMainMenu(std::string name) : Scene(name) {}
	void setupMainMenu();
	void init();
	void update();
	void render();
};