#include "Scene2d.h"
#include <imgui_string_wrap_extension.h>
#include <LuaBridge/LuaBridge.h>
#include "RayExtend.h"
#include "App.h"

Scene2d::Scene2d(const char* path)
	: scriptPath(path)
	, L(NULL)
	, tryUpdate(true)
	, tryDraw(true)
	, tryUIDraw(true)
	, tryPoll(true)
{
}

Scene2d::~Scene2d()
{
	if (L != NULL) {
		lua_close(L);
	}
	L = NULL;
	Nodes.clear();
}

void Scene2d::Initialize()
{
	this->InitScript(scriptPath);
}

void Scene2d::Update(const float& deltaTime)
{
	for (auto&& node : Nodes) node->Update(deltaTime);
	removeDeadNodes();
}

void Scene2d::FixedUpdate(const float& timeStep)
{
	for (auto&& node : Nodes) node->FixedUpdate(timeStep);
	if (tryUpdate) tryUpdate = Scene2d::CallLuaFunctionf(L, "tryUpdate", timeStep);
}

void Scene2d::Draw()
{
	BeginMode2D(camera.cam);
		for (auto&& node : Nodes) node->Draw();
		if (tryDraw) tryDraw = Scene2d::CallLuaFunction(L, "Draw");

	EndMode2D();
		for (auto&& node : Nodes) node->UIDraw();
		if (tryUIDraw) tryUIDraw = Scene2d::CallLuaFunction(L, "UIDraw");
}

void Scene2d::Debug()
{
	camera.Debug();
	if (ImGui::TreeNode("Nodes"))
	{
		for (auto&& node : Nodes) node->Inspect();

		ImGui::TreePop();
	}
}

void Scene2d::PollEvents()
{
	camera.HandleInputs();
	for (auto&& node : Nodes) node->Poll();
	int key_pressed = GetKeyPressed();
	if (tryPoll && key_pressed != KEY_NULL) tryPoll = Scene2d::CallLuaFunctioni(L, "UIDraw", key_pressed);
}

Scene2d* Scene2d::Instance()
{
	Scene2d* m = (Scene2d*) App::GetCurrentScene();
	return m;
}
#include <algorithm>
void Scene2d::removeDeadNodes()
{
	Nodes.erase(
		std::remove_if(
			Nodes.begin(), 
			Nodes.end(), 
			[](std::shared_ptr<ECS::Node2d> n) {return n->alive == false; }
		),
		Nodes.end()
	);
}

void Scene2d::InitScript(const char* path)
{
	if (L != NULL) {
		Scene2d::CallLuaFunction(L, "onSceneEnd");
		lua_close(L);
		L = NULL;
	}

	L = luaL_newstate();
	luaL_openlibs(L);
	int rv = luaL_dofile(L, path);
	if (rv == LUA_OK)
	{
		ERaylib::Extend(L);
		Scene2d::CallLuaFunction(L, "onSceneStart");
	}

}

bool Scene2d::CallLuaFunction(lua_State* L, const char* funcName)
{
	if (L == NULL) return false;
	try {
		luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
		func();
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}

bool Scene2d::CallLuaFunctionf(lua_State* L, const char* funcName, float v)
{
	if (L == NULL) return false;
	try {
		luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
		func(v);
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}

bool Scene2d::CallLuaFunctioni(lua_State* L, const char* funcName, int v)
{
	if (L == NULL) return false;
	try {
		luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
		func(v);
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}
