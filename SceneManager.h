#pragma once
#include "Scene2d.h"
#include <vector>
#include <map>


struct SceneEntryPro
{
	const char* category;
	const char* name;
	const char* path;
	//SceneCreationFunctionString* func = NULL;
	std::function<Scene* (const char*)> func;
	int id = -1;
};

class GlobalSceneManager
{
public:
	static int RegisterScene(const char* category, const char* name, const char* path, std::function<Scene* (const char*)> func);

	static  SceneEntryPro* GetScene(const char* name);
	static  SceneEntryPro* GetScene(const int& id);


	static GlobalSceneManager* Get();


private:
	static GlobalSceneManager* Instance;
	GlobalSceneManager();
	~GlobalSceneManager();

	//private instance level functions
	int AddScene(SceneEntryPro& entry);
	 SceneEntryPro* get_scene(const char* name);
	 SceneEntryPro* get_scene(const int& id);
	//state
	std::map<int, SceneEntryPro> entries;
	std::map<const char*, int> entry_name_lookup;
	std::map<int, const char*> entry_name_reverse_lookup;
	std::map<const char*, std::vector<int>> category_entry_lookup;
	int sceneCount;

public:
	bool Inspect(int& current_scene, const char* title = "Scene Manager");
	
};

