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

		float walk_speed = 1.0, run_speed = 3.0f;
		float walk_acceleration = 0.1f, walk_deceleration = 0.3f;
		float run_acceleration = 0.4f, run_deceleration = 0.6f;

		float jump_launch_mag = 10.0f;
		float ground_fric = 1.0f, ramp_fric = 0.3f;
	};
}

