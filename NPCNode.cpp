#include "NPCNode.h"
#include <LuaBridge/LuaBridge.h>
#include "TextureManager.h"
#include "functionality2d.h"
#include "Scene2d.h"
#include "FontManager.h"

ECS::KeyInput ECS::NPCNode::interact_key = ECS::KeyInput{KEY_E};


ECS::NPCNode::NPCNode(const char* name, const char* alias, const char* iconAlias, const char* script)
	: Sprite2d(name, alias)
	, approached(false)
	, inInteraction(false)
{
	icon.texture = TextureManager::Instance()->GetTexture(iconAlias);
	icon.frame = Rectangle{ 0, 0, 1.0f * icon.texture.width, 1.0f * icon.texture.height };



	Vector2 position = { 0, (float)GetScreenHeight() * 0.75f };
	Vector2 size = { (float)GetScreenWidth(), (float)GetScreenHeight() - position.y };
	Rectangle rec = Rectangle{
		position.x, position.y,
		size.x , size.y
	};

	text_rec_background = Rectangle{
		10, 
		(float)GetScreenHeight() * 0.75f,
		(float)GetScreenWidth() - 20, 
		(float)GetScreenHeight() - (10+(float)GetScreenHeight() * 0.75f)
	};

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
	if (!isTalking) return;

	if (text.cursor > -1 && text.cursor < text.string.length())
	{
		text.timer += dt;
		if (text.timer >= text.writeSpeed)
		{
			text.timer = 0.0f;
			text.cursor += text.inc_size;
		}
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

void ECS::NPCNode::UIDraw()
{

	if (!canTalk) return;

	float scale = 1.0f;

	if (isTalking)
	{

		Rectangle rec = text_rec_background;
		DrawRectangleRec(rec, text.backgroundColor);


		Rectangle destination = {
			rec.x + rec.width - text.fontSize - 10,
			rec.y + rec.height - text.fontSize - 10,
			text.fontSize * scale, text.fontSize * scale
		};



		rec.x += text_padding.x;
		rec.y += text_padding.y;
		rec.width -= text_padding.width;
		rec.height -= text_padding.height;
#if false
		ECS::DrawWrappedText(
			FontManager::Instance()->getFont(this->text.fontId),
			TextSubtext(text.string.c_str(), 0, text.cursor >= 0 ? text.cursor : text.string.length()),
			rec,
			text.fontSize,
			text.fontSpacing,
			text.fontColor,
			Color{ 0, 0, 0, 0 }
		);
#else
		const char* text_string = TextSubtext(text.string.c_str(), 0, text.cursor >= 0 ? text.cursor : text.string.length());
		Font f = FontManager::Instance()->getFont(this->text.fontId);
		Vector2 dim = MeasureTextEx(f, text_string, text.fontSize, text.fontSpacing);
		
		DrawTextEx(
			f,
			text_string,
			Vector2{
				rec.x + rec.width * 0.5f - (dim.x * 0.5f),
				rec.y + rec.height * 0.5f - (dim.y * 0.5f + text.fontSize *0.5f)
			},
			text.fontSize,
			text.fontSpacing,
			text.fontColor
		);
#endif
		


		DrawTexturePro(
			icon.texture,
			icon.frame,
			destination,
			icon.origin,
			icon.rotation,
			interact_key.isDown ? YELLOW: icon.color
		);

	}
	else
	{
		Vector2 screenPosition = GetWorldToScreen2D(transform.position, Scene2d::Instance()->camera.cam);
		Rectangle destination = {
			screenPosition.x + prompt_offset.x, screenPosition.y + prompt_offset.y,
			icon.destination.width * scale, icon.destination.height * scale
		};
		DrawTexturePro(
			icon.texture,
			icon.frame,
			destination, 
			icon.origin, 
			icon.rotation, 
			interact_key.isDown ? YELLOW : icon.color
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
	if (isTalking || canTalk)
	{
		isTalking = false;
		canTalk = false;
	}
}

void ECS::NPCNode::inspect()
{
	Sprite2d::inspect();
	icon.Inspect();
	ImGui::Checkbox("Approached", &approached);
	ImGui::Checkbox("in Interaction", &inInteraction);
	ImGui::Checkbox("can Talk", &canTalk);
	ImGui::Checkbox("in Talking", &isTalking);
	text.inspect();
}


void ECS::NPCNode::doInteract()
{
	//get the scene
	lua_State* L = Scene2d::Instance()->script.L;
#if false
	this->inInteraction = true;
	if (L == NULL) return;
	try {
		luabridge::LuaRef func = luabridge::getGlobal(L, "HandleNPCInteraction");
		func(this);
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", "HandleNPCInteraction", e.what());
	}
	this->inInteraction = false;
#else
	bool in_range = text.cursor > text.cursor > -1 && text.cursor < text.string.length();

	if (!isTalking)
	{
		isTalking = true;
		canTalk = true;
		luabridge::LuaRef func = luabridge::getGlobal(L, "onDialogueStart");
		try { func(this, this->prompter); }
		catch (luabridge::LuaException const& e) {
			printf("%s\n", e.what());
		}
	}
	else if (isTalking && in_range)
	{
		text.cursor = text.string.length();
	}
	else if (isTalking && !in_range)
	{
		isTalking = false;
		text.cursor = 0;

		luabridge::LuaRef func = luabridge::getGlobal(L, "onDialogueEnd");
		try { func(this, this->prompter); }
		catch (luabridge::LuaException const& e) {
			printf("%s\n", e.what());
		}
	}

#endif
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
			.addData("inInteraction", &ECS::NPCNode::inInteraction)
			.addData("text", &ECS::NPCNode::text)
			.addData("canTalk", &ECS::NPCNode::canTalk)
			.addData("isTalking", &ECS::NPCNode::isTalking)
		.endClass()
		.endNamespace();
}
