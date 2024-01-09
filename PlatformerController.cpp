#include "PlatformerController.h"
#include <imgui.h>
#include <string>
#include "Scene2d.h"

ECS::PlatformerController::PlatformerController(std::vector<std::shared_ptr<ECS::Node2d>>* ref)
	: Node2d("Player Controller")
#if USING_ADVANCED_TECHNIQUE
	, container(ref)
#endif
	, grounded(false)
	, vk_up(KEY_UP)
	, vk_left(KEY_LEFT)
	, vk_down(KEY_DOWN)
	, vk_right(KEY_RIGHT)
{
	speed = 1.0f;

	transform.size = Vector2{ 1, 2 };
	transform.Align(0.5, 0.5);

	rigidbody.fixDef.density = 1.0f;
	rigidbody.bdyDef.type = b2BodyType::b2_dynamicBody;
	rigidbody.bdyDef.fixedRotation = true;

	rigidbody.SetBody(Scene2d::Instance()->world, this->transform, this->material.shape);
	
}

ECS::PlatformerController::~PlatformerController()
{
#if USING_ADVANCED_TECHNIQUE
	container = NULL;
#endif
}

void ECS::PlatformerController::Update(const float& deltaTime)
{
	Node2d::Update(deltaTime);
}

void ECS::PlatformerController::FixedUpdate(const float& timestep)
{
	Node2d::FixedUpdate(timestep);

#if USING_ADVANCED_TECHNIQUE
	if (findGroundCollison(transform.position.x, transform.position.y, timestep))
	{
		this->grounded = true;
	}

	this->proccessMovement(timestep);
#else
	HandleMovement(timestep);
#endif
	vk_up.canDefer = false;
	vk_left.canDefer = false;
	vk_down.canDefer = false;
	vk_right.canDefer = false;
}

void ECS::PlatformerController::Draw()
{
	Node2d::Draw();
}

void ECS::PlatformerController::UIDraw()
{
	Node2d::UIDraw();
	if (!helper_on.visible) return;

	std::string text = "";
	
	text += TextFormat("anti_gravity_apex: %s", helper_on.anti_gravity_apex ? "True\n" : "False\n");
	text += TextFormat("early_fall:  %s", helper_on.early_fall ? "True\n" : "False\n");
	text += TextFormat("jump_buffering: %s", helper_on.jump_buffering ? "True\n" : "False\n");
	text += TextFormat("sticky_feet: %s", helper_on.sticky_feet ? "True\n" : "False\n");
	text += TextFormat("speedy_apex: %s", helper_on.speedy_apex ? "True\n" : "False\n");
	text += TextFormat("coyote_jump: %s", helper_on.coyote_jump ? "True\n" : "False\n");
	text += TextFormat("clamp_fall_speed: %s", helper_on.clamp_fall_speed ? "True\n" : "False\n");
	text += TextFormat("catch_missed_jump: %s", helper_on.catch_missed_jump ? "True\n" : "False\n");
	text += TextFormat("bumped_head_on_corner: %s", helper_on.bumped_head_on_corner ? "True" : "False");

	Vector2 s = MeasureTextEx(GetFontDefault(), text.c_str(), 20.0f, 1.0f);
	
	DrawTextEx(GetFontDefault(), text.c_str(), Vector2{ (float)GetScreenWidth() - (s.x + 10), 50.0f }, 20.0f, 1.0f, RAYWHITE);

}

void ECS::PlatformerController::Poll()
{
	if (!enabled) return;
	Node2d::Poll();
	if(vk_up.Poll())	vk_up.canDefer = true;
	if(vk_left.Poll())	vk_left.canDefer = true;
	if(vk_down.Poll())	vk_down.canDefer = true;
	if (vk_right.Poll())vk_right.canDefer = true;
}

void ECS::PlatformerController::inspect()
{
	Node2d::inspect();
	ImGui::Checkbox("grounded", &grounded);
	if (ImGui::TreeNode("Helper"))
	{
		ImGui::Checkbox("visible", &helper_on.visible);
		ImGui::Checkbox("anti_gravity_apex", &helper_on.anti_gravity_apex );
		ImGui::Checkbox("early_fall", &helper_on.early_fall );
		ImGui::Checkbox("jump_buffering", &helper_on.jump_buffering );
		ImGui::Checkbox("sticky_feet", &helper_on.sticky_feet );
		ImGui::Checkbox("speedy_apex", &helper_on.speedy_apex );
		ImGui::Checkbox("coyote_jump", &helper_on.coyote_jump );
		ImGui::Checkbox("clamp_fall_speed", &helper_on.clamp_fall_speed );
		ImGui::Checkbox("catch_missed_jump", &helper_on.catch_missed_jump );
		ImGui::Checkbox("bumped_head_on_corner", &helper_on.bumped_head_on_corner);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Controls"))
	{
		ImGui::InputFloat("clamp_walk", &p.clamp_walk, 1.0f, 10.0f);
		ImGui::InputFloat("clamp_fall_speed", &p.clamp_fall_speed, 1.0f, 10.0f);
		ImGui::InputFloat("walk_acceleration", &p.walk_acceleration, 1.0f, 10.0f);
		ImGui::InputFloat("walk_decceleration", &p.walk_decceleration, 1.0f, 10.0f);
		ImGui::InputFloat("jump_height", &p.jump_height, 1.0f, 10.0f);
#if USING_ADVANCED_TECHNIQUE
		ImGui::Checkbox("jump_early_end", &jump_early_end);
		ImGui::InputFloat("current_h_speed", &current_h_speed, 1.0f, 10.0f);
		ImGui::InputFloat("current_v_speed", &current_v_speed, 1.0f, 10.0f);
		ImGui::InputFloat("fall_speed", &fall_speed , 1.0f, 10.0f);
		ImGui::InputFloat("fall_speed_min", &fall_speed_min , 1.0f, 10.0f);
		ImGui::InputFloat("fall_speed_max", &fall_speed_max, 1.0f, 10.0f);

		ImGui::Checkbox("touched_ground_since_last_jump", &touched_ground_since_last_jump);

		ImGui::InputInt("grounded_frames_ago", &grounded_frames_ago, 10, 100);
		ImGui::InputInt("landed_frames_ago", &landed_frames_ago, 10, 100);
		ImGui::InputInt("apex_frames_ago", &apex_frames_ago, 10, 100);
		ImGui::InputInt("w_frames_ago", &w_frames_ago, 10, 100);
		ImGui::InputInt("frames_going_down", &frames_going_down, 10, 100);


		ImGui::Checkbox("at_apex", &at_apex);
		ImGui::InputFloat("speed_for_apex", &speed_for_apex, 1.0f, 10.0f);

#else

#endif

		ImGui::TreePop();
	}

	vk_left.Debug("vk_left");
	vk_right.Debug("vk_right");
	vk_up.Debug("vk_up");
	vk_down.Debug("vk_down");

}
#if USING_ADVANCED_TECHNIQUE

bool ECS::PlatformerController::findGroundCollison(float x, float y, const float& dt)
{
	if (!!container == false) return false;
	bool result = false;
	for (auto&& element : *container)
	{
		if (element.get() == this) 
			continue;

		Rectangle rect = element->transform.Rect();
		if (element->solid &&
			rect.x <= x &&
			rect.x + rect.width >= x &&
			rect.y >= y &&
			rect.y <= y + (this->direction.y * this->speed * dt)
			)
		{
			result = true;
			break;
		}
	}

	return result;
}
#endif
#if USING_ADVANCED_TECHNIQUE

bool ECS::PlatformerController::place_free(float x, float y)
{
	if (!!!container) return false;
	bool result = false;
	for (auto&& element : *container)
	{
		if (element.get() == this) continue;
		Rectangle rect = element->transform.Rect();
		if (CheckCollisionPointRec(Vector2{ x, y }, rect))
		{
			result = true;
			break;
		}
	}

	return result;
}
#endif

template <class T>
T clamp(const T& v, const T& min, const T& max)
{
	if (v < min) return min;
	else if (v > max) return max;
	else return v;
}
#if USING_ADVANCED_TECHNIQUE
void ECS::PlatformerController::proccessMovement(const float& DT)
{
	float move_to_x = transform.position.x;
	float move_to_y = transform.position.y;
	float move_speed_h = 0.0f;
	float move_speed_v = 0.0f;

	bool w = vk_up.just_pressed;//max(keyboard_check_pressed(vk_up), keyboard_check_pressed(ord("W")), keyboard_check_pressed(ord("Z")), keyboard_check_pressed(vk_space))
	bool w_end = vk_up.just_released; //max(keyboard_check_released(vk_up), keyboard_check_released(ord("W")), keyboard_check_released(ord("Z")), keyboard_check_released(vk_space))
	int a = vk_left.isDown ? 1 : 0;//max(keyboard_check(vk_left), keyboard_check(ord("A")), keyboard_check(ord("Q")))
	int s = vk_down.isDown ? 1 : 0; //max(keyboard_check(vk_down), keyboard_check(ord("S")))
	int d = vk_right.isDown ? 1 : 0; //max(keyboard_check(vk_right), keyboard_check(ord("D")))

	if (a > 0 or d > 0) { // they are moving left / right

		current_h_speed += (d - a) * (p.walk_acceleration * DT);
		current_h_speed = clamp(current_h_speed, -(p.clamp_walk * DT), (p.clamp_walk * DT));
	}
	else { // not moving 
		//slow them down 
		if (current_h_speed > walk_decceleration) {
			current_h_speed -= (walk_decceleration * DT);
		}
		else if (current_h_speed < -walk_decceleration) {
			current_h_speed += (walk_decceleration * DT);
		}
		else {
			current_h_speed = 0;
		}
	}


	// Am I at the apex
	at_apex = false;
	fall_speed = fall_speed_max;
	if (grounded == false) {
		if (abs(current_v_speed) < speed_for_apex and helper_on.anti_gravity_apex) {
			at_apex = true;
			fall_speed = fall_speed_min;
			//trigger_helper(helper.anti_gravity_apex)
		}
	}


	// gravty 
	current_v_speed += (fall_speed / DT);

	// max fall speed 
	if (current_v_speed > p.clamp_fall_speed and helper_on.p.clamp_fall_speed) {
		current_v_speed = p.clamp_fall_speed;
		//trigger_helper(helper.p.clamp_fall_speed)
	}



	// work out if I am allowed to jump
	bool can_jump = false;
	if (grounded == true) {
		can_jump = true;
	}
	if (helper_on.coyote_jump and grounded_frames_ago < 8 and grounded == false and helper_on.coyote_jump) {
		can_jump = true;
	}
	if (touched_ground_since_last_jump == false) {
		can_jump = false;
	}

	bool use_w = w; // for buffering w
	if (w_frames_ago < 7 and frames_going_down > 2 and helper_on.jump_buffering) { // can only jump buffer after going down for a little
		use_w = true;
		if (can_jump) {
			//trigger_helper(helper.jump_buffering)
		}
	}

	// DO THE JUMP
	if (use_w and can_jump) {
		current_v_speed = -(p.jump_height);
		touched_ground_since_last_jump = false;
		jump_early_end = false;

		if (grounded == false) {
			// coyote_jump was used
			//trigger_helper(helper.coyote_jump)
		}
	}

	// Better movment at apex
	if (at_apex and helper_on.speedy_apex) {
		if (current_h_speed > 1 and a) {
			current_h_speed -= 5;
			//trigger_helper(helper.speedy_apex)
		}
		if (current_h_speed < -1 and d) {
			current_h_speed += 5;
			//trigger_helper(helper.speedy_apex)
		}

	}

	// early end jump 
	if (w_end and grounded == false and jump_early_end == false and current_v_speed < 0 and helper_on.early_fall) {
		jump_early_end = true;
		current_v_speed = 0;
		//trigger_helper(helper.early_fall)
	}

	move_to_x += (current_h_speed * DT);
	move_to_y += (current_v_speed * DT);

	// hit floor sticky feet
	if (helper_on.sticky_feet && findGroundCollison(move_to_x, move_to_y + 1, DT)) // I'm going to hit the floor
	{
		if (landed_frames_ago < 6) { // just hit the ground this frame
			if (a && current_h_speed > 0) { // you are moving one direction but pressing the button for other way 
				move_to_x -= p.clamp_walk; // ability to instantly change direction when landing 
				current_h_speed = -p.clamp_walk;
				//trigger_helper(helper.sticky_feet)
			}

			if (d && current_h_speed < 0) { // you are moving one direction but pressing the button for other way 
				move_to_x += p.clamp_walk; // ability to instantly change direction when landing 
				current_h_speed = p.clamp_walk;
				//trigger_helper(helper.sticky_feet)
			}
		}
	}
	//#region Actually move the person
	// from the control checks we know where the player should be 
	if (place_free(move_to_x, move_to_y)) {
		// player didnt hit a wall so move them 
		transform.position.x = move_to_x;
		transform.position.y = move_to_y;
	}
	else {
		// I've hit a wall so work out where I should be 
		float xmoveamount = move_to_x - transform.position.x;
		if (xmoveamount > 0) {
			//move_contact_solid(0, xmoveamount)
			//current_h_speed = 0
		}
		else if (xmoveamount < 0) {
			//move_contact_solid(180, -xmoveamount)
			//current_h_speed = 0
		}
		// doing this twice so the player slides along the wall rather than stopping if I collide in one direction and not the other 
		float ymoveamount = move_to_y - transform.position.y;
		if (ymoveamount > 0) {
			//move_contact_solid(270, ymoveamount)
		}
		else if (ymoveamount < 0) {
			//move_contact_solid(90, -ymoveamount)
		}
	}
	//#endregion

	//#region // on ground 
	if (place_free(transform.position.x, transform.position.y + 1)) {
		grounded = false;
	}
	else {
		if (grounded = false) {
			landed_frames_ago = 0;
		}
		grounded = true;
		grounded_frames_ago = 0;
		current_v_speed = 0;
		touched_ground_since_last_jump = true;
	}
	//#endregion


	//#region teleport me to other places
	if (current_v_speed < 0) { // going up
		if (helper_on.bumped_head_on_corner and place_free(transform.position.x, transform.position.y - 1) == false) { // they are about to hit a wall
			float bump_amount = 3;
			if (place_free(move_to_x + bump_amount, move_to_y - 1) == true) {
				transform.position.x += bump_amount;
				transform.position.y -= 1;
				//move_contact_solid(180, bump_amount)
				//trigger_helper(helper.bumped_head_on_corner)
			}
			if (place_free(move_to_x - bump_amount, move_to_y - 1) == true) {
				transform.position.x -= bump_amount;
				transform.position.y -= 1;
				//move_contact_solid(0, bump_amount)
				//trigger_helper(helper.bumped_head_on_corner)
			}
		}
	}
	if (current_v_speed > 0) { // going down
		float bump_amount = 3;
			if (current_h_speed > 1) { // going right
				if (place_free(transform.position.x + 1, transform.position.y) == false) { // they are about to hit a wall
					if (helper_on.catch_missed_jump and place_free(move_to_x, move_to_y - bump_amount) == true) {
						transform.position.x = move_to_x;
						transform.position.y = move_to_y - (bump_amount + 1);
						//move_contact_solid(270, bump_amount + 2)
						//trigger_helper(helper.catch_missed_jump)
					}
				}
			}
		if (current_h_speed < -1) { // going left
			if (place_free(transform.position.x - 1, transform.position.y) == false) { // they are about to hit a wall
				if (helper_on.catch_missed_jump and place_free(move_to_x, move_to_y - bump_amount) == true) {
					transform.position.x = move_to_x;
					transform.position.y = move_to_y - (bump_amount + 1);
					//move_contact_solid(270, bump_amount + 2)
					//trigger_helper(helper.catch_missed_jump)
				}
			}
		}
	}
	//#endregion


	// bumped head 
	if (current_v_speed < 0 and !place_free(transform.position.x, transform.position.y - 1)) {
		current_v_speed = 0;
	}
	else if (current_v_speed > 0 and !place_free(transform.position.x, transform.position.y + 1)) { // on ground
		current_v_speed = 0;
	}
	// touched wall
	if (current_h_speed > 0 and !place_free(transform.position.x + 1, transform.position.y)) {
current_h_speed = 0;
	}
	else if (current_h_speed < 0 and !place_free(transform.position.x - 1, transform.position.y)) {
	current_h_speed = 0;
	}

	this->direction = Vector2{ current_h_speed, current_v_speed };


	// for buffering stuff next frame
	grounded_frames_ago++; // count how long I've been in air
	if (at_apex or (grounded and landed_frames_ago > 2) or current_v_speed < 0) {
		apex_frames_ago = 0;
	}
	else if (current_v_speed > 0) { // going down 
		apex_frames_ago++;
	}
	if ((grounded and landed_frames_ago > 2) or current_v_speed < 0) {
		frames_going_down = 0;
	}
	else if (current_v_speed > 0) { // going down 
		frames_going_down++;
	}
	if (grounded) {
		landed_frames_ago++; // count how long I've been on ground
	}
	if (w) {
		w_frames_ago = 0;
	}
	else {
		w_frames_ago++;
	}



	/*
	for (var i = 0; i < helper.size; ++i) {
		helper_frame[i] ++;
	}


	record_frame++
		if (record_frame > record_count) {
			record_frame = 0
		}
	record_line_x[record_frame] = x
		record_line_y[record_frame] = y
		record_line_colour[record_frame] = c_white
		if (at_apex) {
			record_line_colour[record_frame] = c_blue
		}
	if (current_v_speed >= p.clamp_fall_speed - 0.5) {
		record_line_colour[record_frame] = c_red
	}
	*/


}
#endif



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


void ECS::PlatformerController::HandleMovement(const float& delta)
{
	//variables
	bool w = vk_up.just_pressed;
	bool w_end = vk_up.just_released;
	int a = vk_left.isDown ? 1 : 0;
	int s = vk_down.isDown ? 1 : 0;
	int d = vk_right.isDown ? 1 : 0;
	int x_dir = d - a;
	rayCaster.evaluate(rigidbody.body, transform.size.x, transform.size.y);
	this->grounded = rayCaster.contact;

	rayCaster.evaluate(rigidbody.body, transform.size.x, -transform.size.y);
	bool is_on_ceiling = rayCaster.contact;
	b2Vec2 velocity = b2Vec2_zero;

	//x_movement
	{
		//stop if not doing movement inputs
		if (x_dir == 0)
		{
			//set velocity 
			velocity.x = move_toward(b2Vec2(velocity.x, .00f), b2Vec2(0, 0), p.walk_decceleration * delta).x;
			return;
		}
		//if we are doing movment inputs and above max speed, don't accelerate or decelerate
		//except if turning
		//keeps momentum gained from outside or slopes!

		/*
		float maximum_allowed_speed = p.walk_speed;
		if (get_input.sprinting)
			maximum_allowed_speed = currMove.max_speed;
		if (get_input.y == -1) //crouching
			maximum_allowed_speed = currMove.crouch_speed;
		*/


		if (abs(velocity.x) >= p.clamp_walk && Sign(velocity.x) == x_dir)
		{
			return;
		}
		//are we turning?
		//decide between acceleration and turn_acceleration
		float accel_rate = Sign(velocity.x) == x_dir ? p.walk_acceleration : p.walk_decceleration;

		//accelerate
		velocity.x += x_dir * accel_rate * delta;

		/*
			set_direction(x_dir);//for visuals
		*/
	}
	//jump logic
	{
		/*
		//reset our jump requirements
		if (grounded)
		{
			currMove.jump_coyote_timer = currMove.jump_coyote;
			is_jumping = false;
		}

		if (get_input.just_jumped)
		{
			currMove.jump_buffer_timer = currMove.jump_buffer;
		}

		//Jump if grounded, there is jump input, and we aren't jumping already
		if (currMove.jump_coyote_timer > 0 && currMove.jump_buffer_timer > 0 && !is_jumping)
		{
			is_jumping = true;
			currMove.jump_coyote_timer = 0;
			currMove.jump_buffer_timer = 0;
			//if falling, account for that lost speed
			if (velocity.y > 0) //not > 0 b/c reverse the direction, thanks raylib lol
			{
				velocity.y -= velocity.y;
			}

			velocity.y = -currMove.jump_force;
		}

		if (get_input.relased_jump && velocity.y < 0)
		{
			velocity.y -= (currMove.jump_cut * velocity.y);
		}
		if (is_on_ceiling)
		{
			velocity.y = currMove.jump_hang_treshold + 10.0f;
		}
		*/
	}
	//application of gravity
	{

	}
	//timers (cool-downs and warm-ups)
	{

	}
	//actual movement
	{

	}

}
