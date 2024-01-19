#include "Scene.h"
#include <box2d/box2d.h>
#include <imgui.h>
#include "RenderPipeline.h"
#include "Components2d.h"
#include "BoxMouse.h"

class Box2dBaseScene : public Scene
{
protected:
	b2World* world;
	b2Vec2 Gravity;
	ECS::Box2dDraw drawer;
	ECS::Camera2d camera;
	int velocity_iterations = 6, position_iterations = 8;
	Box2dMouse* boxMouse;
public:
	Box2dBaseScene()
		:Scene()
		, Gravity(0.0f, 10.0f)
	{
		world = new b2World(Gravity);
		world->SetDebugDraw(&drawer);
		camera.cam.zoom = 1.0f;
		boxMouse = new Box2dMouse(world);
		drawer.e_joint = true;
		drawer.setInternalFlags();
	}
	virtual ~Box2dBaseScene()
	{
		delete world;
		world = NULL;

		delete boxMouse;
		boxMouse = NULL;
	}


	virtual void Draw()
	{
		BeginMode2D(camera.cam);

		world->DebugDraw();

		EndMode2D();
	}
	virtual void Update(const float& deltaTime)
	{
		boxMouse->Update(deltaTime, world);

	}
	virtual void FixedUpdate(const float& timeStep)
	{
		world->Step(timeStep, velocity_iterations, position_iterations);
		boxMouse->FixedUpdate(timeStep, world);
	}
	virtual void PollEvents() override {
		camera.HandleInputs();
		boxMouse->HandleInput(camera.cam, world);
	}
	virtual void Debug() override
	{

		drawer.Debug();
		boxMouse->Debug(world);
		if (ImGui::SliderFloat2("Gravity", &Gravity.x, -20, 20))
		{
			world->SetGravity(Gravity);
		}
		ImGui::SliderInt("velocity iterations", &velocity_iterations, 0, 10);
		ImGui::SliderInt("position iterations", &position_iterations, 0, 10);

		camera.Debug();
	}
};
class Box2dScene_CarScene : public Box2dBaseScene
{
protected:

	b2Body* m_car;
	b2Body* m_wheel1;
	b2Body* m_wheel2;

	float m_speed;
	b2WheelJoint* m_spring1;
	b2WheelJoint* m_spring2;
	bool track = true;
public:
	Box2dScene_CarScene()
		:Box2dBaseScene()
		, m_speed(50.0f)

	{
		Gravity.y = -10.0f;
		world->SetGravity(Gravity);
		camera.cam.zoom = 10.0f;
		camera.cam.rotation = 180;
		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = world->CreateBody(&bd);

			b2EdgeShape shape;

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 0.0f;
			fd.friction = 0.6f;

			shape.SetTwoSided(b2Vec2(-20.0f, 0.0f), b2Vec2(20.0f, 0.0f));
			ground->CreateFixture(&fd);

			float hs[10] = { 0.25f, 1.0f, 4.0f, 0.0f, 0.0f, -1.0f, -2.0f, -2.0f, -1.25f, 0.0f };

			float x = 20.0f, y1 = 0.0f, dx = 5.0f;

			for (int32 i = 0; i < 10; ++i)
			{
				float y2 = hs[i];
				shape.SetTwoSided(b2Vec2(x, y1), b2Vec2(x + dx, y2));
				ground->CreateFixture(&fd);
				y1 = y2;
				x += dx;
			}

			for (int32 i = 0; i < 10; ++i)
			{
				float y2 = hs[i];
				shape.SetTwoSided(b2Vec2(x, y1), b2Vec2(x + dx, y2));
				ground->CreateFixture(&fd);
				y1 = y2;
				x += dx;
			}

			shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
			ground->CreateFixture(&fd);

			x += 80.0f;
			shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
			ground->CreateFixture(&fd);

			x += 40.0f;
			shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 10.0f, 5.0f));
			ground->CreateFixture(&fd);

			x += 20.0f;
			shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x + 40.0f, 0.0f));
			ground->CreateFixture(&fd);

			x += 40.0f;
			shape.SetTwoSided(b2Vec2(x, 0.0f), b2Vec2(x, 20.0f));
			ground->CreateFixture(&fd);
		}

		// Teeter
		{
			b2BodyDef bd;
			bd.position.Set(140.0f, 1.0f);
			bd.type = b2_dynamicBody;
			b2Body* body = world->CreateBody(&bd);

			b2PolygonShape box;
			box.SetAsBox(10.0f, 0.25f);
			body->CreateFixture(&box, 1.0f);

			b2RevoluteJointDef jd;
			jd.Initialize(ground, body, body->GetPosition());
			jd.lowerAngle = -8.0f * b2_pi / 180.0f;
			jd.upperAngle = 8.0f * b2_pi / 180.0f;
			jd.enableLimit = true;
			world->CreateJoint(&jd);

			body->ApplyAngularImpulse(100.0f, true);
		}

		// Bridge
		{
			int32 N = 20;
			b2PolygonShape shape;
			shape.SetAsBox(1.0f, 0.125f);

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 1.0f;
			fd.friction = 0.6f;

			b2RevoluteJointDef jd;

			b2Body* prevBody = ground;
			for (int32 i = 0; i < N; ++i)
			{
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(161.0f + 2.0f * i, -0.125f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&fd);

				b2Vec2 anchor(160.0f + 2.0f * i, -0.125f);
				jd.Initialize(prevBody, body, anchor);
				world->CreateJoint(&jd);

				prevBody = body;
			}

			b2Vec2 anchor(160.0f + 2.0f * N, -0.125f);
			jd.Initialize(prevBody, ground, anchor);
			world->CreateJoint(&jd);
		}

		// Boxes
		{
			b2PolygonShape box;
			box.SetAsBox(0.5f, 0.5f);

			b2Body* body = NULL;
			b2BodyDef bd;
			bd.type = b2_dynamicBody;

			bd.position.Set(230.0f, 0.5f);
			body = world->CreateBody(&bd);
			body->CreateFixture(&box, 0.5f);

			bd.position.Set(230.0f, 1.5f);
			body = world->CreateBody(&bd);
			body->CreateFixture(&box, 0.5f);

			bd.position.Set(230.0f, 2.5f);
			body = world->CreateBody(&bd);
			body->CreateFixture(&box, 0.5f);

			bd.position.Set(230.0f, 3.5f);
			body = world->CreateBody(&bd);
			body->CreateFixture(&box, 0.5f);

			bd.position.Set(230.0f, 4.5f);
			body = world->CreateBody(&bd);
			body->CreateFixture(&box, 0.5f);
		}

		// Car
		{
			b2PolygonShape chassis;
			b2Vec2 vertices[8];
			vertices[0].Set(-1.5f, -0.5f);
			vertices[1].Set(1.5f, -0.5f);
			vertices[2].Set(1.5f, 0.0f);
			vertices[3].Set(0.0f, 0.9f);
			vertices[4].Set(-1.15f, 0.9f);
			vertices[5].Set(-1.5f, 0.2f);
			chassis.Set(vertices, 6);

			b2CircleShape circle;
			circle.m_radius = 0.4f;

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, 1.0f);
			m_car = world->CreateBody(&bd);
			m_car->CreateFixture(&chassis, 1.0f);

			b2FixtureDef fd;
			fd.shape = &circle;
			fd.density = 1.0f;
			fd.friction = 0.9f;

			bd.position.Set(-1.0f, 0.35f);
			m_wheel1 = world->CreateBody(&bd);
			m_wheel1->CreateFixture(&fd);

			bd.position.Set(1.0f, 0.4f);
			m_wheel2 = world->CreateBody(&bd);
			m_wheel2->CreateFixture(&fd);

			b2WheelJointDef jd;
			b2Vec2 axis(0.0f, 1.0f);

			float mass1 = m_wheel1->GetMass();
			float mass2 = m_wheel2->GetMass();

			float hertz = 4.0f;
			float dampingRatio = 0.7f;
			float omega = 2.0f * b2_pi * hertz;

			jd.Initialize(m_car, m_wheel1, m_wheel1->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 20.0f;
			jd.enableMotor = true;
			jd.stiffness = mass1 * omega * omega;
			jd.damping = 2.0f * mass1 * dampingRatio * omega;
			jd.lowerTranslation = -0.25f;
			jd.upperTranslation = 0.25f;
			jd.enableLimit = true;
			m_spring1 = (b2WheelJoint*)world->CreateJoint(&jd);

			jd.Initialize(m_car, m_wheel2, m_wheel2->GetPosition(), axis);
			jd.motorSpeed = 0.0f;
			jd.maxMotorTorque = 10.0f;
			jd.enableMotor = false;
			jd.stiffness = mass2 * omega * omega;
			jd.damping = 2.0f * mass2 * dampingRatio * omega;
			jd.lowerTranslation = -0.25f;
			jd.upperTranslation = 0.25f;
			jd.enableLimit = true;
			m_spring2 = (b2WheelJoint*)world->CreateJoint(&jd);
		}
	}
	~Box2dScene_CarScene()
	{

	}

	virtual void PollEvents() override {
		Box2dBaseScene::PollEvents();


		if (IsKeyDown(KEY_A))
		{
			m_spring1->SetMotorSpeed(-m_speed);

		}
		if (IsKeyDown(KEY_S))
		{
			m_spring1->SetMotorSpeed(0.0f);

		}
		if (IsKeyDown(KEY_D))
		{
			m_spring1->SetMotorSpeed(m_speed);
		}
	};
	virtual void Update(const float& dt) override
	{
		if (track)
		{
			camera.cam.target.x = m_car->GetPosition().x;
		}

	}
	virtual void Draw() override
	{
		Box2dBaseScene::Draw();
		DrawText("Keys: \n\tleft = a\n\tbrake = s\n\tright = d\n\thz down = q\n\thz up = e", 25, 25, 20,
			Color{ (unsigned char)230, (unsigned char)153, (unsigned char)153, (unsigned char)255 });
	}
	virtual void Debug() override
	{
		ImGui::Checkbox("Track Car", &track);
		Box2dBaseScene::Debug();
		ImGui::SliderFloat("Speed", &m_speed, 0.0f, 200.0f);
		b2Vec2 position = m_car->GetPosition();
		ImGui::InputFloat2("Car Position", &position.x);
	}

public:
	static Scene* Create() { return new Box2dScene_CarScene(); }
};


class Box2dScene_Tiles : public Box2dBaseScene
{
public:
	Box2dScene_Tiles() :Box2dBaseScene()
	{
		int e_count = 20;
		b2Timer timer;
		camera.cam.zoom = 10.0f;

		{
			float a = 0.5f;
			b2BodyDef bd;
			bd.position.y = -a;
			b2Body* ground = world->CreateBody(&bd);

			int32 N = 200;
			int32 M = 10;
			b2Vec2 position;
			position.y = 1.0f;
			for (int32 j = 0; j < M; ++j)
			{
				position.x = -N * a;
				for (int32 i = 0; i < N; ++i)
				{
					b2PolygonShape shape;
					shape.SetAsBox(a, a, position, 0.0f);
					ground->CreateFixture(&shape, 0.0f);
					position.x += 2.0f * a;
				}
				position.y += 2.0f * a;
			}
		}


		{
			float a = 0.5f;
			b2PolygonShape shape;
			shape.SetAsBox(a, a);

			b2Vec2 x(-7.0f, 0);
			b2Vec2 y;
			b2Vec2 deltaX(0.5625f, 1.25f);
			b2Vec2 deltaY(1.125f, 0.0f);

			for (int32 i = 0; i < e_count; ++i)
			{
				y = x;
				y.y *= -1;

				for (int32 j = i; j < e_count; ++j)
				{
					b2BodyDef bd;
					bd.type = b2_dynamicBody;
					bd.position = y;

					//if (i == 0 && j == 0)
					//{
					//	bd.allowSleep = false;
					//}
					//else
					//{
					//	bd.allowSleep = true;
					//}

					b2Body* body = world->CreateBody(&bd);
					body->CreateFixture(&shape, 5.0f);
					y += deltaY;
				}

				x += deltaX;
			}
		}
	}
	static Scene* Create() { return new Box2dScene_Tiles; }

};


class Box2dScene_PullyJoint : public Box2dBaseScene
{
	b2PulleyJoint* m_joint1;

public:
	Box2dScene_PullyJoint() :Box2dBaseScene()
	{

		float y = 16.0f;
		float L = 12.0f;
		float a = 1.0f;
		float b = 2.0f;

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = world->CreateBody(&bd);

			b2CircleShape circle;
			circle.m_radius = 2.0f;

			circle.m_p.Set(-10.0f, y - (+b + L));
			ground->CreateFixture(&circle, 0.0f);

			circle.m_p.Set(10.0f, y - (+b + L));
			ground->CreateFixture(&circle, 0.0f);
		}

		{

			b2PolygonShape shape;
			shape.SetAsBox(a, b);

			b2BodyDef bd;
			bd.type = b2_dynamicBody;

			//bd.fixedRotation = true;
			bd.position.Set(-10.0f, y);
			b2Body* body1 = world->CreateBody(&bd);
			body1->CreateFixture(&shape, 5.0f);

			bd.position.Set(10.0f, y);
			//bd.fixedRotation = true;
			b2Body* body2 = world->CreateBody(&bd);
			body2->CreateFixture(&shape, 5.0f);

			b2PulleyJointDef pulleyDef;
			b2Vec2 anchor1(-10.0f, y - b);
			b2Vec2 anchor2(10.0f, y - b);
			b2Vec2 groundAnchor1(-10.0f, y - (+b + L));
			b2Vec2 groundAnchor2(10.0f, y - (+b + L));
			pulleyDef.Initialize(body1, body2, groundAnchor1, groundAnchor2, anchor1, anchor2, 1.5f);

			m_joint1 = (b2PulleyJoint*)world->CreateJoint(&pulleyDef);
		}
		camera.cam.zoom = 11;
		//camera.cam.offset.y = 0.0f;
		this->drawer.e_joint = true;
		this->drawer.setInternalFlags();
	}

	~Box2dScene_PullyJoint()
	{
	}
	virtual void Draw() override
	{
		Box2dBaseScene::Draw();

		float ratio = m_joint1->GetRatio();
		float L = m_joint1->GetCurrentLengthA() + ratio * m_joint1->GetCurrentLengthB();
		DrawText(TextFormat("L1 + %4.2f * L2 = %4.2f", (float)ratio, (float)L), 25, 25, 20, RAYWHITE);
	}
	//virtual void Update(const float& deltaTime){}
	virtual void FixedUpdate(const float& timeStep) {
		Box2dBaseScene::FixedUpdate(timeStep);

	}
	//virtual void PollEvents() override {}
	//virtual void Debug() override{}

	static Scene* Create() { return new Box2dScene_PullyJoint; }
};
class GearJoint : Box2dBaseScene
{
private:
	b2RevoluteJoint* m_joint1, * m_joint2;
	b2PrismaticJoint* m_joint3;
	b2GearJoint* m_joint4, * m_joint5;

public:
	GearJoint() : Box2dBaseScene()
	{

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = world->CreateBody(&bd);

			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(50.0f, 0.0f), b2Vec2(-50.0f, 0.0f));
			ground->CreateFixture(&shape, 0.0f);
		}

		{
			b2CircleShape circle1;
			circle1.m_radius = 1.0f;

			b2PolygonShape box;
			box.SetAsBox(0.5f, 5.0f);

			b2CircleShape circle2;
			circle2.m_radius = 2.0f;

			b2BodyDef bd1;
			bd1.type = b2_staticBody;
			bd1.position.Set(10.0f, 9.0f);
			b2Body* body1 = world->CreateBody(&bd1);
			body1->CreateFixture(&circle1, 5.0f);

			b2BodyDef bd2;
			bd2.type = b2_dynamicBody;
			bd2.position.Set(10.0f, 8.0f);
			b2Body* body2 = world->CreateBody(&bd2);
			body2->CreateFixture(&box, 5.0f);

			b2BodyDef bd3;
			bd3.type = b2_dynamicBody;
			bd3.position.Set(10.0f, 6.0f);
			b2Body* body3 = world->CreateBody(&bd3);
			body3->CreateFixture(&circle2, 5.0f);

			b2RevoluteJointDef jd1;
			jd1.Initialize(body1, body2, bd1.position);
			b2Joint* joint1 = world->CreateJoint(&jd1);

			b2RevoluteJointDef jd2;
			jd2.Initialize(body2, body3, bd3.position);
			b2Joint* joint2 = world->CreateJoint(&jd2);

			b2GearJointDef jd4;
			jd4.bodyA = body1;
			jd4.bodyB = body3;
			jd4.joint1 = joint1;
			jd4.joint2 = joint2;
			jd4.ratio = circle2.m_radius / circle1.m_radius;
			world->CreateJoint(&jd4);
		}

		{
			b2CircleShape circle1;
			circle1.m_radius = 1.0f;

			b2CircleShape circle2;
			circle2.m_radius = 2.0f;

			b2PolygonShape box;
			box.SetAsBox(0.5f, 5.0f);

			b2BodyDef bd1;
			bd1.type = b2_dynamicBody;
			bd1.position.Set(-3.0f, 12.0f);
			b2Body* body1 = world->CreateBody(&bd1);
			body1->CreateFixture(&circle1, 5.0f);

			b2RevoluteJointDef jd1;
			jd1.bodyA = ground;
			jd1.bodyB = body1;
			jd1.localAnchorA = ground->GetLocalPoint(bd1.position);
			jd1.localAnchorB = body1->GetLocalPoint(bd1.position);
			jd1.referenceAngle = body1->GetAngle() - ground->GetAngle();
			m_joint1 = (b2RevoluteJoint*)world->CreateJoint(&jd1);

			b2BodyDef bd2;
			bd2.type = b2_dynamicBody;
			bd2.position.Set(0.0f, 12.0f);
			b2Body* body2 = world->CreateBody(&bd2);
			body2->CreateFixture(&circle2, 5.0f);

			b2RevoluteJointDef jd2;
			jd2.Initialize(ground, body2, bd2.position);
			m_joint2 = (b2RevoluteJoint*)world->CreateJoint(&jd2);

			b2BodyDef bd3;
			bd3.type = b2_dynamicBody;
			bd3.position.Set(2.5f, 12.0f);
			b2Body* body3 = world->CreateBody(&bd3);
			body3->CreateFixture(&box, 5.0f);

			b2PrismaticJointDef jd3;
			jd3.Initialize(ground, body3, bd3.position, b2Vec2(0.0f, 1.0f));
			jd3.lowerTranslation = -5.0f;
			jd3.upperTranslation = 5.0f;
			jd3.enableLimit = true;

			m_joint3 = (b2PrismaticJoint*)world->CreateJoint(&jd3);

			b2GearJointDef jd4;
			jd4.bodyA = body1;
			jd4.bodyB = body2;
			jd4.joint1 = m_joint1;
			jd4.joint2 = m_joint2;
			jd4.ratio = circle2.m_radius / circle1.m_radius;
			m_joint4 = (b2GearJoint*)world->CreateJoint(&jd4);

			b2GearJointDef jd5;
			jd5.bodyA = body2;
			jd5.bodyB = body3;
			jd5.joint1 = m_joint2;
			jd5.joint2 = m_joint3;
			jd5.ratio = -1.0f / circle2.m_radius;
			m_joint5 = (b2GearJoint*)world->CreateJoint(&jd5);
		}
	}
	static Scene* Create() { return new GearJoint(); }
};

class sliderCrank1 : Box2dBaseScene
{
public:
	sliderCrank1() : Box2dBaseScene() {

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			bd.position.Set(0.0f, 17.0f);
			ground = world->CreateBody(&bd);
		}

		{
			b2Body* prevBody = ground;

			// Define crank.
			{
				b2PolygonShape shape;
				shape.SetAsBox(4.0f, 1.0f);

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-8.0f, 20.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&shape, 2.0f);

				b2RevoluteJointDef rjd;
				rjd.Initialize(prevBody, body, b2Vec2(-12.0f, 20.0f));
				world->CreateJoint(&rjd);

				prevBody = body;
			}

			// Define connecting rod
			{
				b2PolygonShape shape;
				shape.SetAsBox(8.0f, 1.0f);

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(4.0f, 20.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&shape, 2.0f);

				b2RevoluteJointDef rjd;
				rjd.Initialize(prevBody, body, b2Vec2(-4.0f, 20.0f));
				world->CreateJoint(&rjd);

				prevBody = body;
			}

			// Define piston
			{
				b2PolygonShape shape;
				shape.SetAsBox(3.0f, 3.0f);

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.fixedRotation = true;
				bd.position.Set(12.0f, 20.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&shape, 2.0f);

				b2RevoluteJointDef rjd;
				rjd.Initialize(prevBody, body, b2Vec2(12.0f, 20.0f));
				world->CreateJoint(&rjd);

				b2PrismaticJointDef pjd;
				pjd.Initialize(ground, body, b2Vec2(12.0f, 17.0f), b2Vec2(1.0f, 0.0f));
				world->CreateJoint(&pjd);
			}
		}
	}
	static Scene* Create() { return new sliderCrank1(); }
};

class sliderCrank2 : Box2dBaseScene
{
public:
	sliderCrank2() : Box2dBaseScene() {

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = world->CreateBody(&bd);

			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
			ground->CreateFixture(&shape, 0.0f);
		}

		{
			b2Body* prevBody = ground;

			// Define crank.
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.5f, 2.0f);

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(0.0f, -7.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&shape, 2.0f);

				b2RevoluteJointDef rjd;
				rjd.Initialize(prevBody, body, b2Vec2(0.0f, -5.0f));
				rjd.motorSpeed = 1.0f * b2_pi;
				rjd.maxMotorTorque = 10000.0f;
				rjd.enableMotor = true;
				m_joint1 = (b2RevoluteJoint*)world->CreateJoint(&rjd);

				prevBody = body;
			}

			// Define follower.
			{
				b2PolygonShape shape;
				shape.SetAsBox(0.5f, 4.0f);

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(0.0f, -13.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&shape, 2.0f);

				b2RevoluteJointDef rjd;
				rjd.Initialize(prevBody, body, b2Vec2(0.0f, -9.0f));
				rjd.enableMotor = false;
				world->CreateJoint(&rjd);

				prevBody = body;
			}

			// Define piston
			{
				b2PolygonShape shape;
				shape.SetAsBox(1.5f, 1.5f);

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.fixedRotation = true;
				bd.position.Set(0.0f, -17.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&shape, 2.0f);

				b2RevoluteJointDef rjd;
				rjd.Initialize(prevBody, body, b2Vec2(0.0f, -17.0f));
				world->CreateJoint(&rjd);

				b2PrismaticJointDef pjd;
				pjd.Initialize(ground, body, b2Vec2(0.0f, -17.0f), b2Vec2(0.0f, 1.0f));

				pjd.maxMotorForce = 1000.0f;
				pjd.enableMotor = true;

				m_joint2 = (b2PrismaticJoint*)world->CreateJoint(&pjd);
			}

			// Create a payload
			{
				b2PolygonShape shape;
				shape.SetAsBox(1.5f, 1.5f);

				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(0.0f, -23.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&shape, 2.0f);
			}
		}
	}
	virtual void Draw() override
	{
		Box2dBaseScene::Draw();
		DrawText("[F] Engage Motor 1\n[M] Engage Motor 2", 25, 25, 20, RAYWHITE);
	}
	virtual void PollEvents() override
	{
		Box2dBaseScene::PollEvents();
		if (IsKeyPressed(KEY_F))
		{
			m_joint2->EnableMotor(!m_joint2->IsMotorEnabled());
			m_joint2->GetBodyB()->SetAwake(true);
		}
		if (IsKeyPressed(KEY_M))
		{

			m_joint1->EnableMotor(!m_joint1->IsMotorEnabled());
			m_joint1->GetBodyB()->SetAwake(true);
		}
	}

	static Scene* Create() { return new sliderCrank2(); }

	b2RevoluteJoint* m_joint1;
	b2PrismaticJoint* m_joint2;
};

class PrismaticJointScene : public Box2dBaseScene
{
public:
	PrismaticJointScene() : Box2dBaseScene()
	{

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = world->CreateBody(&bd);

			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
			ground->CreateFixture(&shape, 0.0f);
		}

		m_enableLimit = true;
		m_enableMotor = false;
		m_motorSpeed = 10.0f;

		{
			b2PolygonShape shape;
			shape.SetAsBox(1.0f, 1.0f);

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(0.0f, 10.0f);
			bd.angle = 0.5f * b2_pi;
			bd.allowSleep = false;
			b2Body* body = world->CreateBody(&bd);
			body->CreateFixture(&shape, 5.0f);

			b2PrismaticJointDef pjd;

			// Horizontal
			pjd.Initialize(ground, body, bd.position, b2Vec2(1.0f, 0.0f));

			pjd.motorSpeed = m_motorSpeed;
			pjd.maxMotorForce = 10000.0f;
			pjd.enableMotor = m_enableMotor;
			pjd.lowerTranslation = -10.0f;
			pjd.upperTranslation = 10.0f;
			pjd.enableLimit = m_enableLimit;

			m_joint = (b2PrismaticJoint*)world->CreateJoint(&pjd);
		}
	}

	b2PrismaticJoint* m_joint;
	float m_motorSpeed;
	bool m_enableMotor;
	bool m_enableLimit;

	virtual void PollEvents() override {
		Box2dBaseScene::PollEvents();

	}
	virtual void Debug() override {
		Box2dBaseScene::Debug();

		ImGui::SetNextWindowPos(ImVec2(10.0f, 100.0f));
		ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
		ImGui::Begin("Joint Controls", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		if (ImGui::Checkbox("Limit", &m_enableLimit))
		{
			m_joint->EnableLimit(m_enableLimit);
		}

		if (ImGui::Checkbox("Motor", &m_enableMotor))
		{
			m_joint->EnableMotor(m_enableMotor);
		}

		if (ImGui::SliderFloat("Speed", &m_motorSpeed, -100.0f, 100.0f, "%.0f"))
		{
			m_joint->SetMotorSpeed(m_motorSpeed);
		}

		ImGui::End();

	}

	static Scene* Create() { return new PrismaticJointScene(); }
};
// a sort of 'weld' joint ^_^
class CantileverScene : public Box2dBaseScene
{
public:
	int e_count = 8;
	CantileverScene() : Box2dBaseScene()
	{

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = world->CreateBody(&bd);

			b2EdgeShape shape;
			shape.SetTwoSided(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
			ground->CreateFixture(&shape, 0.0f);
		}

		{
			b2PolygonShape shape;
			shape.SetAsBox(0.5f, 0.125f);

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 20.0f;

			b2WeldJointDef jd;

			b2Body* prevBody = ground;
			for (int32 i = 0; i < e_count; ++i)
			{
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-14.5f + 1.0f * i, -5.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&fd);

				b2Vec2 anchor(-15.0f + 1.0f * i, -5.0f);
				jd.Initialize(prevBody, body, anchor);
				world->CreateJoint(&jd);

				prevBody = body;
			}
		}

		{
			b2PolygonShape shape;
			shape.SetAsBox(1.0f, 0.125f);

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 20.0f;

			b2WeldJointDef jd;
			jd.damping = 5.0f;
			jd.stiffness = 0.7f;
			//jd.frequencyHz = 5.0f;
			//jd.dampingRatio = 0.7f;

			b2Body* prevBody = ground;
			for (int32 i = 0; i < 3; ++i)
			{
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-14.0f + 2.0f * i, -15.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&fd);

				b2Vec2 anchor(-15.0f + 2.0f * i, -15.0f);
				jd.Initialize(prevBody, body, anchor);
				world->CreateJoint(&jd);

				prevBody = body;
			}
		}

		{
			b2PolygonShape shape;
			shape.SetAsBox(0.5f, 0.125f);

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 20.0f;

			b2WeldJointDef jd;

			b2Body* prevBody = ground;
			for (int32 i = 0; i < e_count; ++i)
			{
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(-4.5f + 1.0f * i, -5.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&fd);

				if (i > 0)
				{
					b2Vec2 anchor(-5.0f + 1.0f * i, -5.0f);
					jd.Initialize(prevBody, body, anchor);
					world->CreateJoint(&jd);
				}

				prevBody = body;
			}
		}

		{
			b2PolygonShape shape;
			shape.SetAsBox(0.5f, 0.125f);

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 20.0f;

			b2WeldJointDef jd;
			//jd.frequencyHz = 8.0f;
			//jd.dampingRatio = 0.7f;
			jd.damping = 8.0f;
			jd.stiffness = 0.7f;
			b2Body* prevBody = ground;
			for (int32 i = 0; i < e_count; ++i)
			{
				b2BodyDef bd;
				bd.type = b2_dynamicBody;
				bd.position.Set(5.5f + 1.0f * i, -10.0f);
				b2Body* body = world->CreateBody(&bd);
				body->CreateFixture(&fd);

				if (i > 0)
				{
					b2Vec2 anchor(5.0f + 1.0f * i, -10.0f);
					jd.Initialize(prevBody, body, anchor);
					world->CreateJoint(&jd);
				}

				prevBody = body;
			}
		}

		for (int32 i = 0; i < 2; ++i)
		{
			b2Vec2 vertices[3];
			vertices[0].Set(-0.5f, 0.0f);
			vertices[1].Set(0.5f, 0.0f);
			vertices[2].Set(0.0f, -1.5f);

			b2PolygonShape shape;
			shape.Set(vertices, 3);

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 1.0f;

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-8.0f + 8.0f * i, -12.0f);
			b2Body* body = world->CreateBody(&bd);
			body->CreateFixture(&fd);
		}

		for (int32 i = 0; i < 2; ++i)
		{
			b2CircleShape shape;
			shape.m_radius = 0.5f;

			b2FixtureDef fd;
			fd.shape = &shape;
			fd.density = 1.0f;

			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(-6.0f + 6.0f * i, -10.0f);
			b2Body* body = world->CreateBody(&bd);
			body->CreateFixture(&fd);
		}

		this->camera.cam.zoom = 16.0;
	}
	static Scene* Create() { return new CantileverScene(); }

	b2Body* m_middle;
};


static int scene000 = RegisterScene("Box2d", "Car", Box2dScene_CarScene::Create);
static int scene001 = RegisterScene("Box2d", "Tiles", Box2dScene_Tiles::Create);
static int scene002 = RegisterScene("Box2d", "Pully Joint", Box2dScene_PullyJoint::Create);
static int scene003 = RegisterScene("Box2d", "Gear Joint", GearJoint::Create);
static int scene004 = RegisterScene("Box2d", "Slider Crank One", sliderCrank1::Create);
static int scene005 = RegisterScene("Box2d", "Slider Crank Two", sliderCrank2::Create);
static int scene006 = RegisterScene("Box2d", "Prismatic Joint", PrismaticJointScene::Create);
static int scene007 = RegisterScene("Box2d", "Cantilever", CantileverScene::Create);
