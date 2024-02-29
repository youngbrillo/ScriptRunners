#pragma once
#include <string>
#include "RenderPipeline.h"
#include "Node2d.h"
#include <lua.hpp>

namespace Job
{
	class Task
	{
	public:
		Task();
		Task(std::string title, std::string desc);
		virtual ~Task();

		int CreateOriginBody(lua_State* L);
		int CreateDestinationBody(lua_State* L);
		void DestroyBody(b2Body* b);

		virtual void Update(const float& dt) = 0;
		virtual void BeginContact(b2Contact* contact, ECS::Node2d* other) = 0;
		virtual void EndContact(b2Contact * contact, ECS::Node2d * other) = 0;
		
		void Inspect();
	protected:
		virtual void inspect();

	public:
		std::string title = "task title";
		std::string description = "go here";
		ECS::Node2d* subject = NULL;
		b2Body* destinationBody = NULL;
		b2Body* originBody = NULL;

		bool succeded = false;
		bool failed = false;
		bool started = false;
	public:
		static void Extend(lua_State* L);
	};


	class Delivery : public Task
	{
	public:
		Delivery();
		Delivery(std::string title, std::string desc);
		virtual ~Delivery();
		virtual void BeginDelivery(float destination_x, float destination_y);

		virtual void Update(const float& dt);
		virtual void BeginContact(b2Contact* contact, ECS::Node2d* other);
		virtual void EndContact(b2Contact* contact, ECS::Node2d* other);
	public:
		float timeLimit = 60; //1 minute
		float startTime = 0;
		float finishTime = -1;
		bool hasTimeLimit = false;

	public:
		static void Extend(lua_State* L);
	};
}


