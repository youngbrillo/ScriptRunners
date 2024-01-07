#pragma once
#include "Components2d.h"
namespace ECS
{
	class Node2d
	{
	public:
		Node2d(const char* name);
		~Node2d();
			
		virtual void Update(const float& deltaTime);
		virtual void FixedUpdate(const float& timestep);
		virtual void Draw();
		virtual void UIDraw();
		virtual void Poll();
		void Inspect();
		virtual void inspect();
	public:
		const char* Name = "Node";
		bool enabled, visible, alive;
		ECS::Transform transform;
		ECS::Material material;
		ECS::RigidBody rigidbody;
		
		float speed;
		Vector2 direction;
		bool solid;
	};
}
