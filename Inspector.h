#pragma once
#include <vector>
struct WindowSettings;
struct SceneManager;
struct GameObjectManager;
class Inspector
{
public:
	Inspector();
	~Inspector();
	void Render(WindowSettings* settings, SceneManager* sm, GameObjectManager* gom );
	void poll();
private:
	void InspectWindows(WindowSettings* settings);
	void InspectScenes(SceneManager* SceneManager);
	void InspectObjects(GameObjectManager* objectManager);
	void InspectGameObject(unsigned int gameObject);
public:
	bool menu_visible = true;
	bool view_demo = false;
	std::vector<unsigned int> gameObjects;
};

