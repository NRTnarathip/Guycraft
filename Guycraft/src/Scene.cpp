#include "Scene.h"

Scene::Scene() {
}
Scene::Scene(std::string sceneName) {
	name = sceneName;
}

void Scene::registerRenderWithoutDepth(void (*refFunc)())
{
	m_ptrFuncRenderWithoutDepth.push_back(refFunc);
}
