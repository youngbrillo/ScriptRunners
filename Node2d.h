#pragma once
#include "Components2d.h"
namespace ECS
{
	class Node2d
	{
	public:
		Node2d(const char* name);
		~Node2d();
			
		void Update(const float& deltaTime);
		void FixedUpdate(const float& timestep);
		void Draw();
		void UIDraw();
		void Poll();
		void Inspect();
	public:
		const char* Name = "Node";
		bool enabled, visible, alive;
		ECS::Transform transform;
		ECS::Material material;
		float speed;
		Vector2 direction;
	};
}
