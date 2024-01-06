#include "Components2d.h"
#include <imgui_string_wrap_extension.h>
#include "TextureManager.h"

// TRANSFORM //////////////////////////////////////////////
Rectangle ECS::Transform::Destination() const
{
	return Rectangle{ position.x, position.y, size.x, size.y };
}

Rectangle ECS::Transform::Rect() const
{
	return Rectangle{ 
		position.x - origin.x,
		position.y - origin.y,
		size.x, 
		size.y 
	};
}
void ECS::Transform::Center()
{
	this->Align(0.5f, 0.5f);
}
void ECS::Transform::Align(float x, float y)
{
	origin.x = size.x * x;
	origin.y = size.y * y;
}

void ECS::Transform::Debug(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::InputFloat("x", &position.x, 1.0f, 10.0f);
		ImGui::InputFloat("y", &position.y, 1.0f, 10.0f);
		ImGui::InputFloat("width", &size.x, 1.0f, 10.0f);
		ImGui::InputFloat("height", &size.y, 1.0f, 10.0f);
		ImGui::SliderFloat("origin.x", &origin.x, -size.x, size.x);
		ImGui::SliderFloat("origin.y", &origin.y, -size.y, size.y);
		ImGui::SliderFloat("rotation", &rotation, -360, 360);

		ImGui::TreePop();
	}
}

// MATERIAL //////////////////////////////////////////////
void ECS::Material::Update(const float& dt)
{
	source.x += uv_scroll.x * dt;
	source.y += uv_scroll.y * dt;

	if (source.x > texture.width * 2) source.x = texture.width * -2;
	if (source.y > texture.height * 2) source.y = texture.width * -2;
	if (source.x < texture.width * -2) source.x = texture.width * 2;
	if (source.y < texture.height * -2) source.y = texture.width * 2;
}
void ECS::Material::SetTexture(const char* alias)
{
	this->SetTextureT(TextureManager::Instance()->GetTexture(alias));
}
void ECS::Material::SetTextureT(Texture2D t)
{
	texture = t;
	source = Rectangle{ 0, 0, (float)t.width, (float)t.height };
}
void ECS::Material::SetColorH(unsigned int hex)
{
	color = GetColor(hex);
}
void ECS::Material::SetColorV(Vector4 vec)
{
	color = ColorFromNormalized(vec);
}
void ECS::Material::SetColorf(float r, float g, float b, float a)
{
	this->SetColorV(Vector4{ r, g, b, a });
}
static const char* shapeNames[ECS::shape_edge + 1] = { "BOX", "CIRCLE", "EDGE" };

void ECS::Material::Debug(const char* title )
{
	if (ImGui::TreeNode(title))
	{
		Vector4 a = ColorNormalize(color);
		if (ImGui::ColorEdit4("Color", &a.x))
		{
			color = ColorFromNormalized(a);
		}

		int s = shape;
		if (ImGui::SliderInt("Shape", &s, 0, ECS::shape_edge, shapeNames[shape])) shape = (ECS::shape_)s;

		int tid = texture.id;
		if (ImGui::InputInt("textureID", &tid)) texture.id = tid;
		ImGui::SliderFloat2("Uv scroll", &uv_scroll.x, -20, 20.0f);

		ImGui::SliderFloat("x", &source.x, -texture.width, texture.width);
		ImGui::SliderFloat("y", &source.y, -texture.height, texture.height);
		ImGui::SliderFloat("width", &source.width, -texture.width, texture.width);
		ImGui::SliderFloat("height", &source.height, -texture.height, texture.height);
		ImGui::TreePop();
	}
}

// RIGIDBODY //////////////////////////////////////////////


// SCRIPT /////////////////////////////////////////////////

// INPUT /////////////////////////////////////////////////

void ECS::KeyInput::Debug(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::InputInt("Key", &key);
		ImGui::Checkbox("just_pressed", &just_pressed);
		ImGui::Checkbox("just_released", &just_released);
		ImGui::Checkbox("isDown", &isDown);

		ImGui::TreePop();
	}
}

// CAMERA /////////////////////////////////////////////////

ECS::Camera2d::Camera2d()
	: cam()
	, startPos()
	, input()
{
	input.key = MOUSE_BUTTON_RIGHT;
	cam.zoom = 1.0f;
	cam.offset.x = GetScreenWidth() * 0.5f;
	cam.offset.y = GetScreenHeight() * 0.5f;
}

ECS::Camera2d::~Camera2d()
{
}

void ECS::Camera2d::Update()
{

}

void ECS::Camera2d::HandleInputs()
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	input.Poll();
	this->PanCamera();
	//handle zoom
	int zOff = GetMouseWheelMove();
	if (zOff)
	{
		if (zOff > 0) cam.zoom *= 1.1f;
		else cam.zoom /= 1.1f;

		if (cam.zoom < 0.01f) cam.zoom = 0.01f;
	}
}
void ECS::Camera2d::PanCamera()
{
	if (input.just_pressed)
	{
		startPos = GetScreenToWorld2D(GetMousePosition(), cam);
	}
	else if (input.just_released)
	{

	}
	else if (!input.just_pressed && input.isDown)
	{
		Vector2 wc = GetScreenToWorld2D(GetMousePosition(), cam);
		Vector2 currMousePos = wc;
		Vector2 _mouse_position_diff = Vector2Subtract(currMousePos, startPos);

		cam.target.x -= _mouse_position_diff.x;
		cam.target.y -= _mouse_position_diff.y;
	}
}
void ECS::Camera2d::Debug(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::SliderFloat2("offset", &cam.offset.x, -GetScreenWidth(), GetScreenWidth());
		ImGui::InputFloat2("target", &cam.target.x);
		ImGui::SliderFloat("rotation", &cam.rotation, -360, 360);
		ImGui::SliderFloat("zoom", &cam.zoom, 1, 100);
		input.Debug("Pan Key");
		ImGui::TreePop();
	}
}
