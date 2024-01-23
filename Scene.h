#pragma once
#include <functional>

class Scene
{
public:
	Scene() {};
	virtual ~Scene() {};
	virtual void Initialize() {};
	virtual void Update(const float& deltaTime) = 0;
	virtual void FixedUpdate(const float& timeStep) = 0;
	virtual void Draw() = 0;
	virtual void Debug() {};
	virtual void PollEvents() {};
};

int RegisterScene(const char* category, const char* name, std::function<Scene*(const char*)> func, const char* path = "");
