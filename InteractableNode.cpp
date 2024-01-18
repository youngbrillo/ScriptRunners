#include "InteractableNode.h"
#include <LuaBridge/LuaBridge.h>
#include "TextureManager.h"

ECS::InteractableNode::InteractableNode(std::string name)
	: Node2d(name)
	, observer(NULL)
	, isInteractive(false)
{
}

ECS::InteractableNode::~InteractableNode()
{
	this->observer = NULL;
}

void ECS::InteractableNode::Draw()
{
	Node2d::Draw();
	if (isInteractive)
	{
		DrawTexturePro(
			icon.texture,
			icon.frame,
			getIconTrueDestination(),
			icon.origin,
			icon.rotation,
			icon.color
		);
	}

}

void ECS::InteractableNode::Update(const float& deltaTime)
{
	Node2d::Update(deltaTime);
}

void ECS::InteractableNode::BeginContact(b2Contact* contact, ECS::Node2d* other)
{
	if (other != observer) return;
	isInteractive = true;
}

void ECS::InteractableNode::EndContact(b2Contact* contact, ECS::Node2d* other)
{
	if (other != observer) return;
	isInteractive = false;
}

void ECS::InteractableNode::inspect()
{
	Node2d::inspect();
	ImGui::Checkbox("active", &isInteractive);
	icon.Inspect();
}

void ECS::Icon::Inspect(const char* title)
{
	if (ImGui::TreeNode(title))
	{

		int tid = texture.id;
		if (ImGui::InputInt("textureID", &tid)) texture.id = tid;
		ImGui::InputFloat4("frame", &frame.x);
		ImGui::InputFloat4("destination", &destination.x);
		ImGui::InputFloat4("origin", &origin.x);
		ImGui::SliderFloat("rotation", &rotation, -360.0f, 360.0f);

		Vector4 a = ColorNormalize(color);
		if (ImGui::ColorEdit4("Color", &a.x))
		{
			color = ColorFromNormalized(a);
		}

		ImGui::TreePop();
	}
}
void ECS::InteractableNode::setObserver(ECS::Node2d* t)
{
	observer = t;
}

void ECS::InteractableNode::setIconTexture(const char* alias)
{
	icon.texture = TextureManager::Instance()->GetTexture(alias);
	icon.frame = Rectangle{ 0, 0, 1.0f * icon.texture.width, 1.0f * icon.texture.height };

}

void ECS::InteractableNode::setIconDestination(float x, float y, float w, float h)
{
	icon.destination = Rectangle{ x,y,w,h };
}

void ECS::InteractableNode::setIconFrame(float x, float y, float w, float h)
{
	icon.frame = Rectangle{ x,y,w,h };
}

void ECS::InteractableNode::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("ECS")
			.deriveClass<ECS::InteractableNode, ECS::Node2d>("InteractableNode")
				.addFunction("setObserver", &ECS::InteractableNode::setObserver)
				.addFunction("setIconTexture", &ECS::InteractableNode::setIconTexture)
				.addFunction("setIconDestination", &ECS::InteractableNode::setIconDestination)
				.addFunction("setIconFrame", &ECS::InteractableNode::setIconFrame)
				.addData("observer", &ECS::InteractableNode::observer)
			.endClass()
		.endNamespace();
}

Rectangle ECS::InteractableNode::getIconTrueDestination() const
{
	Rectangle r = icon.destination;
	r.x = transform.position.x + icon.destination.x;
	r.y = transform.position.y + icon.destination.y;

	return r;
}
