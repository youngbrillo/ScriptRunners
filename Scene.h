#pragma once

class Scene
{
public:
	Scene() {};
	virtual ~Scene() {};
	virtual void Update(const float& deltaTime) = 0;
	virtual void FixedUpdate(const float& timeStep) = 0;
	virtual void Draw() = 0;
	virtual void Debug() {};
	virtual void PollEvents() {};
};


#define MAX_SCENE_COUNT 64

typedef Scene* SceneCreationFunction();
int RegisterScene(const char* category, const char* name, SceneCreationFunction* func);
struct SceneEntry
{
	const char* category;
	const char* name;
	SceneCreationFunction* creationFunction;
};

extern SceneEntry GlobalSceneList[MAX_SCENE_COUNT];
extern int GlobalSceneCount;
