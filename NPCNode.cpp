#include "NPCNode.h"
#include <LuaBridge/LuaBridge.h>
#include "TextureManager.h"

ECS::KeyInput ECS::NPCNode::interact_key = ECS::KeyInput{KEY_E};


ECS::NPCNode::NPCNode(const char* name, const char* alias, const char* iconAlias, const char* script)
	: Sprite2d(name, alias)
	, approached(false)
{
	icon.texture = TextureManager::Instance()->GetTexture(iconAlias);
	icon.frame = Rectangle{ 0, 0, 1.0f * icon.texture.width, 1.0f * icon.texture.height };
}

ECS::NPCNode::~NPCNode()
{
}

void ECS::NPCNode::Update(const float& dt)
{
	Sprite2d::Update(dt);
	if (this->approached && interact_key.just_pressed)
	{
		doInteract();
	}
}

void ECS::NPCNode::Draw()
{
	Sprite2d::Draw();

	if (approached)
	{

		Rectangle destination = icon.destination;
		destination.x = transform.position.x + icon.destination.x;
		destination.y = transform.position.y + icon.destination.y;

		DrawTexturePro(
			icon.texture,
			icon.frame,
			destination,
			icon.origin,
			icon.rotation,
			icon.color
		);
	}
}

void ECS::NPCNode::Poll()
{
	interact_key.Poll();
}

void ECS::NPCNode::BeginContact(b2Contact* contact, ECS::Node2d* other)
{
	if (other != ECS::NPCNode::prompter) return;
	approached = true;
}

void ECS::NPCNode::EndContact(b2Contact* contact, ECS::Node2d* other)
{
	if (other != ECS::NPCNode::prompter) return;
	approached = false;
}

void ECS::NPCNode::inspect()
{
	Sprite2d::inspect();
	icon.Inspect();
	ImGui::Checkbox("Approached", &approached);
}

void ECS::NPCNode::doInteract()
{
	//get the scene

	//call a function

	//call it a day

	//but for now
	printf("HEY, I'm not hooked up to anything yet...\n");
}

void ECS::NPCNode::setIconFrame(float x, float y, float w, float h)
{
	icon.frame = Rectangle{ x,y,w,h };
}

void ECS::NPCNode::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("ECS")
		.deriveClass<ECS::NPCNode, ECS::Sprite2d>("NPCNode")
			.addData("icon", &ECS::NPCNode::icon)
			.addData("prompter", &ECS::NPCNode::prompter)
		.endClass()
		.endNamespace();
}
