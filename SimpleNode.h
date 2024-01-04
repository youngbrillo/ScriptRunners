#pragma once
#include "SimpleComponents.h"
#include <vector>
namespace test
{
	class Node
	{
	public:
		Node(std::string NAME, float x, float y, float w, float h);
		virtual ~Node();

		void Draw();
		void Update(const float& deltaTime);
		void FixedUpdate(const float& deltaTime);
		void Debug(bool inScreenSpace);

	public:
		std::string name = "simple node";
		bool visible = true;
		bool enabled = true;
		bool canDestroy = false;
		test::Transform transform;
		test::Material material;
		test::Rigidbody rigidbody;

		Vector2 velocity = Vector2{ 0.0f, 0.0f };
		float AngularVelocity = 0.0f;

	public:
		static void Extend(lua_State* L);
		static std::vector<test::Node*>* instanceContainer;
	};
}

