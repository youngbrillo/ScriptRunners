#include "Scene2d.h"
#include <imgui_string_wrap_extension.h>
#include <LuaBridge/LuaBridge.h>
#include "App.h"
#include "Extentions2d.h"
#include "TextureManager.h"

Scene2d::Scene2d(const char* path)
	: scriptPath(path)
	, tryUpdate(true)
	, tryDraw(true)
	, tryUIDraw(true)
	, tryPoll(true)
	, world(NULL)
	, b2drawer()
	, boxMouse(NULL)
	, Gravity(0.0f, 10.0f)
	, script()
{
	world = new b2World(Gravity);
	world->SetDebugDraw(&b2drawer);
	boxMouse = new Box2dMouse(world);
	world->SetContactListener(this);
}

Scene2d::~Scene2d()
{
	world->SetContactListener(NULL);
	script.End();
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
	script.Update(timeStep);
}

void Scene2d::Draw()
{
	BeginMode2D(camera.cam);
		for (auto&& node : Nodes) node->Draw();
		script.Draw();
		world->DebugDraw();
	EndMode2D();
		for (auto&& node : Nodes) node->UIDraw();
		script.UIDraw();


}

void Scene2d::Debug()
{
	if (ImGui::SliderFloat2("Gravity", &Gravity.x, -20.0f, 20.0f)) world->SetGravity(Gravity);
	camera.Debug();
	boxMouse->Debug(world);
	b2drawer.Debug();
	script.Inspect();
	if (ImGui::TreeNode("Nodes"))
	{
		for (auto&& node : Nodes) node->Inspect();

		ImGui::TreePop();
	}

	if (ImGui::BeginCombo("Node A", NodeA >= 0 ? Nodes[NodeA]->Name : "- Select a Node -"))
	{
		for (int n = 0; n < Nodes.size(); n++)
		{
			const char* previewValue = n >= 0 ? Nodes[n]->Name : "- Select a Node -";
			const bool is_selected = (NodeA == n);
			if (ImGui::Selectable(previewValue, is_selected))
			{
				NodeA = n;
				Nodes[n]->_inspected = true;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Node B", NodeB >= 0 ? Nodes[NodeB]->Name : "- Select a Node -"))
	{
		for (int n = 0; n < Nodes.size(); n++)
		{
			const char* previewValue = n >= 0 ? Nodes[n]->Name : "- Select a Node -";
			const bool is_selected = (NodeB == n);
			if (ImGui::Selectable(previewValue, is_selected))
			{
				NodeB = n;
				Nodes[n]->_inspected = true;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
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
	script.Poll();
}

//bool Scene2d::validateContact(b2Contact* contact, ECS::Node2d* A, ECS::Node2d* B)
bool Scene2d::validateContact(b2Contact* contact, std::shared_ptr<ECS::Node2d>& A, std::shared_ptr<ECS::Node2d>& B)
{
	bool val = false;
	b2Body* bod_a = contact->GetFixtureA()->GetBody(),
		* bod_b = contact->GetFixtureB()->GetBody();
	for (auto&& node : Nodes)
	{
		if (!node->rigidbody.enabled()) continue;
		if (node->rigidbody.body == bod_a)
		{
			A = node;
		}
		else if (node->rigidbody.body == bod_b)
		{
			B = node;
		}
		if (A != nullptr && B != nullptr)
		{
			val = true;
			break;
		}
	}


	return val;
}

void Scene2d::BeginContact(b2Contact* contact)
{
	std::shared_ptr<ECS::Node2d> A, B;
	if (!validateContact(contact, A, B)) return;
	A->BeginContact(contact, B.get());
	B->BeginContact(contact, A.get());
}

void Scene2d::EndContact(b2Contact* contact)
{
	//ECS::Node2d* A = NULL, * B = NULL;
	std::shared_ptr<ECS::Node2d> A, B;
	if (!validateContact(contact, A, B)) return;
	A->EndContact(contact, B.get());
	B->EndContact(contact, A.get());
}

void Scene2d::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

void Scene2d::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
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
	script.configureScript(path);
	if(script.enabled)
	{
		this->Extend(script.L);
		script.Begin();
	}
}
#include "PlatformerController.h"
#include "Sprite2d.h"
#include "CameraController2d.h"
static ECS::Node2d* CreatePlayerControllerNode(const char* name, const char* alias)
{
	Scene2d::Instance()->Nodes.emplace_back(std::make_shared<ECS::PlatformerController>(name, alias));
	return Scene2d::Instance()->Nodes[Scene2d::Instance()->Nodes.size() -1].get();
}


static ECS::Sprite2d* CreateSprite2dNode(const char* name, const char* alias)
{
	auto  node = std::make_shared<ECS::Sprite2d>(name, alias);
		Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}

static ECS::CameraController2d* CreateCameraController2dNode(const char* name)
{
	auto  node =
	std::make_shared<ECS::CameraController2d>(name);
	Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}

void Scene2d::Extend(lua_State* L)
{
	auto getCameraFunction = std::function<Camera2D* (void)>([]() {return &Scene2d::Instance()->camera.cam; });
	auto isPausedFunction = std::function<bool(void)>([]() {return App::GetState() != AppState_::AppState_Play; });
	ECS::ExtendAll(L);
	TextureManager::Extend(L);
	luabridge::getGlobalNamespace(L)
		.beginNamespace("App")
			.addFunction("GetCamera", getCameraFunction)
			.addFunction("Quit", App::QuitApp)
			.addFunction("RestartScene", App::RestartScene)
			.addFunction("isPaused", isPausedFunction)
			.addFunction("GoToScene", App::GoToScene)
		.endNamespace()
		.beginNamespace("Scene")
			.addFunction("CreateNode2d", Scene2d::iCreateNode2d)
			.addFunction("CreatePlayerController", CreatePlayerControllerNode)
			.addFunction("CreateSprite2d", CreateSprite2dNode)
			.addFunction("CreateCameraController2d", CreateCameraController2dNode)
			.addFunction("GetWorld", GetWorld)
		.endNamespace();

	ECS::Sprite2d::Extend(L);
	ECS::CameraController2d::Extend(L);

}
