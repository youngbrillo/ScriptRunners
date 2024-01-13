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

		virtual void BeginContact(b2Contact* contact, ECS::Node2d* other) {};
		virtual void EndContact(b2Contact* contact, ECS::Node2d* other) {};
		virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {};
		virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {};

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
		Vector2 textureScale;
		bool solid;
		bool _inspected = false;

	};
}
