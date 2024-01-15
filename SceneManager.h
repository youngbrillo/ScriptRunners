#pragma once
#include "Scene2d.h"
#include <vector>
#include <map>

typedef Scene* SceneCreationFunctionNoArgs();
typedef Scene* SceneCreationFunctionString(const char*);

struct SceneEntryPro
{
	const char* category;
	const char* name;
	SceneCreationFunctionNoArgs* creationFunction1 = NULL;
	SceneCreationFunctionString* creationFunction2 = NULL;
	const char* func2Arguments;
};

class GlobalSceneManager
{
public:
	static int RegisterScene(const char* category, const char* name, SceneCreationFunctionNoArgs*);
	static int RegisterScene(const char* category, const char* name, SceneCreationFunctionString*, const char* path);

	static GlobalSceneManager* Get();
private:
	static GlobalSceneManager* Instance;
	GlobalSceneManager(){}
	~GlobalSceneManager() {}

	//private instance level functions
	int AddScene1(const char* category, const char* name, SceneCreationFunction*);
	int AddScene2(const char* category, const char* name, SceneCreationFunctionString*, const char* path);
	int AddScene(const SceneEntryPro& entry);
	//state
	std::vector<SceneEntryPro> entries;
	std::map<const char*, int> entry_name_lookup;
	std::map<int, const char*> entry_name_reverse_lookup;
	std::map<const char*, std::vector<int>> category_entry_lookup;
};

