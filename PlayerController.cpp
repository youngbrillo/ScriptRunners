#include "PlayerController.h"
#include "Scene2d.h"

ECS::PlayerController::PlayerController(const char* name, const char* alias, const char* config)
	: Sprite2d(name, alias)
	, mPlayerConfig(config)
{
	transform.size = Vector2{ 0.5f, 1.0f };
	transform.Align(0.5, 0.5);

	rigidbody.fixDef.density = 1.0f;
	rigidbody.bdyDef.type = b2BodyType::b2_dynamicBody;
	rigidbody.bdyDef.fixedRotation = true;
	rigidbody.bdyDef.gravityScale = 5.0f;

	rigidbody.SetBody(Scene2d::Instance()->world, this->transform, this->material.shape);
}

ECS::PlayerController::~PlayerController()
{
}

void ECS::PlayerController::Update(const float& dt)
{
	Sprite2d::Update(dt);
	this->handleAnimations(dt);
}

void ECS::PlayerController::FixedUpdate(const float& dt)
{
	Sprite2d::FixedUpdate(dt);

	rayCaster.evaluate(rigidbody.body, 0.0f, transform.size.y * 0.55);
	mPlayerConfig.isGrounded = rayCaster.contact;


	this->handleMovement(dt);
	mInputs.up.canDefer = false;
}

void ECS::PlayerController::Draw()
{
	Sprite2d::Draw();
}

void ECS::PlayerController::UIDraw()
{
	Sprite2d::UIDraw();
	const b2Vec2& lv = rigidbody.body->GetLinearVelocity();
	DrawText(TextFormat("Linear velocity: (%.3f, %.3f)", lv.x, lv.y), 25, 25, 20, WHITE);
}

void ECS::PlayerController::Poll()
{
	if (!enabled) return;
	Sprite2d::Poll();
	mInputs.Poll();

	if (mInputs.up.just_pressed) mInputs.up.canDefer = true;
}

void ECS::PlayerController::inspect()
{
	Sprite2d::inspect();
	mInputs.Debug();
	mPlayerConfig.Inspect();
}

#define CMP_EPSILON 0.00001f
static int Sign(const float& val)
{
	if (val < 0) return -1;
	return 1;
}
static float moveValueToward(float target, float value, float force)
{
	float d = target - value;
	if (d == target) { return 0; }
	float r  =  abs(d) < CMP_EPSILON ? target :  Sign(d) * force;
	return r;
}
void ECS::PlayerController::handleMovement(const float& dt)
{
	this->handleXmovement(dt);
	this->handleYmovement(dt);
}

void ECS::PlayerController::handleXmovement(const float& dt)
{
	bool isRunning = mInputs.sprint.isDown;
	float speed = !isRunning ? mPlayerConfig.walk_speed : mPlayerConfig.run_speed;
	float accelleration = !isRunning ? mPlayerConfig.walk_acceleration : mPlayerConfig.run_acceleration;
	float deceleration = !isRunning ? mPlayerConfig.walk_deceleration : mPlayerConfig.run_deceleration;

	const b2Vec2& lv = rigidbody.body->GetLinearVelocity();
	b2Vec2 v = b2Vec2_zero;
	
	if (abs(lv.x) < abs(speed))
	{
		v.x = mInputs.direction * accelleration;
		
	}
		
	if (mInputs.direction == 0)
	{
		v.x = moveValueToward(0.0f, lv.x, deceleration);
	}


	rigidbody.body->ApplyForceToCenter(v, true);

}

void ECS::PlayerController::handleYmovement(const float& dt)
{
	if (mPlayerConfig.isGrounded && mInputs.up.just_pressed)
	{
		rigidbody.body->ApplyLinearImpulseToCenter(b2Vec2(0, -1.0f * mPlayerConfig.jump_launch_mag), true);
		mPlayerConfig.canDoubleJump = true;
		return;
	}

	if(!mPlayerConfig.isGrounded && mPlayerConfig.canDoubleJump && mInputs.up.just_pressed)
	{
		rigidbody.body->ApplyLinearImpulseToCenter(b2Vec2(0, -1.0f * mPlayerConfig.jump_launch_mag), true);
		mPlayerConfig.canDoubleJump = false;
		return;
	}
}

void ECS::PlayerController::handleAnimations(const float& dt)
{
	material.direction.x = mInputs.lastDirection;
	float speed = abs(mInputs.direction * 0.5f);
	if (mInputs.sprint.isDown) 
		speed *= 2.0f;

	this->SetState("grounded", mPlayerConfig.isGrounded);
	this->SetState("speed", speed);
}

void ECS::PlayerController::handleActions(const float& dt)
{
}

#include "Extentions2d.h"
#include "Scene2d.h"
#include <LuaBridge/LuaBridge.h>

static ECS::PlayerController* CreatePlayerController(const char* name, const char* alias, const char* config)
{
	auto  node = std::make_shared<ECS::PlayerController>(name, alias, config);
	Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}
void ECS::PlayerController::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scene")
			.addFunction("CreatePlayerController2", CreatePlayerController)
		.endNamespace()
		.beginNamespace("ECS")
			.deriveClass<ECS::PlayerController, ECS::Sprite2d>("PlayerController")
		.endClass()
	.endNamespace();
}

//static int kc = ExtensionManager::Register(ECS::PlayerController::Extend);

