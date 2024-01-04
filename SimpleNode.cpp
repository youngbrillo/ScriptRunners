#include "SimpleNode.h"
#include <LuaBridge/LuaBridge.h>

std::vector<test::Node*>* test::Node::instanceContainer = NULL;

test::Node::Node(std::string NAME, float x, float y, float w, float h)
	: name(NAME)
{
	transform.position = Vector2{ x, y };
	transform.size = Vector2{ w,h};

	assert(instanceContainer != NULL && "Instance Container shouldn't be null!");
	instanceContainer->emplace_back(this);
}

test::Node::~Node()
{
	rigidbody.clear();
}

void test::Node::Draw()
{
	if (!visible) return;
	switch (transform.shape_type)
	{
	case B2_SHAPE_TYPE_BOX:
		DrawTexturePro(
			material.texture,
			material.source,
			transform.Destination(),
			transform.origin,
			transform.angle,
			material.color
		);
		break;
	case B2_SHAPE_TYPE_CIRCLE:
		DrawCircleV(transform.position, transform.size.x, material.color);
		break;
	case B2_SHAPE_TYPE_EDGE: 
		DrawLineV(transform.position, transform.size, material.color);
		break;
	default:
		break;
	}
}

void test::Node::Update(const float& deltaTime)
{
	if (!enabled) return;
	if (this->visible)
	{
		material.source.x += material.uv_scroll.x * deltaTime;
		material.source.y += material.uv_scroll.y * deltaTime;
	}
	if (rigidbody.isEnabled()) return;

	transform.angle += AngularVelocity * deltaTime;

	if (transform.position.x < 0.0f || transform.position.x > GetScreenWidth())
	{
		velocity.x *= -1;
	}
	if (transform.position.y < 0.0f || transform.position.y > GetScreenHeight())
	{
		velocity.y *= -1;
	}

	if (transform.angle > 360.0f) transform.angle = -360.0f;
	if (transform.angle < -360.0f) transform.angle = 360.0f;
}

void test::Node::FixedUpdate(const float& deltaTime)
{
	if (!rigidbody.isEnabled()) return;

	transform.position.x = rigidbody.body->GetPosition().x;
	transform.position.y = rigidbody.body->GetPosition().y;
	transform.angle = rigidbody.body->GetAngle() * RAD2DEG;
}

void test::Node::Debug(bool inScreenSpace)
{
	if (ImGui::TreeNode(this->name.c_str()))
	{
		ImGui::Checkbox("visible", &visible);
		ImGui::Checkbox("enabled", &enabled);
		ImGui::Checkbox("canDestroy", &canDestroy);

		transform.Debug("Transform", inScreenSpace);
		material.Debug();
		rigidbody.Debug();

		ImGui::TreePop();
	}
}

void test::Node::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
	.beginNamespace("test")
		.beginClass<test::Node>("Node")
		.addConstructor<void(*)(std::string, float, float, float, float)>()
		.addData("name", &test::Node::name)
		.addData("enabled", &test::Node::enabled)
		.addData("visible", &test::Node::visible)
		.addData("canDestroy", &test::Node::canDestroy)
		.addData("transform", &test::Node::transform)
		.addData("material", &test::Node::material)
		.addData("rigidbody", &test::Node::rigidbody)
		.addData("velocity", &test::Node::velocity)
		.addData("AngularVelocity", &test::Node::AngularVelocity)
		.endClass()
	.endNamespace();
}
