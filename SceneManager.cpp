#include "SceneManager.h"

GlobalSceneManager* GlobalSceneManager::Instance = NULL;

int GlobalSceneManager::RegisterScene(const char* category, const char* name, SceneCreationFunctionNoArgs* func)
{
	return Get()->AddScene1(category, name, func);
}

int GlobalSceneManager::RegisterScene(const char* category, const char* name, SceneCreationFunctionString* func, const char* path)
{
	return Get()->AddScene2(category, name, func, path);
}

GlobalSceneManager* GlobalSceneManager::Get()
{
	if (Instance == NULL)
	{
		Instance = new GlobalSceneManager();
	}
	return Instance;
}

int GlobalSceneManager::AddScene1(const char* category, const char* name, SceneCreationFunction* func)
{
	SceneEntryPro entry{ category, name, func, NULL, ""};
	return AddScene(entry);
}
int GlobalSceneManager::AddScene2(const char* category, const char* name, SceneCreationFunctionString* func, const char* path)
{
	SceneEntryPro entry{ category, name, NULL, func, path };
	return AddScene(entry);
}
int GlobalSceneManager::AddScene(const SceneEntryPro& entry)
{
	entries.push_back(entry);
	int id = entries.size() - 1;
	entry_name_lookup[entry.name] = id;
	entry_name_reverse_lookup[id] = entry.name;
	category_entry_lookup[entry.category].push_back(id);

	return id;
}
