#include "BoxMouse.h"
#include <imgui.h>

Box2dMouse::Box2dMouse(b2World* world)
{
	b2BodyDef bodyDef;
	m_groundBody = world->CreateBody(&bodyDef);
}

Box2dMouse::~Box2dMouse()
{
}

void Box2dMouse::Update(const float& dt, b2World* world)
{
}

void Box2dMouse::FixedUpdate(const float& timestep, b2World* world)
{
	if (m_bombSpawning)
	{
		Vector4 c = Vector4{ 0.0f, 0.0f, 1.0f, 1.0f };
		float size = 4.0f;
		DrawRectanglePro(
			Rectangle{ m_bombSpawnPoint.x, m_bombSpawnPoint.y, size, size }, { size * 0.5f, size * 0.5f }, 0.0f, 
			ColorFromNormalized(c)
		);

		c = Vector4{ 0.8f, 0.8f, 0.8f, 1.0f };
		rlBegin(RL_LINES);
		Color d = ColorFromNormalized(c);
		rlColor4ub(d.r, d.g, d.b, d.a);
		rlVertex2f(m_mouseWorld.x, m_mouseWorld.y);
		rlVertex2f(m_bombSpawnPoint.x, m_bombSpawnPoint.y);
		rlEnd();
	}

}

void Box2dMouse::Debug(b2World* world, const char* title)
{
	if (ImGui::TreeNode(title))
	{

		ImGui::TreePop();
	}
}

void Box2dMouse::HandleInput(Camera2D& camera, b2World* world)
{
	auto v = GetMousePosition();
	v = GetScreenToWorld2D(v, camera);

	b2Vec2 mp = b2Vec2(v.x, v.y);
	onMouse(mp, camera, world);
}

void Box2dMouse::onMouse(const b2Vec2& mousePoint, Camera2D& camera, b2World* world)
{
	if (ImGui::GetIO().WantCaptureMouse) return;

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		onMouseDown(mousePoint, world);

	}

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		onMouseMove(mousePoint, world);

	}
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
		onMouseUp(mousePoint, world);
	}
}

void Box2dMouse::SpawnBomb(const b2Vec2& worldPoint, b2World* world)
{

	m_bombSpawnPoint = worldPoint;
	m_bombSpawning = true;
}

void Box2dMouse::completeBombSpawn(const b2Vec2& p, b2World* world)
{
	if (m_bombSpawning == false)
	{
		return;
	}

	const float multiplier = 30.0f;
	b2Vec2 vel = m_bombSpawnPoint - p;
	vel *= multiplier;
	LaunchBomb(m_bombSpawnPoint, vel, world);
	m_bombSpawning = false;
}

void Box2dMouse::LaunchBomb(b2World* world)
{
	b2Vec2 p(RandomFloat(-15.0f, 15.0f), 30.0f);
	b2Vec2 v = -5.0f * p;
	LaunchBomb(p, v, world);
}
void Box2dMouse::LaunchBomb(const b2Vec2& position, const b2Vec2& velocity, b2World* world)
{
	if (m_bomb)
	{
		world->DestroyBody(m_bomb);
		m_bomb = NULL;
	}

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.position = position;
	bd.bullet = true;
	//bd.userData.pointer = INVALID_ENTITY;
	m_bomb = world->CreateBody(&bd);
	m_bomb->SetLinearVelocity(velocity);

	b2CircleShape circle;
	circle.m_radius = 0.3f;

	b2FixtureDef fd;
	fd.shape = &circle;
	fd.density = 20.0f;
	fd.restitution = 0.4f;
	//fd.userData.pointer = INVALID_ENTITY;
	b2Vec2 minV = position - b2Vec2(0.3f, 0.3f);
	b2Vec2 maxV = position + b2Vec2(0.3f, 0.3f);

	b2AABB aabb;
	aabb.lowerBound = minV;
	aabb.upperBound = maxV;

	m_bomb->CreateFixture(&fd);

}

void Box2dMouse::onMouseDown(const b2Vec2& p, b2World* world)
{
	m_mouseWorld = p;

	if (m_mouseJoint != NULL)
	{
		return;
	}

	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(p);
	world->QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		float frequencyHz = 5.0f;
		float dampingRatio = 0.7f;

		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef jd;
		jd.bodyA = m_groundBody;
		jd.bodyB = body;
		jd.target = p;
		jd.maxForce = 1000.0f * body->GetMass();
		b2LinearStiffness(jd.stiffness, jd.damping, frequencyHz, dampingRatio, jd.bodyA, jd.bodyB);

		m_mouseJoint = (b2MouseJoint*)world->CreateJoint(&jd);
		body->SetAwake(true);
	}

	if (IsKeyDown(KEY_LEFT_SHIFT))
	{
		ShiftMouseDown(p, world);
	}
}

void Box2dMouse::onMouseUp(const b2Vec2& p, b2World* world)
{
	if (m_mouseJoint)
	{
		world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
	}

	if (m_bombSpawning)
	{
		completeBombSpawn(p, world);
	}
}

void Box2dMouse::ShiftMouseDown(const b2Vec2& p, b2World* world)
{
	m_mouseWorld = p;

	if (m_mouseJoint != NULL)
	{
		return;
	}

	SpawnBomb(p, world);

}

void Box2dMouse::onMouseMove(const b2Vec2& p, b2World* world)
{
	m_mouseWorld = p;

	if (m_mouseJoint)
	{
		m_mouseJoint->SetTarget(p);
	}
}
