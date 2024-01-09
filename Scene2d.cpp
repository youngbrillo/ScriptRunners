#include "Scene2d.h"
#include <imgui_string_wrap_extension.h>
#include <LuaBridge/LuaBridge.h>
#include "App.h"
#include "Extentions2d.h"
#include "TextureManager.h"

Scene2d::Scene2d(const char* path)
	: scriptPath(path)
	, L(NULL)
	, tryUpdate(true)
	, tryDraw(true)
	, tryUIDraw(true)
	, tryPoll(true)
	, world(NULL)
	, b2drawer()
	, boxMouse(NULL)
	, Gravity(0.0f, 10.0f)
{
	world = new b2World(Gravity);
	world->SetDebugDraw(&b2drawer);
	boxMouse = new Box2dMouse(world);

}

Scene2d::~Scene2d()
{
	if (L != NULL) {
		lua_close(L);
	}
	L = NULL;
	Nodes.clear();


	delete world;
	world = NULL;
	delete boxMouse;
	boxMouse = NULL;
}

void Scene2d::Initialize()
{
	this->InitScript(scriptPath);
}

void Scene2d::Update(const float& deltaTime)
{
	boxMouse->Update(deltaTime, world);
	for (auto&& node : Nodes) node->Update(deltaTime);
	removeDeadNodes();
}

void Scene2d::FixedUpdate(const float& timeStep)
{
	world->Step(timeStep, 6, 8);
	boxMouse->FixedUpdate(timeStep, world);

	for (auto&& node : Nodes) node->FixedUpdate(timeStep);
	if (tryUpdate) tryUpdate = ECS::CallLuaFunctionf(L, "Update", timeStep);
}

void Scene2d::Draw()
{
	BeginMode2D(camera.cam);
		for (auto&& node : Nodes) node->Draw();
		if (tryDraw) tryDraw = ECS::CallLuaFunction(L, "Draw");
		world->DebugDraw();
	EndMode2D();
		for (auto&& node : Nodes) node->UIDraw();
		if (tryUIDraw) tryUIDraw = ECS::CallLuaFunction(L, "UIDraw");


}

void Scene2d::Debug()
{
	if (ImGui::SliderFloat2("Gravity", &Gravity.x, -20.0f, 20.0f)) world->SetGravity(Gravity);
	camera.Debug();
	boxMouse->Debug(world);
	b2drawer.Debug();
	if (ImGui::TreeNode("Nodes"))
	{
		for (auto&& node : Nodes) node->Inspect();

		ImGui::TreePop();
	}

	if (ImGui::SliderInt("Node A", &NodeA, -1, Nodes.size()-1, NodeA > 0 ? Nodes[NodeA]->Name : "- Select Node-"))
	{
		if (NodeA > -1)
			Nodes[NodeA]->_inspected = true;
	}

	if (ImGui::SliderInt("Node B", &NodeB, -1, Nodes.size() - 1, NodeB > 0 ? Nodes[NodeB]->Name : "- Select Node-"))
	{
		if(NodeB > -1)
			Nodes[NodeB]->_inspected = true;
	}

	if (NodeA > -1 && Nodes[NodeA]->_inspected)
	{
		DebugNode(Nodes[NodeA].get(), "Node Inspector");
	}

	if (NodeB > -1 && Nodes[NodeB]->_inspected)
	{
		DebugNode(Nodes[NodeB].get(), "Node Inspector (2)");
	}
}

void Scene2d::DebugNode(ECS::Node2d* Node, const char* title)
{
	ImGui::Begin(title, &Node->_inspected);
	ImGui::Text("Name: %s", Node->Name);
	Node->inspect();

	ImGui::End();
}

void Scene2d::PollEvents()
{
	camera.HandleInputs();
	boxMouse->HandleInput(camera.cam, world);

	for (auto&& node : Nodes) node->Poll();
	int key_pressed = GetKeyPressed();
	if (tryPoll && key_pressed != KEY_NULL) tryPoll = ECS::CallLuaFunctioni(L, "onKeyPress", key_pressed);
}

Scene2d* Scene2d::Instance()
{
	Scene2d* m = (Scene2d*) App::GetCurrentScene();
	return m;
}
#include <algorithm>
ECS::Node2d* Scene2d::CreateNode2d(const char* name)
{
	auto n = std::make_shared<ECS::Node2d>(name);
	Nodes.emplace_back(n);
	return Nodes[Nodes.size() - 1].get();
}
ECS::Node2d* Scene2d::iCreateNode2d(const char* name)
{
	return Instance()->CreateNode2d(name);
}
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
		ECS::CallLuaFunction(L, "onSceneEnd");
		lua_close(L);
		L = NULL;
	}

	L = luaL_newstate();
	luaL_openlibs(L);
	int rv = luaL_dofile(L, path);
	if (rv == LUA_OK)
	{
		this->Extend(L);
		ECS::CallLuaFunction(L, "onSceneStart");
	}
	else {
		printf("LuaScripted Scene found an Error on load:\n\t%s\n", lua_tostring(L, -1));
		lua_close(L);
		L = NULL;
	}
}

void Scene2d::Extend(lua_State* L)
{
	auto getCameraFunction = std::function<Camera2D* (void)>([]() {return &Scene2d::Instance()->camera.cam; });
	ECS::ExtendAll(L);
	TextureManager::Extend(L);
	luabridge::getGlobalNamespace(L)
		.beginNamespace("App")
		.addFunction("GetCamera", getCameraFunction)
		.addFunction("Quit", App::QuitApp)
		.addFunction("RestartScene", App::RestartScene)
		.endNamespace()
		.beginNamespace("Scene")
			.addFunction("CreateNode2d", Scene2d::iCreateNode2d)
			.addFunction("GetWorld", GetWorld)
		.endNamespace();
}
