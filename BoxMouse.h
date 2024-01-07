#pragma once
#include <vector>
#include <box2d/box2d.h>
#include <raylib/raylib.h>


struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
	float normalImpulse;
	float tangentImpulse;
	float separation;
};
const int32 k_maxContactPoints = 2048;

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture) override
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

class Box2dMouse
{
public:
	Box2dMouse(b2World* world);
	~Box2dMouse();

	void Update(const float& dt, b2World* world);
	void FixedUpdate(const float& timestep, b2World* world);
	void HandleInput(Camera2D& camera, b2World* world);
	void Debug(b2World* world, const char* title = "Physics Mouse Controller");
protected:
	void onMouse(const b2Vec2& mousePoint, Camera2D& camera, b2World* world);
	void SpawnBomb(const b2Vec2& worldPoint, b2World* world);
	void completeBombSpawn(const b2Vec2& p, b2World* world);
	void LaunchBomb(b2World* world);
	void LaunchBomb(const b2Vec2& position, const b2Vec2& velocity, b2World* world);

private:
	b2Body* m_groundBody;
	b2AABB m_worldAABB;
	//ContactPoint m_points[k_maxContactPoints];
	//int32 m_pointCount;
	int32 m_textLine;
	//b2World* m_world;
	b2Body* m_bomb;
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_bombSpawnPoint;
	bool m_bombSpawning;
	b2Vec2 m_mouseWorld;
	//int32 m_stepCount;
	//int32 m_textIncrement;
	b2Profile m_maxProfile;
	b2Profile m_totalProfile;

private:
	//mouse
	void onMouseDown(const b2Vec2& p, b2World* world);
	void onMouseUp(const b2Vec2& p, b2World* world);
	void ShiftMouseDown(const b2Vec2& p, b2World* world);
	void onMouseMove(const b2Vec2& p, b2World* world);
};
#define	RAND_LIMIT 32767

inline float RandomFloat()
{
	float r = (float)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float RandomFloat(float lo, float hi)
{
	float r = (float)(rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}
#include <glm/glm.hpp>
#include <raylib/rlgl.h>
/*
namespace DRAWING
{
	inline Color ColorConvert(const glm::vec4& color) { return { (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) }; }
	inline void DrawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color)
	{
		rlBegin(RL_LINES);

		Color _color = DRAWING::ColorConvert({ color.r, color.g, color.b, color.a });
		rlColor4ub(_color.r, _color.g, _color.b, _color.a);

		rlVertex2f(p1.x, p1.y);
		rlVertex2f(p2.x, p2.y);

		rlEnd();
	}
	inline void DrawPoint(const glm::vec2& p, float size, const glm::vec4& color)
	{
		DrawRectanglePro({ p.x, p.y, size, size }, { size * 0.5f, size * 0.5f }, 0.0f, DRAWING::ColorConvert(color));
	}
}
*/
