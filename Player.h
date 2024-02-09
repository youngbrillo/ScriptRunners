#pragma once
namespace SR
{
	//static class, info must persist through 'deaths' and 'scenes'
	class Player
	{
	public:
		Player();
		Player(const char* config_file);
		~Player();
		void Inspect(const char* title = "Player Config");

	public:
		//parameters
		float walk_speed = 4.0, run_speed = 8.0f;
		float walk_acceleration = 10.f, walk_deceleration = 6.0f;
		float run_acceleration = 20.0f, run_deceleration = 15.0f;

		float jump_launch_mag = 10.0f;
		float ground_fric = 1.0f, ramp_fric = 0.3f;

	public:
		//state
		//jumping
		bool isGrounded = false, canDoubleJump = false;
		//climbing
		bool isClimbing = false, canClimb = false, isMantling = false;
		//crouching
		bool isCrouching = false;
		//contacts
		bool front_contact = false, back_contact = false, head_contact = false;
	};
}

