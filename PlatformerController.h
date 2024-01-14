#pragma once
#include "Sprite2d.h"
#include <memory>
#include <vector>
#include <imgui.h>
namespace Player
{
	struct Inputs
	{
		ECS::KeyInput left{ KEY_LEFT };
		ECS::KeyInput right{ KEY_RIGHT };
		ECS::KeyInput up{ KEY_UP };
		ECS::KeyInput down{ KEY_DOWN };
		ECS::KeyInput action{ KEY_Z };
		ECS::KeyInput sprint{ KEY_LEFT_SHIFT };
		int direction = 0;
		int lastDirection = 1;
		void Poll()
		{
			left.Poll(); right.Poll(); up.Poll(); down.Poll(); action.Poll(); sprint.Poll();
			int a = left.isDown ? 1 : 0, d = right.isDown ? 1 : 0;
			direction = d - a;
			if (direction != 0) lastDirection = direction;
		}
		void Debug(const char* title = "Inputs")
		{
			if (ImGui::TreeNode(title))
			{
				ImGui::Text("Direction/Last Direction: %d ", direction, lastDirection);
				left.Debug("Left");
				right.Debug("right");
				down.Debug("down");
				action.Debug("action");
				sprint.Debug("sprint");
				ImGui::TreePop();
			}
		}
	};
	struct State
	{
		bool takingDamage = false;
		bool grounded = false;
		bool crouched = false;
		bool climbing = false;
		bool holding = false;
		bool sprinting = false;
		bool stalled = false; 
		bool airJump_no_input = false; 
		bool double_jump_enabled = false;

		bool front_contact = false;
		bool head_contact = false;
		bool back_contact = false;
		bool movement_restricted = false; 
		bool armed = false;
		bool in_dodge_roll = false;
		bool can_wall_climb = false;
		bool grabbing = false;
		bool pushing = false;
		bool pulling = false;
		void debug(const char* title = "State")
		{
			if (ImGui::TreeNode(title))
			{
				ImGui::Checkbox("movement restricted", &movement_restricted);
				ImGui::Checkbox("takingDamage", &takingDamage);
				ImGui::Checkbox("grounded", &grounded);
				ImGui::Checkbox("front contact", &front_contact);
				ImGui::Checkbox("head contact", &head_contact);
				ImGui::Checkbox("back contact", &back_contact);
				ImGui::Checkbox("crouched", &crouched);
				ImGui::Checkbox("climbing", &climbing);
				ImGui::Checkbox("holding", &holding);
				ImGui::Checkbox("sprinting", &sprinting);
				ImGui::Checkbox("stalled (in-air)", &stalled);
				ImGui::Checkbox("airJump_no_input", &airJump_no_input);
				ImGui::Checkbox("double enabled", &double_jump_enabled);
				ImGui::Checkbox("armed", &armed);
				ImGui::Checkbox("in dodge roll", &in_dodge_roll);
				ImGui::Checkbox("can wall climb", &can_wall_climb);
				ImGui::Checkbox("pushing", &pushing);
				ImGui::Checkbox("grabbing", &grabbing);
				ImGui::Checkbox("pulling", &pulling);
				ImGui::TreePop();
			}
		}
	};



	struct Parameters
	{
		float walk_speed = 2.18f;
		float run_speed = 6.315f;
		float crouch_speed = 0.75f;
		float air_speed = 4.7f;
		float fall_speed = 0.0f; //additional force added after stalling in air
		float jump_force = 10.0f;
		float gravity_scale = 5.0f;

		bool wall_jump_enabled = true;
		bool wall_jump_debug_draw = false;
		b2Vec2 wall_jump_force_scale = b2Vec2(1.0f, 1.0f);
		b2Vec2 wall_jump_dir = b2Vec2(1.0f, -1.0f);

		float double_jump_ratio = 1.0f; //what percentage of the jump force is applied to the double jump?

		float dodge_roll_iframe_duration = 0.166f;
		float wall_climb_speed = 4.0f;

		b2Vec2 wall_jump_direction(int direction) const
		{
			b2Vec2 p2 = wall_jump_dir;
			p2.x *= jump_force * wall_jump_force_scale.x * -direction;
			p2.y *= jump_force * wall_jump_force_scale.y;
			return p2;
		}
		void Debug(const char* title = "Parameters") 
		{
			if (ImGui::TreeNode(title))
			{
				ImGui::SliderFloat("Crouch Speed", &crouch_speed, 0, walk_speed);
				ImGui::SliderFloat("Walk Speed", &walk_speed, 0, run_speed);
				ImGui::SliderFloat("Run Speed", &run_speed, walk_speed, 20);
				ImGui::SliderFloat("Fall Speed", &fall_speed, 0, 100);
				ImGui::SliderFloat("Air Speed", &air_speed, 0, run_speed);
				ImGui::SliderFloat("Jump Force", &jump_force, 0, 100);
				ImGui::SliderFloat("double Jump ratio", &double_jump_ratio, 0, 1);
				ImGui::Text("Wall Jumping");
				ImGui::Checkbox("wall jump: enabled", &wall_jump_enabled);
				ImGui::Checkbox("wall jump: draw debug line", &wall_jump_debug_draw);
				ImGui::SliderFloat2("wall jump: jump force scaling", &wall_jump_force_scale.x, 0.0f, 1.0f);
				ImGui::SliderFloat2("wall jump: direction", &wall_jump_dir.x, -1, 1);
				ImGui::SliderFloat("dodge roll i-frame duration", &dodge_roll_iframe_duration, 0, 1);
				ImGui::SliderFloat("wall climb speed", &wall_climb_speed, 0, 10);
				ImGui::TreePop();
			}
		}

	};


}

namespace ECS
{
	class PlatformerController : public ECS::Sprite2d
	{
	public:
		PlatformerController(const char* name, const char* alias);
		~PlatformerController();

		virtual void Update(const float& deltaTime) override;
		virtual void FixedUpdate(const float& timestep) override;
		virtual void Draw() override;
		virtual void UIDraw() override;
		virtual void Poll() override;
		virtual void inspect() override;
	protected:

		void updateState(const float& dt);
		void updateState_crouch();
		void handleMovement(const float& deltaTime);
		void handleAnimations(const float& deltaTime);
		void handleActions(const float& deltaTime);

		void handleInvincibility() {};
		void restoreMovement() {}//if movement is restricted see if we can enable it again

	protected: //variables
		ECS::RayCastCallback rayCaster;
		Player::Inputs mInputs;
		Player::State mState;
		Player::Parameters mfields;
	};


	template <class T>
	T clamp(const T& v, const T& min, const T& max)
	{
		if (v < min) return min;
		else if (v > max) return max;
		else return v;
	}


	#define CMP_EPSILON_PLAYER 0.00001
	static b2Vec2 move_toward(const b2Vec2& v, const b2Vec2& p_to, const float& p_delta)
	{
		b2Vec2 vd = p_to - v;
		float  len = vd.Length();
		if (len <= p_delta || len < (float)CMP_EPSILON_PLAYER)
			return p_to;
		else {
			b2Vec2 r = v;
			r.x /= len * p_delta;
			r.y /= len * p_delta;

			return r;
		};
	}

	static int Sign(const int& val)
	{
		if (val < 0) return -1;
		return 1;
	}


}

