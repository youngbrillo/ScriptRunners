#pragma once
#include "Scene2d.h"
#include <vector>
#include <map>
#include <string>

struct SceneEntryPro
{
#if false
	const char* category;
	const char* name;
	const char* path;
	//SceneCreationFunctionString* func = NULL;
#else
	std::string category ="uncatergorized";
	std::string name = "unnamed";
	std::string path = "no_path";
#endif
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
	std::map<std::string, int> entry_name_lookup;
	std::map<int, std::string> entry_name_reverse_lookup;
	std::map<std::string, std::vector<int>> category_entry_lookup;
	int sceneCount;

public:
	bool Inspect(int& current_scene, const char* title = "Scene Manager");
	
};

