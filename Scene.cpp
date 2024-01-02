#include "Scene.h"
SceneEntry GlobalSceneList[MAX_SCENE_COUNT] = { {nullptr} };
int GlobalSceneCount = 0;

int RegisterScene(const char* category, const char* name, SceneCreationFunction* func)
{
	int index = GlobalSceneCount;
	if (index < MAX_SCENE_COUNT)
	{
		GlobalSceneList[index] = { category, name, func };
		++GlobalSceneCount;
		return index;
	}
	return -1;
}
