#include "SceneMainMenu.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "CameraManager.h"
#include "ClientEngine.h";
#include "GUI.h"

void SceneMainMenu::init() {
	setupMainMenu();
}
void SceneMainMenu::setupMainMenu() {
	auto res = ResourceManager::GetInstance();
	//setup button main menu
	m_UIMenu.setupEntt(&registry);
	auto contentMainMenu = m_UIMenu.createContainer("mainmenu", { 0 ,0 }, { 800, 600});
	{
		auto spriteButton = res->getSprite("assets/texture/gui/button_0");
		auto button = contentMainMenu->createButton("play");
		button->rect.size = { 220, 60 };
		button->rect.position = { 0, 80 };
		auto img = button->getComponent<Image>();
		img->m_sprite = spriteButton;
		auto text = button->getComponent<UIText>();
		text->text = "Play";
		auto cButtonPlay = button->getComponent<Button>();
		cButtonPlay->m_fnOnClick = []{
			Game::GetInstance()->play();
		};

		button = contentMainMenu->createButton("setting");
		button->rect.size = { 220, 60 };
		img = button->getComponent<Image>();
		img->m_sprite = spriteButton;
		text = button->getComponent<UIText>();
		text->text = "Setting";


		auto buttonExit = contentMainMenu->createButton("exit");
		buttonExit->rect.size = { 220, 60 };
		buttonExit->rect.position = { 0, -80 };
		img = buttonExit->getComponent<Image>();
		img->m_sprite = spriteButton;
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