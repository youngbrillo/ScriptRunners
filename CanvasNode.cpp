#include "CanvasNode.h"
#include <LuaBridge/LuaBridge.h>
#include <imgui_string_wrap_extension.h>



ECS::CanvasNode::CanvasNode(const char* name)
	: Node2d(name)
{
}

ECS::CanvasNode::~CanvasNode()
{
}

void ECS::Text::update(const float& dt)
{
	if (!visible) return;


	if (cursor >= string.length() && expires)
	{
		timer += dt;
		if (timer >= expirationLimit)
		{
			visible = false;
			timer = 0.0f;
			expires = false;
		}
	}
	else if(cursor < string.length())
	{
		timer += dt;
		if (timer >= writeSpeed)
		{
			timer = 0.0f;
			cursor += inc_size;
		}
	}
}


void ECS::CanvasNode::Update(const float& deltaTime)
{
	if (!this->enabled) return;
	ECS::Node2d::Update(deltaTime);
	mText.update(deltaTime);
}

void ECS::CanvasNode::Draw()
{
}

void ECS::CanvasNode::UIDraw()
{
	if (!mText.visible) return;

	ECS::Node2d::Draw();
	
	DrawTextEx(
		GetFontDefault(),
		TextSubtext(mText.string.c_str(), 0, mText.cursor),
		transform.position,
		mText.fontSize,
		mText.fontSpacing,
		mText.color
	);

}

void ECS::CanvasNode::inspect()
{
	Node2d::inspect();
	mText.inspect();
}

void ECS::Text::inspect(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::Checkbox("visible", &visible);
		ImGui::Checkbox("expires", &expires);

		ImGui::InputText("string", &string);
		ImGui::SliderInt("cursor", &cursor, 0, string.length());
		ImGui::InputInt("increment", &inc_size, 1, 10);
		ImGui::SliderFloat("timer", &timer, 0, expirationLimit);
		ImGui::SliderFloat("writeSpeed", &writeSpeed, 0, 1.0f);
		ImGui::SliderFloat("expirationLimit", &expirationLimit, 1, 30.0f);
		ImGui::SliderFloat("fontSize", &fontSize, 0, 1.0f);
		ImGui::SliderFloat("fontSpacing", &fontSpacing, 0, 1.0f);

		Vector4 a = ColorNormalize(color);
		if (ImGui::ColorEdit4("Color", &a.x))
		{
			color = ColorFromNormalized(a);
		}
		ImGui::TreePop();
	}
}
void ECS::CanvasNode::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("ECS")
		.beginClass<ECS::Text>("Text")
			.addData("string", &ECS::Text::string)
			.addData("cursor", &ECS::Text::cursor)
			.addData("inc_size", &ECS::Text::inc_size)
			.addData("writeSpeed", &ECS::Text::writeSpeed)
			.addData("expirationLimit", &ECS::Text::expirationLimit)
			.addData("visible", &ECS::Text::visible)
			.addData("expires", &ECS::Text::expires)
			.addFunction("setText", &ECS::Text::setText)
		.endClass()
		.deriveClass<ECS::CanvasNode, ECS::Node2d>("CanvasNode")
			.addData("mText", &ECS::CanvasNode::mText)
		.endClass()
		.endNamespace();
}
