#pragma once
#include <Game.h>
#include <GraphicSetting.h>
#include "TextRenderer.h"
#include "GUI.h"

class Input;

class ClientEngine {
private:
	static ClientEngine* instance;
public:
	ClientEngine() {
		instance = this;
	}
	//Singletons should not be cloneable.
	ClientEngine(ClientEngine& other) = delete;
	static ClientEngine& GetInstance() { return *instance; }

	GraphicSetting graphicSetting;
	Game* game = nullptr;
	Window* window = nullptr;
	Input* m_input = nullptr;
	TextRenderer* m_textRenderer = nullptr;
	ResourceManager* m_resouceManager = nullptr;
	SceneManager *m_sceneManager = nullptr;
	GUI* m_gui = nullptr;

	int setupWindow();
	int launch();
	int exit();
};