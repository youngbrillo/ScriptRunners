#include "Player.h"

SR::Player::Player()
{
}

SR::Player::Player(const char* config_file)
{
}

SR::Player::~Player()
{
}
#include <imgui.h>
void SR::Player::Inspect(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::InputFloat("walk speed", &walk_speed, 1.0f, 2.0f);
		ImGui::InputFloat("walk acceleration", &walk_acceleration, .10f, 1.0f);
		ImGui::InputFloat("walk deceleration", &walk_deceleration, .10f, 1.0f);

		ImGui::InputFloat("run speed", &run_speed, 1.0f, 5.0f);
		ImGui::InputFloat("run acceleration", &run_acceleration, .10f, 1.0f);
		ImGui::InputFloat("run deceleration", &run_deceleration, .10f, 1.0f);

		ImGui::InputFloat("jump launch magnitude", &jump_launch_mag, 1.0f, 10.0f);
		ImGui::SliderFloat("Ground Friction", &ground_fric, 0, 1);
		ImGui::SliderFloat("Ramp Friction", &ramp_fric, 0, 1);

		ImGui::Checkbox("isGrounded", &isGrounded); ImGui::SameLine(); ImGui::Checkbox("can Double Jump", &canDoubleJump);
		ImGui::Checkbox("head contact", &head_contact); ImGui::SameLine(); ImGui::Checkbox("front contact", &front_contact);
		//ImGui::Checkbox("isGrounded", &isGrounded); ImGui::SameLine(); ImGui::Checkbox("canDoubleJump", &canDoubleJump);
		//ImGui::Checkbox("isGrounded", &isGrounded); ImGui::SameLine(); ImGui::Checkbox("canDoubleJump", &canDoubleJump);

		ImGui::TreePop();
	}
}
