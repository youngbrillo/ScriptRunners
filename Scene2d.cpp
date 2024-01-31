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
	, tryBeginContact(true)
	, tryEndContact(true)
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

	target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
	SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use
	drawToTarget = App::Settings().drawToTargetTexture;

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

	UnloadRenderTexture(target);
}

void Scene2d::Initialize()
{
	this->InitScript(scriptPath);
}

void Scene2d::Update(const float& deltaTime)
{
	boxMouse->Update(deltaTime, world);
	for (auto&& node : Nodes) node->Update(deltaTime);
}

void Scene2d::FixedUpdate(const float& timeStep)
{
	removeDeadNodes();
	world->Step(timeStep, 6, 8);
	boxMouse->FixedUpdate(timeStep, world);

	for (auto&& node : Nodes) node->FixedUpdate(timeStep);
	script.Update(timeStep);
}

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))
void Scene2d::Draw()
{
	if (drawToTarget)
		DrawToResolution();
	else
		DrawContent();
}

void Scene2d::DrawContent()
{
	BeginMode2D(camera.cam);
	for (auto&& node : Nodes) node->Draw();
	script.Draw();
	world->DebugDraw();
	EndMode2D();
	for (auto&& node : Nodes) node->UIDraw();
	script.UIDraw();
}

void Scene2d::DrawToResolution()
{
	float scale = MIN((float)GetScreenWidth() / sceneScreenWidth, (float)GetScreenHeight() / sceneScreenHeight);
	BeginTextureMode(target);
		ClearBackground(BLACK);
		DrawContent();
	EndTextureMode();


	Rectangle source = Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
	Rectangle destRec = Rectangle{
			(GetScreenWidth() - (sceneScreenWidth * scale)) * 0.5f,
			(GetScreenHeight() - (sceneScreenHeight * scale)) * 0.5f,
			sceneScreenWidth * scale,
			sceneScreenHeight * scale
	};

	//scale = (1.0f * sceneScreenWidth) / (1.0f * sceneScreenHeight);
	//float virtualRatio = (float)GetScreenWidth() / (float)sceneScreenWidth;
	//destRec = { -virtualRatio , -virtualRatio , GetScreenWidth() + (virtualRatio * 2), GetScreenHeight() + (virtualRatio * 2) };
	DrawTexturePro(target.texture, source ,destRec ,Vector2{ 0, 0 }, 0.0f, WHITE);
}

void Scene2d::Debug()
{
	if (ImGui::SliderFloat2("Gravity", &Gravity.x, -20.0f, 20.0f)) world->SetGravity(Gravity);
	camera.Debug();
	boxMouse->Debug(world);
	b2drawer.Debug();
	script.Inspect();
	if (ImGui::Checkbox("Draw to Target", &drawToTarget))
	{
		App::Settings().drawToTargetTexture = this->drawToTarget;
	}
	ImGui::SliderInt("resolution x", &sceneScreenWidth, 10, GetScreenWidth());
	ImGui::SliderInt("resolution y", &sceneScreenHeight, 10, GetScreenHeight());


	if (ImGui::BeginCombo("Node A", NodeA >= 0 ? Nodes[NodeA]->Name.c_str() : "- Select a Node -"))
	{
		for (int n = 0; n < Nodes.size(); n++)
		{
			const char* previewValue = n >= 0 ? Nodes[n]->Name.c_str() : "- Select a Node -";
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

	if (ImGui::BeginCombo("Node B", NodeB >= 0 ? Nodes[NodeB]->Name.c_str() : "- Select a Node -"))
	{
		for (int n = 0; n < Nodes.size(); n++)
		{
			const char* previewValue = n >= 0 ? Nodes[n]->Name.c_str() : "- Select a Node -";
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

	if (ImGui::TreeNode("Nodes"))
	{
		for (auto&& node : Nodes) node->Inspect();

		ImGui::TreePop();
	} 
}

void Scene2d::DebugNode(ECS::Node2d* Node, const char* title)
{
	ImGui::Begin(title, &Node->_inspected);
	ImGui::Text("Name: %s", Node->Name.c_str());
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
		if (node == nullptr) 
			continue;
		if (!node->rigidbody.enabled()) 
			continue;
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

	if (tryBeginContact && script.enabled)
	{
		try {
			luabridge::LuaRef func = luabridge::getGlobal(script.L, "onBeginContact");
			func(A.get(), B.get());
		}
		catch (luabridge::LuaException const& e) {
			printf("error in '%s'\t%s\n", "onBeginContact", e.what());
			tryBeginContact = false;
		}
	}
}

void Scene2d::EndContact(b2Contact* contact)
{
	//ECS::Node2d* A = NULL, * B = NULL;
	std::shared_ptr<ECS::Node2d> A, B;
	if (!validateContact(contact, A, B)) return;
	A->EndContact(contact, B.get());
	B->EndContact(contact, A.get());


	if (tryEndContact && script.enabled)
	{
		try {
			luabridge::LuaRef func = luabridge::getGlobal(script.L, "onEndContact");
			func(A.get(), B.get());
		}
		catch (luabridge::LuaException const& e) {
			printf("error in '%s'\t%s\n", "onEndContact", e.what());
			tryEndContact = false;
		}
	}
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
	//Nodes.erase(
	//	std::remove_if(
	//		Nodes.begin(), 
	//		Nodes.end(), 
	//		[](std::shared_ptr<ECS::Node2d> n) {return n->alive == false; }
	//	),
	//	Nodes.end()
	//);


	auto it = Nodes.begin();
	while (it != Nodes.end())
	{
		if (it->get()->alive == false)
		{
			if (NodeA >= Nodes.size() - 2)
				NodeA = -1;
			if (NodeB >= Nodes.size() - 2)
				NodeB = -1;
			it = Nodes.erase(it);
			
		}
		else it++;
	}

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
#include "InteractableNode.h"
#include "NPCNode.h"

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

static ECS::InteractableNode* CreateInteractableNode(const char* name)
{
	auto  node =
		std::make_shared<ECS::InteractableNode>(name);
	Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}
static ECS::NPCNode* CreateNPCNode(const char* name, const char* alias, const char* iconAlias, const char* scriptPath)
{
	auto  node =
		std::make_shared<ECS::NPCNode>(name, alias, iconAlias, scriptPath);
	Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}





#include "FontManager.h"
void Scene2d::Extend(lua_State* L)
{
	auto getCameraFunction = std::function<Camera2D* (void)>([]() {return &Scene2d::Instance()->camera.cam; });
	auto isPausedFunction = std::function<bool(void)>([]() {return App::GetState() != AppState_::AppState_Play; });
	auto setScreenResolution = std::function<void(int, int)>([](int x, int y) { Scene2d::Instance()->sceneScreenWidth = x;Scene2d::Instance()->sceneScreenHeight = y;});
	auto setDrawToTarget = std::function<void(bool)>([](bool v) { Scene2d::Instance()->drawToTarget = v;});

	ECS::ExtendAll(L);
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
			.addFunction("CreateInteractableNode", CreateInteractableNode)
			.addFunction("CreateNPCNode", CreateNPCNode)
			.addFunction("GetWorld", GetWorld)
			.addFunction("SetScreenResolution", setScreenResolution)
			.addFunction("setDrawToTarget", setDrawToTarget)
		.endNamespace();

	ECS::Sprite2d::Extend(L);
	ECS::CameraController2d::Extend(L);
	ECS::InteractableNode::Extend(L);
	ECS::NPCNode::Extend(L);
}
