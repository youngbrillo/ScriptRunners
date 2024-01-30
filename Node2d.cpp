#include "Node2d.h"
#include <imgui_string_wrap_extension.h>
#include <functional>
#include <LuaBridge/LuaBridge.h>

unsigned int ECS::Node2d::Instances = 0;

ECS::Node2d::Node2d(std::string name)
	:Name(name)
	, alive(true)
	, enabled(true)
	, visible(true)
	, speed(1.0f)
	, direction({0.0f, 0.0f})
	, solid(true)
	, textureScale({ 1.0f, 1.0f })
{
	id = Node2d::Instances++;
}

ECS::Node2d::~Node2d()
{
}

void ECS::Node2d::Update(const float& deltaTime)
{
	if (!enabled) return;
	transform.position = Vector2Add(transform.position, Vector2Scale(direction, speed * deltaTime));
	material.Update(deltaTime);
}

void ECS::Node2d::FixedUpdate(const float& timestep)
{
	if (!enabled) return;
	if (!rigidbody.enabled()) return;

	transform.position.x = rigidbody.body->GetPosition().x;
	transform.position.y = rigidbody.body->GetPosition().y;
	transform.rotation = rigidbody.body->GetAngle() * RAD2DEG;

}

void ECS::Node2d::Draw()
{
	if (!visible) return;
	switch (material.shape)
	{
	case ECS::shape_Rectangle:
	{
		Rectangle destination = transform.Destination();
		destination.width *= textureScale.x;
		destination.height *= textureScale.y;

		Vector2 origin = transform.origin;
		origin.x *= textureScale.x;
		origin.y *= textureScale.y;
		DrawTexturePro(
			material.texture,
			material.Source(),
			destination,
			origin,
			transform.rotation,
			material.color
		);
	}
		break;
	case ECS::shape_Circle:
		DrawCircleV(transform.position, transform.size.x, material.color);
		break;
	case ECS::shape_edge:
		DrawLineV(transform.position, transform.size, material.color);
		break;
	default:
		break;
	}
}

void ECS::Node2d::UIDraw()
{
}

void ECS::Node2d::Poll()
{
	if (!enabled) return;
}

void ECS::Node2d::Inspect()
{

	if (ImGui::TreeNode(TextFormat("%03d: %s", this->id, Name.c_str())))
	{
		ImGui::PushItemWidth(250.0f);
		this->inspect();
		ImGui::TreePop();
	}
}

void ECS::Node2d::inspect()
{
	ImGui::Checkbox("alive", &alive);
	ImGui::Checkbox("enabled", &enabled);
	ImGui::Checkbox("visible", &visible);
	ImGui::Checkbox("solid", &solid);

	if (transform.Debug() && rigidbody.enabled())
	{
		rigidbody.SetBody(rigidbody.body->GetWorld(), transform,(int) material.shape);
	}
	material.Debug();
	rigidbody.Debug();

	ImGui::SliderFloat("speed", &speed, 0, 30);
	ImGui::SliderFloat2("direction", &direction.x, -1, 1);
	ImGui::SliderFloat2("textureScale", &textureScale.x, 1, 5);

}

void ECS::Node2d::Extend(lua_State* L)
{
	std::function<void(ECS::Node2d*, float, float)> setPosition = [](ECS::Node2d* n, float x, float y) {
		n->transform.position.x = x;
		n->transform.position.y = y;
		if (n->rigidbody.enabled())
		{
			n->rigidbody.body->SetTransform(b2Vec2{ x, y }, n->rigidbody.body->GetAngle());
		}
	};

	luabridge::getGlobalNamespace(L)
		.beginNamespace("ECS")
			.beginClass<ECS::Node2d>("Node2d")
				.addData("Name", &ECS::Node2d::Name)
				.addData("transform", &ECS::Node2d::transform)
				.addData("material", &ECS::Node2d::material)
				.addData("rigidbody", &ECS::Node2d::rigidbody)
				.addData("alive", &ECS::Node2d::alive)
				.addData("enabled", &ECS::Node2d::enabled)
				.addData("visible", &ECS::Node2d::visible)
				.addData("direction", &ECS::Node2d::direction)
				.addData("speed", &ECS::Node2d::speed)
				.addData("solid", &ECS::Node2d::solid)
				.addData("textureScale", &ECS::Node2d::textureScale)
				.addFunction("setPosition", setPosition)
				.addFunction("GetID", &ECS::Node2d::GetID)
			.endClass()
		.endNamespace();
}
