#include "PlatformerController.h"
#include <imgui.h>
#include <string>
#include "Scene2d.h"

ECS::PlatformerController::PlatformerController(const char* name, const char* alias)
	: Sprite2d(name, alias)
	, rayCaster()
	, mInputs()
	, mState()
	, mfields()
{
	speed = 1.0f;

	transform.size = Vector2{ .5, 1 };
	transform.Align(0.5, 0.5);

	rigidbody.fixDef.density = 1.0f;
	rigidbody.bdyDef.type = b2BodyType::b2_dynamicBody;
	rigidbody.bdyDef.fixedRotation = true;
	rigidbody.bdyDef.gravityScale = 5.0f;

	rigidbody.SetBody(Scene2d::Instance()->world, this->transform, this->material.shape);
	
	

}

ECS::PlatformerController::~PlatformerController()
{
}

void ECS::PlatformerController::Update(const float& deltaTime)
{
	Sprite2d::Update(deltaTime);
	this->updateState(deltaTime);
	this->handleMovement(deltaTime);
	this->handleAnimations(deltaTime);
	this->handleActions(deltaTime);
}

void ECS::PlatformerController::FixedUpdate(const float& timestep)
{
	Sprite2d::FixedUpdate(timestep);
	//check for grounded
	rayCaster.evaluate(rigidbody.body, 0.0f , transform.size.y); 
	mState.grounded = rayCaster.contact;
	//check for head contact
	rayCaster.evaluate(rigidbody.body, 0.0f, -transform.size.y);
	mState.head_contact = rayCaster.contact;
	//check for 'front' contact
	rayCaster.evaluate(rigidbody.body, transform.size.x * mInputs.lastDirection, 0.0f);
	mState.front_contact = rayCaster.contact;// && (mInputs.sprint.isDown || mState.grabbing);
	mState.can_wall_climb = mState.front_contact;
	mState.pushing = mState.grabbing && mInputs.direction != 0;

	if (!mState.grounded)
	{
		mState.in_dodge_roll = false;
	}
	mState.stalled = !mState.grounded && rigidbody.body->GetLinearVelocity().y >= 0.0f;
}

void ECS::PlatformerController::Draw()
{
	Sprite2d::Draw();
}

void ECS::PlatformerController::UIDraw()
{
	Sprite2d::UIDraw();
}

void ECS::PlatformerController::Poll()
{
	if (!enabled) return;
	Sprite2d::Poll();
	mInputs.Poll();
}

void ECS::PlatformerController::inspect()
{
	Sprite2d::inspect();
	mInputs.Debug();
	mState.debug();
	mfields.Debug();

}
void ECS::PlatformerController::updateState(const float& dt)
{
	mState.sprinting = mInputs.sprint.isDown;

	if (mState.airJump_no_input && (mInputs.left.just_pressed || mInputs.right.just_pressed))
	{
		mState.airJump_no_input = false;
	}

	if (mState.grounded && mState.airJump_no_input) mState.airJump_no_input = false;
	if (mState.grounded)
	{
		mState.double_jump_enabled = true;
	}
	if (mState.grounded && mInputs.direction != 0 && mInputs.down.isDown && mState.sprinting)
	{
		mState.in_dodge_roll = true;
		handleInvincibility();
	}
	if (mState.movement_restricted == false) //only try to set if the movement isn't already restricted by something else (i.e. an action)
	{
		restoreMovement();
	}

	updateState_crouch();
}
void ECS::PlatformerController::updateState_crouch()
{
	bool lastCrouched = mState.crouched;
	mState.crouched = mInputs.down.isDown;
	if (lastCrouched && mState.crouched != lastCrouched && mState.head_contact)
	{
		mState.crouched = lastCrouched;
	}

}
void ECS::PlatformerController::handleMovement(const float& deltaTime)
{


	b2Vec2 velocity = b2Vec2_zero;
	velocity.x = mInputs.direction;
	float max_speed = 0.0f;
	bool early_return = false;
	bool set_linear_velocity = false;
	float direction = mInputs.direction;


	if (mState.grounded && !mState.movement_restricted) //on ground
	{
		if (mState.sprinting)
		{
			velocity *= mfields.run_speed;
			max_speed = mfields.run_speed;
		}
		else if (mState.crouched)
		{
			velocity *= mfields.crouch_speed;
			max_speed = mfields.crouch_speed;
		}
		else
		{
			velocity *= mfields.walk_speed;
			max_speed = mfields.walk_speed;
		}

		if (mInputs.up.just_pressed)
		{
			velocity.y = -mfields.jump_force;
			rigidbody.body->ApplyLinearImpulseToCenter(velocity, true);
		}
	}
	else //in air
	{
		if (mState.stalled)
		{
			velocity.y = mfields.fall_speed;
		}
		if (!mState.movement_restricted)
		{
			velocity *= mState.sprinting ? mfields.air_speed_sprint : mfields.air_speed;
			max_speed = mState.sprinting ? mfields.air_speed_sprint : mfields.air_speed;
		}

		if (mState.can_wall_climb && mInputs.up.isDown && !mInputs.up.just_pressed)
		{
			velocity.y -= mfields.wall_climb_speed;
			early_return = true;
			set_linear_velocity = true;
		}
		else if (mfields.wall_jump_enabled && mInputs.up.just_pressed && (mState.front_contact || mState.back_contact) && !mState.movement_restricted)
		{
			//do wall jump
			if (mState.back_contact) direction *= -1.0f;
			b2Vec2 v = mfields.wall_jump_direction(direction);
			rigidbody.body->SetLinearVelocity(b2Vec2_zero);
			rigidbody.body->ApplyLinearImpulseToCenter(v, true);
			mState.airJump_no_input = true;
			early_return = true;
		}
		else if (mInputs.up.just_pressed && mState.double_jump_enabled && !mState.movement_restricted)
		{
			velocity.y = -mfields.jump_force * mfields.double_jump_ratio;
			rigidbody.body->SetLinearVelocity(b2Vec2_zero);
			rigidbody.body->ApplyLinearImpulseToCenter(velocity, true);
			mState.double_jump_enabled = false;
		}
	}

	if (mState.airJump_no_input) { early_return = true; }
	if (early_return && set_linear_velocity)
	{
		rigidbody.body->SetLinearVelocity(velocity);
	}
	else if (abs(rigidbody.body->GetLinearVelocity().x) <= max_speed)
	{
		if (!mState.movement_restricted) rigidbody.body->ApplyForceToCenter(velocity, true);
	}
	else if (early_return == false)
	{
		//instead of sitting the linear velocity to the max speed instantly, bring it back to it, gently
		glm::vec2 v1 = glm::vec2(rigidbody.body->GetLinearVelocity().x, rigidbody.body->GetLinearVelocity().y);
		glm::vec2 vLimit = glm::vec2(direction * max_speed, rigidbody.body->GetLinearVelocity().y);
		rigidbody.body->SetLinearVelocity(b2Vec2(vLimit.x, vLimit.y));
	}

}
void ECS::PlatformerController::handleAnimations(const float& deltaTime)
{
	SetState("grounded", mState.grounded);
	//SetState("doublejump", mState.double_jump_enabled);
	material.direction.x = mInputs.lastDirection;
	float speed = abs(mInputs.direction * 0.5f);
	if (mState.sprinting) speed *= 2.0f;
	SetState("speed",speed);
	SetState("crouch", mState.crouched);
	SetState("fall_land", mState.grounded);

	bool spinningInTheAir = !mState.grounded && !mState.double_jump_enabled;
	if (SetState("doublejump", spinningInTheAir))
	{
	}
	if (animator.getStateIndex("air-spin") == animator.currentState && spinningInTheAir && frameDone)
	{
		SetState("doublejump", false);

	}

	SetState("fall_land", mState.grounded);
	int fall_landState = animator.getStateIndex("fall-land");
	if (fall_landState == animator.currentState && mState.grounded && (frameDone || mInputs.direction != 0))
	{
		SetState("fall_land", false);
	}

	SetState("dodging", mState.in_dodge_roll);
	if (animator.getStateIndex("roll") == animator.currentState && mState.in_dodge_roll && frameDone)
	{
		mState.in_dodge_roll = false;
	}
	SetState("climbing", mState.can_wall_climb);
	SetState("grabbing", mState.grabbing);
	SetState("pushing", mState.pushing);
	SetState("pulling", mState.pulling);
}
void ECS::PlatformerController::handleActions(const float& deltaTime)
{
	if (mInputs.action.just_pressed && !mState.grabbing)
	{
		mState.grabbing = mState.front_contact && mState.grounded;
	}
	else if ((mInputs.action.just_pressed && mState.grabbing )|| mState.front_contact == false)
	{
		mState.grabbing = false;
	}

}