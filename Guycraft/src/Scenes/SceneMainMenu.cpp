#include "SceneMainMenu.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "CameraManager.h"
#include "ClientEngine.h";
#include "GUI.h"

void SceneMainMenu::init() {
	//setupMainMenu();
}
void SceneMainMenu::setupMainMenu() {
	auto res = ResourceManager::GetInstance();
	//setup button main menu
	m_UIMenu.setupEntt(&registry);
	auto contentBG = m_UIMenu.createContainer("lobby_background", {960 ,540}, {1920, 1080});
	auto contentMainMenu = m_UIMenu.createContainer("mainmenu", { 0 ,0 }, { 800, 600});
	contentMainMenu->rect.position.y = -140.f;
	{
		auto button = contentMainMenu->createButton("play");
		button->rect.size = { 220, 60 };
		button->rect.position = { 0, 80 };
		auto img = button->getComponent<Image>();
		img->sprite = res->getSprite("gui/button_0");
		auto text = button->getComponent<UIText>();
		text->text = "Play";
		auto cButtonPlay = button->getComponent<Button>();
		cButtonPlay->m_fnOnClick = []{
			Game::GetInstance()->play();
		};

		button = contentMainMenu->createButton("setting");
		button->rect.size = { 220, 60 };
		img = button->getComponent<Image>();
		img->sprite = res->getSprite("gui/button_0");
		text = button->getComponent<UIText>();
		text->text = "Setting";


		auto buttonExit = contentMainMenu->createButton("exit");
		buttonExit->rect.size = { 220, 60 };
		buttonExit->rect.position = { 0, -80 };
		img = buttonExit->getComponent<Image>();
		img->sprite = res->getSprite("gui/button_0");
		text = buttonExit->getComponent<UIText>();
		text->text = "Exit";
		auto cButton = buttonExit->getComponent<Button>();
		cButton->m_fnOnClick = []{
			Game::GetInstance()->exit();
		};
	}
}
void SceneMainMenu::update() {
	
}
void SceneMainMenu::render() {
	
}