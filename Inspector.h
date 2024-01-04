#pragma once
#include <vector>
class AppSettings;
struct SceneManager;
struct GameObjectManager;
class Inspector
{
public:
	Inspector();
	~Inspector();
	void Render(AppSettings* settings, SceneManager* sm, GameObjectManager* gom );
	void poll();
private:
	void InspectWindows(AppSettings* settings);
	void InspectScenes(SceneManager* SceneManager, AppSettings* settings);
	void InspectObjects(GameObjectManager* objectManager);
	void InspectGameObject(unsigned int gameObject);
public:
	bool menu_visible = true;
	bool view_demo = false;
	std::vector<unsigned int> gameObjects;
};

