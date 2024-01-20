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

void ECS::CanvasNode::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("ECS")
		.deriveClass<ECS::CanvasNode, ECS::Node2d>("CanvasNode")
			.addData("mText", &ECS::CanvasNode::mText)
		.endClass()
		.endNamespace();
}
