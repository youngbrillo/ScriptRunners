#pragma once
#include "Scene.h"
#include <vector>
#include <lua.hpp>

namespace test { class Node; }
class LuaScene : public Scene
{
public:
	LuaScene(const char* script, const char* startFunction);
	virtual ~LuaScene();

	void loadScript(const char* script, const char* startFunction);
	void unloadScript();

	virtual void Draw();
	virtual void Update(const float& deltaTime);
	void updateObjects(const float& delta);
	virtual void FixedUpdate(const float& timeStep);
	virtual void PollEvents() override;
	virtual void Debug();

	static Scene* Create(const char* script, const char* startFunction = "onSceneStart")
	{
		return new LuaScene(script, startFunction);
	}
	
	static bool CallLuaFunction(lua_State* L, const char* funcName);
	static bool CallLuaFunctionFloat(lua_State* L, const char* funcName, float val);

public:
	lua_State* L;
	std::vector<test::Node*> SceneNodes;
	std::vector<test::Node*> collidingNodes;
	bool canUpdate = true;
	bool canDraw = true;
};