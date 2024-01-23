#include "Scene.h"
#include "SceneManager.h"
//int RegisterScene(const char* category, const char* name, SceneCreationFunction* func)
int RegisterScene(const char* category, const char* name, std::function<Scene* (const char*)> func, const char* path)
{
	return GlobalSceneManager::RegisterScene(category, name, path, func);
}
