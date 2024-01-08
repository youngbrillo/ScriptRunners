#pragma once
#include "Node2d.h"
#include <memory>
#include <vector>
#define USING_ADVANCED_TECHNIQUE false

namespace PlatController
{
	struct PlatformerHelper
	{
		bool visible = false;
		bool anti_gravity_apex = true;
		bool early_fall = true;
		bool jump_buffering = true;
		bool sticky_feet = true;
		bool speedy_apex = true;
		bool coyote_jump = true;
		bool clamp_fall_speed = true;
		bool catch_missed_jump = true;
		bool bumped_head_on_corner = true;
	};


	struct AdvancedParameters
	{
		float clamp_walk = 10; // maximum walking speed 
		float clamp_fall_speed = 20;

		float walk_acceleration = 2; // when holding button how fast to get to max speed
		float walk_decceleration = 1.4; // when no button is held how fast to get to 0

		float jump_height = 19;
		bool jump_early_end = false;

		float current_h_speed = 0; // how fast the player would like to move 
		float current_v_speed = 0;

		float fall_speed = 1.2;
		float fall_speed_min = 0.8;
		float fall_speed_max = 1.2;

		bool touched_ground_since_last_jump = false;

		// recording how many frames ago these things happened 
		int grounded_frames_ago = 999;
		int landed_frames_ago = 999;
		int apex_frames_ago = 999; // 0 if on floor and on way up (only counts on way down)
		int w_frames_ago = 999;
		int frames_going_down = 999;

		bool at_apex = false; // this isnt the apex but the top 4th
		float speed_for_apex = 6; // if they are going slower than this and in the air we call this the apex
	};

	struct BaseParamteters
	{
		float clamp_walk = 10.0f;
		float clamp_fall_speed = 20;
		float walk_acceleration = 2.0f;
		float walk_decceleration = 1.4f;
		float jump_height = 10.0f;
	};
	class ControllerRayCastCallback : public b2RayCastCallback
	{
	public:
		float fracLen = 0.0f;
		bool contact = false;
		virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
		{
			if (fixture->IsSensor()) return -1;
			fracLen = fraction;
			contact = true;
		}
		void evaluate(b2Body* body, float width, float height)
		{
			b2Vec2 origin = b2Vec2(body->GetTransform().p.x, body->GetTransform().p.y);
			b2Vec2 p2 = origin; 
			p2.y += height;
			this->contact = false;
			body->GetWorld()->RayCast(this, origin, p2);
		}

	};
}

namespace ECS
{
	class PlatformerController : public ECS::Node2d
	{
	public:
		PlatformerController(std::vector<std::shared_ptr<ECS::Node2d>>* ref);
		~PlatformerController();

		virtual void Update(const float& deltaTime) override;
		virtual void FixedUpdate(const float& timestep) override;
		virtual void Draw() override;
		virtual void UIDraw() override;
		virtual void Poll() override;
		virtual void inspect() override;
	public:
#if USING_ADVANCED_TECHNIQUE
		std::vector<std::shared_ptr<ECS::Node2d>>* container;
		bool findGroundCollison(float x, float y, const float& dt);
		bool place_free(float x, float y);
#endif
	private:
		//variables
		bool grounded = false;
		PlatController::PlatformerHelper helper_on;
#if USING_ADVANCED_TECHNIQUE
		PlatController::AdvancedParameters p;
		void proccessMovement(const float& DT);
#else
		PlatController::BaseParamteters p;
		PlatController::ControllerRayCastCallback rayCaster;
		void HandleMovement(const float& delta);
#endif
		ECS::KeyInput vk_up, vk_left, vk_down, vk_right;

	};
}

