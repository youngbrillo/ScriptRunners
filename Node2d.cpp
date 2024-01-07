#include "Node2d.h"
#include <imgui_string_wrap_extension.h>

ECS::Node2d::Node2d(const char* name)
	:Name(name)
	, alive(true)
	, enabled(true)
	, visible(true)
	, speed(1.0f)
	, direction({0.0f, 0.0f})
	, solid(true)
{
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
		DrawTexturePro(
			material.texture,
			material.source,
			transform.Destination(),
			transform.origin,
			transform.rotation,
			material.color
		);		
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

	if (ImGui::TreeNode(Name))
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

}
