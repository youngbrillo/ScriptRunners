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
	//bottom contact
	rayCaster.evaluate(rigidbody.body, 0.0f, transform.size.y * 0.55);
	mPlayerConfig.isGrounded = rayCaster.contact;
	//head contact
	rayCaster.evaluate(rigidbody.body, 0.0f, -transform.size.y * 0.75);
	mPlayerConfig.head_contact = rayCaster.contact;
	//front contact
	//rayCaster.evaluate(rigidbody.body, mInputs.lastDirection * transform.size.x * 0.55, 0.0f);
	//mPlayerConfig.front_contact = rayCaster.contact;


	this->handleMovement(dt);
	mInputs.up.canDefer = false;
	mInputs.down.canDefer = false;
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
	if (mInputs.down.just_pressed || mInputs.down.just_released) mInputs.down.canDefer = true;
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
	this->handleCrouching(dt);
	this->handleXmovement(dt);
	this->handleYmovement(dt);
}

void ECS::PlayerController::handleCrouching(const float& dt)
{
	bool lastCrouchVal = mPlayerConfig.isCrouching;
	if (lastCrouchVal && !mInputs.down.isDown && mPlayerConfig.head_contact && mPlayerConfig.isGrounded)
	{
		mPlayerConfig.isCrouching = true;
	}
	else
		mPlayerConfig.isCrouching = mInputs.down.isDown && mPlayerConfig.isGrounded;

	if (lastCrouchVal != mPlayerConfig.isCrouching)
	{
		if (mPlayerConfig.isCrouching)
		{
			//constrict fixture
			printf("gen. crouch fixture\n");

			this->rigidbody.body->DestroyFixture(this->rigidbody.fixture);
			this->rigidbody.fixture = NULL;
			b2FixtureDef fd = this->rigidbody.fixDef;
			b2Vec2 crouchSize(b2Vec2_zero), crouchOrigin(b2Vec2_zero);
			crouchSize.x = transform.size.x * 0.5f;
			crouchSize.y = transform.size.y * 0.25f;

			crouchOrigin.y = transform.size.y * 0.25f;

			b2PolygonShape box;
			box.SetAsBox(crouchSize.x, crouchSize.y, crouchOrigin, 0.0f);
			fd.shape = &box;
			fd.density = this->rigidbody.fixDef.density * 2.0f;
			this->rigidbody.fixture = this->rigidbody.body->CreateFixture(&fd);

		}
		else
		{
			//unconstrict fixture
			printf("destroy. crouch fixture, regen std. fixture\n");
			//rigidbody.SetBody(Scene2d::Instance()->world, this->transform, this->material.shape);

			this->rigidbody.body->DestroyFixture(this->rigidbody.fixture);
			this->rigidbody.fixture = NULL;
			this->rigidbody.fixture = rigidbody.createFixture(this->rigidbody.fixDef, this->transform, this->material.shape);
		}
	}
}

void ECS::PlayerController::handleXmovement(const float& dt)
{
	bool isRunning = mInputs.sprint.isDown && mPlayerConfig.isCrouching == false;
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
	float currentSpeed = mInputs.direction * (mInputs.sprint.isDown ? mPlayerConfig.run_speed : mPlayerConfig.walk_speed);


	if (mPlayerConfig.isGrounded && mInputs.up.just_pressed)
	{
		rigidbody.body->ApplyLinearImpulseToCenter(b2Vec2(currentSpeed *0.5f, -1.0f * mPlayerConfig.jump_launch_mag), true);
		mPlayerConfig.canDoubleJump = true;
		return;
	}

	if(!mPlayerConfig.isGrounded && mPlayerConfig.canDoubleJump && mInputs.up.just_pressed)
	{
		rigidbody.body->SetLinearVelocity(b2Vec2(rigidbody.body->GetLinearVelocity().x, 0.0f));
		
		rigidbody.body->ApplyLinearImpulseToCenter(b2Vec2( currentSpeed * 0.5f, -1.0f * mPlayerConfig.jump_launch_mag), true);
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
	this->SetState("crouch", mPlayerConfig.isCrouching);
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

