#include "Scene.h"
#include <box2d/box2d.h>
#include <imgui.h>
#include "RenderPipeline.h"

class Rayb2Draw : public b2Draw
{
public:
	bool e_shape, e_joint, e_aabb, e_pair, e_centerOfMass;
public:
	Rayb2Draw()
		:b2Draw()
		, e_shape(true)
		, e_joint(false)
		, e_aabb(false)
		, e_pair(false)
		, e_centerOfMass(false)
	{
		setInternalFlags();
	}
	~Rayb2Draw() {

	}

	void setInternalFlags()
	{
		int flags = 0;
		if (e_shape) flags += b2Draw::e_shapeBit;
		if (e_joint) flags += b2Draw::e_jointBit;
		if (e_aabb) flags += b2Draw::e_aabbBit;
		if (e_pair) flags += b2Draw::e_pairBit;
		if (e_centerOfMass) flags += b2Draw::e_centerOfMassBit;

		this->SetFlags(flags);
	}

	/// Draw a closed polygon provided in CCW order.
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		b2Vec2 p1 = vertices[vertexCount - 1];
		rlBegin(RL_LINES);



		Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
		rlColor4ub(_color.r, _color.g, _color.b, _color.a);

		for (int32 i = 0; i < vertexCount; ++i)
		{
			b2Vec2 p2 = vertices[i];

			rlVertex2f(p1.x, p1.y);
			rlVertex2f(p2.x, p2.y);

			p1 = p2;
		}
		rlEnd();
	}
	/// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		
		rlEnd();
	}
	/// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
		Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
		DrawCircleLines(center.x, center.y, radius, _color);

	}
	/// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
		Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
		DrawCircleV({ center.x, center.y }, radius, _color);

	}
	/// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
		Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };

		rlBegin(RL_LINES);

		rlColor4ub(_color.r, _color.g, _color.b, _color.a);

		rlVertex2f(p1.x, p1.y);
		rlVertex2f(p2.x, p2.y);

		rlEnd();
	}
	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf) {

		const float k_axisScale = 0.4f;
		b2Color red(1.0f, 0.0f, 0.0f);
		b2Color green(0.0f, 1.0f, 0.0f);
		b2Vec2 p1 = xf.p, p2;

		p2 = p1 + k_axisScale * xf.q.GetXAxis();

		DrawSegment(p1, p2, red);
		p2 = p1 + k_axisScale * xf.q.GetYAxis();
		DrawSegment(p1, p2, green);
	}
	/// Draw a point.
	virtual void DrawPoint(const b2Vec2& p, float size, const b2Color& color) {
		Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };

		float scaleFactor = 0.050f;
		float _size = size * scaleFactor;
		DrawRectanglePro({ p.x, p.y, _size, _size }, { _size * 0.5f, _size * 0.5f }, 0.0f, _color);
	}

	void Debug(const char* title = "Ray b2Draw")
	{
		if (ImGui::TreeNode(title))
		{
			bool a = ImGui::Checkbox("e_shape", &e_shape);
			bool b = ImGui::Checkbox("e_joint", &e_joint);
			bool c = ImGui::Checkbox("e_aabb", &e_aabb);
			bool d = ImGui::Checkbox("e_pair", &e_pair);
			bool e = ImGui::Checkbox("e_centerOfMass", &e_centerOfMass);

			if (a || b || c || d || e)
			{
				this->setInternalFlags();
			}

			ImGui::TreePop();
		}
	}
};

class B2dCameraWrapper
{
public:
	Camera2D mCamera;
	Vector2 mouseStartPos;
	bool mRightMouseDown = false;
	B2dCameraWrapper()
	{
		mCamera.zoom = 1.0f;
		mCamera.offset.x = GetScreenWidth() / 2.0f;
		mCamera.offset.y = GetScreenHeight() / 2.0f;
	}
	void control()
	{

		if (ImGui::GetIO().WantCaptureMouse) return;
		bool first_click = false;
		//handle position
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			mRightMouseDown = true;
			Vector2 wc = GetScreenToWorld2D(GetMousePosition(), mCamera);
			mouseStartPos = wc;
			first_click = true;
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) mRightMouseDown = false;

		if (mRightMouseDown && !first_click)
		{

			Vector2 wc = GetScreenToWorld2D( GetMousePosition(), mCamera);
			Vector2 currMousePos = wc;
			Vector2 _mouse_position_diff = Vector2Subtract(currMousePos, mouseStartPos);

			mCamera.target.x -= _mouse_position_diff.x;
			mCamera.target.y -= _mouse_position_diff.y;
		}

		//handle zoom
		int zOff = GetMouseWheelMove();
		if (zOff)
		{
			if (zOff > 0) mCamera.zoom *= 1.1f;
			else mCamera.zoom /= 1.1f;


			if (mCamera.zoom < 0.01f) mCamera.zoom = 0.01f;
		}

	}
};

class Box2dBaseScene : public Scene
{
protected:
	b2World* world;
	b2Vec2 Gravity;
	Rayb2Draw drawer;
	B2dCameraWrapper camera;
	int velocity_iterations = 6, position_iterations = 8;

public:
	Box2dBaseScene()
		:Scene()
		, Gravity(0.0f, 10.0f)
	{
		world = new b2World(Gravity);
		world->SetDebugDraw(&drawer);
		camera.mCamera.zoom = 1.0f;
	}
	virtual ~Box2dBaseScene()
	{
		delete world;
		world = NULL;
	}


	virtual void Draw()
	{
		BeginMode2D(camera.mCamera);
		
		world->DebugDraw();

		EndMode2D();
	}
	virtual void Update(const float& deltaTime)
	{

	}
	virtual void FixedUpdate(const float& timeStep)
	{
		world->Step(timeStep, velocity_iterations, position_iterations);
	}
	virtual void PollEvents() override {
		camera.control();
	}
	virtual void Debug() override
	{

		drawer.Debug();
		if (ImGui::SliderFloat2("Gravity", &Gravity.x, -20, 20))
		{
			world->SetGravity(Gravity);
		}
		ImGui::SliderInt("velocity iterations", &velocity_iterations, 0, 10);
		ImGui::SliderInt("position iterations", &position_iterations, 0, 10);


		if (ImGui::TreeNode("Camera"))
		{

			ImGui::SliderFloat("Offset x", &camera.mCamera.offset.x, -GetScreenWidth(), GetScreenWidth());
			ImGui::SliderFloat("Offset y", &camera.mCamera.offset.y, -GetScreenHeight() , GetScreenHeight());
			ImGui::SliderFloat2("target", &camera.mCamera.target.x, -100, 100);
			ImGui::SliderFloat("rotation", &camera.mCamera.rotation, -360, 360);
			ImGui::SliderFloat("zoom", &camera.mCamera.zoom,1, 100);
			
			ImGui::TreePop();
		}
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
		camera.mCamera.zoom = 10.0f;
		camera.mCamera.rotation = 180;
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
			camera.mCamera.target.x = m_car->GetPosition().x;
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
		camera.mCamera.zoom = 10.0f;

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
static int scene000 = RegisterScene("Template", "Box2d: Car", Box2dScene_CarScene::Create);
static int scene001 = RegisterScene("Template", "Box2d: Tiles", Box2dScene_Tiles::Create);
