
#include "Scene.h"
#include <lua.hpp>
#include <vector>
#include <LuaBridge/LuaBridge.h>
#include <imgui.h>
#include "RenderPipeline.h"
#include "SimpleComponents.h"
#include "SimpleNode.h"
#include "GlobalManager.h"

class LuaSceneTemplate : public Scene
{
public: 
	lua_State* L;
	//std::vector<std::shared_ptr<test::Node>> SceneNodes;
	std::vector<test::Node*> SceneNodes;
	std::vector<test::Node*> collidingNodes;

public:
	LuaSceneTemplate(const char* scriptPath, const char* startFunctionName)
		: Scene()
		, L(NULL)
		, SceneNodes({})
	{
		GlobalManager::Set();
		test::Node::instanceContainer = &this->SceneNodes;
		test::Node::collisionContainer = &this->collidingNodes;
		LoadScript(scriptPath, startFunctionName);
	}
	~LuaSceneTemplate()
	{
		unloadScript("onSceneEnd");
		lua_close(L);
		L = NULL;
		test::Node::instanceContainer = NULL;
		test::Node::collisionContainer = NULL;
		collidingNodes.clear();

		for (auto&& n : SceneNodes)
		{
			delete n;
			n = NULL;
		}
		SceneNodes.clear();
		GlobalManager::Reset();

	}
	void LoadScript(std::string path, const char* startFunctionName)
	{
		if (L != NULL) {
			unloadScript("onSceneEnd");

			lua_close(L);
			L = NULL;
		}
		L = luaL_newstate();
		luaL_openlibs(L);
		int rv = luaL_dofile(L, path.c_str());
		if (rv == LUA_OK)
		{
			test::ExtendAll(L);
			luabridge::LuaRef func = luabridge::getGlobal(L, startFunctionName);
			try {
				func();
			}
			catch (luabridge::LuaException const& e) {
				printf("%s\n", e.what());
			}
		}
		else {
			printf("LuaScripted Scene found an Error on load:\n\t%s\n", lua_tostring(L, -1));

		}
	}
	void unloadScript(const char* functionName)
	{
		//LSE::GameObject::Extend(L);

		luabridge::LuaRef func = luabridge::getGlobal(L, functionName);
		try {
			func();
		}
		catch (luabridge::LuaException const& e) {
			printf("%s\n", e.what());
		}

	}
	virtual void Draw()
	{
		//GlobalManager::Draw();
		for (auto&& object : SceneNodes) object->Draw();
		GlobalManager::CanvasDraw();
	}
	virtual void Update(const float& deltaTime)
	{

		auto it = SceneNodes.begin();
		while (it != SceneNodes.end())
		{
			auto i = (*it);
			auto jt = collidingNodes.begin();
			while (jt != collidingNodes.end())
			{
				auto j = (*jt);
				if (j->canDestroy) 
					jt = collidingNodes.erase(jt);
				else if (i == j) jt++;
				else
				{
					auto pair = test::CheckCollision(j->transform, j->velocity, i->transform, deltaTime);
					if (pair.first)
					{
						luabridge::LuaRef func = luabridge::getGlobal(L, "onSimpleCollision");
						try {
							func(j, i, pair.second.x, pair.second.y);
						}
						catch (luabridge::LuaException const& e) {
							printf("%s\n", e.what());
						}

					}
					jt++;
				}
				
			}

				
			i->Update(deltaTime);
			if (i->canDestroy) {
				it = SceneNodes.erase(it);
				delete i;
				i = NULL;
			}
			else 
				it++;
		}
		GlobalManager::Update(deltaTime);

	}
	virtual void FixedUpdate(const float& timeStep)
	{
		for (auto&& object : SceneNodes) object->FixedUpdate(timeStep);

	}
	virtual void PollEvents() override
	{
		int key = GetKeyPressed();
		if (key == KEY_NULL) return;

		luabridge::LuaRef func = luabridge::getGlobal(L, "onKeyPress");
		try {
			func(key);
		}
		catch (luabridge::LuaException const& e) {
			printf("%s\n", e.what());
		}


	}
	virtual void Debug()
	{
		if (ImGui::TreeNode(TextFormat("Game Objects: %d", SceneNodes.size())))
		{
			for (auto&& object : SceneNodes) object->Debug(true);
			ImGui::TreePop();
		}

	}

	static Scene* Create() { return new LuaSceneTemplate("Assets/Scripts/TemplateScene.lua", "onSceneStart"); }
};
static int scene000 = RegisterScene("Template", "Lua: box drawing", LuaSceneTemplate::Create);
