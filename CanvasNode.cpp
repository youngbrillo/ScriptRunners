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
		mText.fontColor
	);

}

void ECS::CanvasNode::inspect()
{
	Node2d::inspect();
	mText.inspect();
}
#include "Extentions2d.h"
#include "Scene2d.h"

static ECS::CanvasNode* CreateCanvasNode(const char* name)
{
	auto  node = std::make_shared<ECS::CanvasNode>(name);
	Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}
void ECS::CanvasNode::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scene")
			.addFunction("CreateCanvasNode", CreateCanvasNode)
		.endNamespace()
		.beginNamespace("ECS")
			.deriveClass<ECS::CanvasNode, ECS::Node2d>("CanvasNode")
				.addData("mText", &ECS::CanvasNode::mText)
			.endClass()
		.endNamespace();
}

static int kc = ExtensionManager::Register(ECS::CanvasNode::Extend);