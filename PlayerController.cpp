#include "PlayerController.h"

ECS::PlayerController::PlayerController(const char* name, const char* alias, const char* config)
	: Sprite2d(name, alias)
	, mPlayer(config)
{
}

ECS::PlayerController::~PlayerController()
{
}

void ECS::PlayerController::Update(const float& dt)
{
	Sprite2d::Update(dt);
}

void ECS::PlayerController::FixedUpdate(const float& dt)
{
	Sprite2d::FixedUpdate(dt);
}

void ECS::PlayerController::Draw()
{
	Sprite2d::Draw();
}

void ECS::PlayerController::UIDraw()
{
	Sprite2d::UIDraw();
}

void ECS::PlayerController::Poll()
{
	if (!enabled) return;
	Sprite2d::Poll();
	//mInputs.Poll();
}

void ECS::PlayerController::inspect()
{
	Sprite2d::inspect();
}

void ECS::PlayerController::handleMovement(const float& dt)
{
}

void ECS::PlayerController::handleXmovement(const float& dt)
{
}

void ECS::PlayerController::handleYmovement(const float& dt)
{
}

void ECS::PlayerController::handleAnimState(const float& dt)
{
}

void ECS::PlayerController::handleActions(const float& dt)
{
}

#include "Extentions2d.h"
#include "Scene2d.h"
#include <LuaBridge/LuaBridge.h>

static ECS::PlayerController* CreatePlayerController(const char* name, const char* alias, const char* config)
{
	auto  node = std::make_shared<ECS::PlayerController>(name, alias, config);
	Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}
void ECS::PlayerController::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scene")
			.addFunction("CreateTilemapNode", CreatePlayerController)
		.endNamespace()
		.beginNamespace("ECS")
			.deriveClass<ECS::PlayerController, ECS::Sprite2d>("PlayerController")
		.endClass()
	.endNamespace();
}

//static int kc = ExtensionManager::Register(ECS::PlayerController::Extend);
