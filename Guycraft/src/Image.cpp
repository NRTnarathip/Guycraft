#include "Image.h"
#include "ResourceManager.h"
#include "GUI.h"
void Image::init() {
	initRenderer();
}
void Image::render() {
	auto res = ResourceManager::GetInstance();
	auto shaderSprite = res->m_shaders["sprite"];
	auto rect = m_uiObject->rect;
	auto model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(rect.getPosition() - rect.getoffs(), 0.f));
	model = glm::scale(model, glm::vec3(rect.getsize() , 1.f));

	shaderSprite->Bind();
	shaderSprite->SetMat4("model", model);
	shaderSprite->SetVec4("color", color);
	draw();
	shaderSprite->UnBind();
}