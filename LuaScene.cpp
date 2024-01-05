
#include "LuaScene.h"
#include <lua.hpp>
#include <vector>
#include <LuaBridge/LuaBridge.h>
#include "RenderPipeline.h"
#include "SimpleComponents.h"
#include "SimpleNode.h"
#include "GlobalManager.h"

LuaScene* LuaScene::Instance = NULL;

LuaScene::LuaScene(const char* script, const char* startFunction)
	: Scene()
	, L(NULL)
	, SceneNodes({})
	, collidingNodes({})
{
	GlobalManager::Set();
	test::Node::instanceContainer = &this->SceneNodes;
	test::Node::collisionContainer = &this->collidingNodes;
	LuaScene::Instance = this;
	loadScript(script, startFunction);
}
LuaScene::~LuaScene()
{
	unloadScript();
	lua_close(L);
	LuaScene::Instance = NULL;
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
void LuaScene::loadScript(const char* script, const char* startFunction)
{
	if (L != NULL) {
		unloadScript();
		lua_close(L);
		L = NULL;
	}
	L = luaL_newstate();
	luaL_openlibs(L);
	int rv = luaL_dofile(L, script);
	if (rv == LUA_OK)
	{
		test::ExtendAll(L);
		luabridge::getGlobalNamespace(L)
			.beginNamespace("Scene")
			.addCFunction("Inspect", assignInspector)
			.endNamespace();

		LuaScene::CallLuaFunction(L, startFunction);

	}
	else {
		printf("LuaScripted Scene found an Error on load:\n\t%s\n", lua_tostring(L, -1));

	}
}
void LuaScene::unloadScript()
{
	LuaScene::CallLuaFunction(L, "onSceneEnd");
}
void LuaScene::Draw()
{

	//GlobalManager::Draw();
	for (auto&& object : SceneNodes) object->Draw();
	GlobalManager::CanvasDraw();
	if(canDraw)
		canDraw = LuaScene::CallLuaFunction(L, "Draw");

}
void LuaScene::Update(const float& deltaTime)
{
	updateObjects(deltaTime);
	GlobalManager::Update(deltaTime);
}

void LuaScene::updateObjects(const float& deltaTime)
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
}

void LuaScene::FixedUpdate(const float& timeStep)
{
	for (auto&& object : SceneNodes) object->FixedUpdate(timeStep);
	if(canUpdate)
		canUpdate = CallLuaFunctionFloat(L, "Update", timeStep);


}
void LuaScene::PollEvents()
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
void LuaScene::Debug()
{
	if (ImGui::TreeNode(TextFormat("Game Objects: %d", SceneNodes.size())))
	{
		for (auto&& object : SceneNodes) object->Debug(true);
		ImGui::TreePop();
	}

	for (auto&& n : m_inspectors)
	{
		n.second->Inspect(L);
	}

}

bool LuaScene::CallLuaFunction(lua_State* L, const char* funcName)
{
	luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
	try {
		func();
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}

bool LuaScene::CallLuaFunctionFloat(lua_State* L, const char* funcName, float val)
{
	luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
	try {
		func(val);
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}
#include "App.h"

namespace luaVar
{
	struct ITable : public luaVar::iVar
	{
		ITable() : iVar(),key("Empty") {};
		ITable(const char* k) : iVar(), key(k) {};
		const char* key;
		std::vector<std::shared_ptr<luaVar::iVar>> mFields;

		virtual void Inspect(lua_State* L)
		{
			if (ImGui::TreeNode(key))
			{
				for (auto&& field : mFields)
				{
					field->Inspect(L);
				}
				ImGui::TreePop();
			}
		}
	};

}

std::shared_ptr<luaVar::iVar> GetVariable(lua_State* L, const char* key, luabridge::LuaRef ref)
{
	std::shared_ptr<luaVar::iVar> q;

	if (ref.isBool())
	{
		bool v = ref.cast<bool>();
		q = std::make_shared<luaVar::Ibool>(key, v);
	}
	else if (ref.isNumber())
	{
		float v = ref.cast<float>();
		 q = std::make_shared<luaVar::INumber>(key, v);
	}
	else if (ref.isString())
	{
		const char* v = ref.cast<const char*>();
		q = std::make_shared<luaVar::IString>(key, v);
	}
	else if (ref.isTable())
	{
		std::shared_ptr<luaVar::ITable> qq = std::make_shared<luaVar::ITable>(key);

		luabridge::Iterator i = luabridge::Iterator(ref);
		while (!i.isNil())
		{
			const char* k = i.key().cast<const char*>();

			std::shared_ptr<luaVar::iVar> j = GetVariable(L, k, i.value());
			qq->mFields.emplace_back(j);
			++i;

			q = qq;
		}


	}
	return q;
}


int LuaScene::assignInspector(lua_State* L)
{
	int top = lua_gettop(L); int count = 1;
	LuaScene* scene = LuaScene::Instance;

	while (count <= top)
	{
		const char* key = lua_tostring(L, count++);
		luabridge::LuaRef ref = luabridge::getGlobal(L, key);
		scene->m_inspectors[key] = GetVariable(L, key, ref);
	}

	return 0;
}


static Scene* TemplateScene() { return LuaScene::Create("Assets/Scripts/TemplateScene.lua", "onSceneStart"); }
static Scene* BreakoutLuaScene() { return LuaScene::Create("Scripts/BreakoutScene.lua"); }


static int scene000 = RegisterScene("Template: Lua", "Lua: Draw", TemplateScene);
static int scene001 = RegisterScene("Template: Lua", "Lua: Breakout", BreakoutLuaScene);
