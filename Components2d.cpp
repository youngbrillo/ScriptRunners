#include "Components2d.h"
#include <imgui_string_wrap_extension.h>
#include "TextureManager.h"

// TRANSFORM //////////////////////////////////////////////
Rectangle ECS::Transform::Destination() const
{
	return Rectangle{ position.x, position.y, size.x, size.y };
}

Rectangle ECS::Transform::Rect() const
{
	return Rectangle{ 
		position.x - origin.x,
		position.y - origin.y,
		size.x, 
		size.y 
	};
}
void ECS::Transform::Center()
{
	this->Align(0.5f, 0.5f);
}
void ECS::Transform::Align(float x, float y)
{
	origin.x = size.x * x;
	origin.y = size.y * y;
}

bool ECS::Transform::Debug(const char* title)
{
	int res = 0;
	if (ImGui::TreeNode(title))
	{
		if (ImGui::InputFloat("x", &position.x, 1.0f, 10.0f)) res++;
		if (ImGui::InputFloat("y", &position.y, 1.0f, 10.0f)) res++;
		if (ImGui::InputFloat("width", &size.x, 1.0f, 10.0f)) res++;
		if (ImGui::InputFloat("height", &size.y, 1.0f, 10.0f)) res++;
		if (ImGui::SliderFloat("origin.x", &origin.x, -size.x, size.x)) res++;
		if (ImGui::SliderFloat("origin.y", &origin.y, -size.y, size.y)) res++;
		if (ImGui::SliderFloat("rotation", &rotation, -360, 360)) res++;
		if (ImGui::Button("Center")) {
			this->Center();
			res++;

		}

		ImGui::TreePop();
	}

	return res > 0;
}

// MATERIAL //////////////////////////////////////////////
Rectangle ECS::Material::Source() const
{
	Rectangle r = source;
	r.width *= direction.x;
	r.height *= direction.y;

	return r;
}
void ECS::Material::Update(const float& dt)
{
	source.x += uv_scroll.x * dt;
	source.y += uv_scroll.y * dt;

	if (source.x > texture.width * 2) source.x = texture.width * -2;
	if (source.y > texture.height * 2) source.y = texture.width * -2;
	if (source.x < texture.width * -2) source.x = texture.width * 2;
	if (source.y < texture.height * -2) source.y = texture.width * 2;
}
void ECS::Material::SetTexture(const char* alias)
{
	this->SetTextureT(TextureManager::Instance()->GetTexture(alias));
}
void ECS::Material::SetTextureT(Texture2D t)
{
	texture = t;
	source = Rectangle{ 0, 0, (float)t.width, (float)t.height };
}
void ECS::Material::SetColorH(unsigned int hex)
{
	color = GetColor(hex);
}
void ECS::Material::SetColorV(Vector4 vec)
{
	color = ColorFromNormalized(vec);
}
void ECS::Material::SetColorf(float r, float g, float b, float a)
{
	this->SetColorV(Vector4{ r, g, b, a });
}
static const char* shapeNames[ECS::shape_edge + 1] = { "BOX", "CIRCLE", "EDGE" };

void ECS::Material::Debug(const char* title )
{
	if (ImGui::TreeNode(title))
	{
		Vector4 a = ColorNormalize(color);
		if (ImGui::ColorEdit4("Color", &a.x))
		{
			color = ColorFromNormalized(a);
		}

		int s = shape;
		if (ImGui::SliderInt("Shape", &s, 0, ECS::shape_edge, shapeNames[shape])) shape = (ECS::shape_)s;

		int tid = texture.id;
		if (ImGui::InputInt("textureID", &tid)) texture.id = tid;
		ImGui::SliderFloat2("Uv scroll", &uv_scroll.x, -20, 20.0f);

		ImGui::SliderFloat("x", &source.x, -texture.width, texture.width);
		ImGui::SliderFloat("y", &source.y, -texture.height, texture.height);
		ImGui::SliderFloat("width", &source.width, -texture.width, texture.width);
		ImGui::SliderFloat("height", &source.height, -texture.height, texture.height);
		ImGui::SliderFloat2("Direction", &direction.x,-1.0f, 1.0f);
		ImGui::TreePop();
	}
}

// RIGIDBODY //////////////////////////////////////////////


// SCRIPT /////////////////////////////////////////////////

// INPUT /////////////////////////////////////////////////

void ECS::KeyInput::Debug(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::InputInt("Key", &key);
		ImGui::Checkbox("just_pressed", &just_pressed);
		ImGui::Checkbox("just_released", &just_released);
		ImGui::Checkbox("isDown", &isDown);
		ImGui::Checkbox("canDefer", &canDefer);

		ImGui::TreePop();
	}
}

// CAMERA /////////////////////////////////////////////////

ECS::Camera2d::Camera2d()
	: cam()
	, startPos()
	, input()
{
	input.key = MOUSE_BUTTON_RIGHT;
	cam.zoom = 1.0f;
	cam.offset.x = GetScreenWidth() * 0.5f;
	cam.offset.y = GetScreenHeight() * 0.5f;
}

ECS::Camera2d::~Camera2d()
{
}

void ECS::Camera2d::Update()
{

}

void ECS::Camera2d::HandleInputs()
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	input.Poll();
	this->PanCamera();
	//handle zoom
	int zOff = GetMouseWheelMove();
	if (zOff)
	{
		if (zOff > 0) cam.zoom *= 1.1f;
		else cam.zoom /= 1.1f;

		if (cam.zoom < 0.01f) cam.zoom = 0.01f;
	}
}
void ECS::Camera2d::PanCamera()
{
	if (input.just_pressed)
	{
		startPos = GetScreenToWorld2D(GetMousePosition(), cam);
	}
	else if (input.just_released)
	{

	}
	else if (!input.just_pressed && input.isDown)
	{
		Vector2 wc = GetScreenToWorld2D(GetMousePosition(), cam);
		Vector2 currMousePos = wc;
		Vector2 _mouse_position_diff = Vector2Subtract(currMousePos, startPos);

		cam.target.x -= _mouse_position_diff.x;
		cam.target.y -= _mouse_position_diff.y;
	}
}
void ECS::Camera2d::Debug(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::SliderFloat2("offset", &cam.offset.x, -GetScreenWidth(), GetScreenWidth());
		ImGui::InputFloat2("target", &cam.target.x);
		ImGui::SliderFloat("rotation", &cam.rotation, -360, 360);
		ImGui::SliderFloat("zoom", &cam.zoom, 1, 100);
		input.Debug("Pan Key");
		ImGui::TreePop();
	}
}
// Box2D DRAW /////////////////////////////////////////////////

void ECS::Box2dDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
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

void ECS::Box2dDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	//b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
	rlBegin(RL_TRIANGLES);
	rlColor4ub(_color.r, _color.g, _color.b, _color.a);
	for (int32 i = 1; i < vertexCount - 1; ++i)
	{
		rlVertex2f(vertices[0].x, vertices[0].y);
		rlVertex2f(vertices[i].x, vertices[i].y);
		rlVertex2f(vertices[i + 1].x, vertices[i + 1].y);
	}
	rlEnd();
	rlBegin(RL_LINES);
	rlColor4ub(_color.r, _color.g, _color.b, _color.a);
	b2Vec2 p1 = vertices[vertexCount - 1];
	for (int32 i = 0; i < vertexCount; ++i)
	{
		b2Vec2 p2 = vertices[i];
		rlVertex2f(p1.x, p1.y);
		rlVertex2f(p2.x, p2.y);
		p1 = p2;
	}
}

void ECS::Box2dDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
	DrawCircleLines(center.x, center.y, radius, _color);
	DrawCircleV(Vector2{ center.x, center.y }, radius, _color);
}

void ECS::Box2dDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
	DrawCircleV({ center.x, center.y }, radius, _color);
}

void ECS::Box2dDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
	rlBegin(RL_LINES);
	rlColor4ub(_color.r, _color.g, _color.b, _color.a);
	rlVertex2f(p1.x, p1.y);
	rlVertex2f(p2.x, p2.y);
	rlEnd();
}

void ECS::Box2dDraw::DrawTransform(const b2Transform& xf)
{
	const float k_axisScale = 0.4f;
	b2Color red(1.0f, 0.0f, 0.0f);
	b2Color green(0.0f, 1.0f, 0.0f);
	b2Vec2 p1 = xf.p, p2;
	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1, p2, red);
	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1, p2, green);
}
void ECS::Box2dDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	Color _color = Color{ (unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255) };
	float scaleFactor = 0.050f;
	float _size = size * scaleFactor;
	DrawRectanglePro({ p.x, p.y, _size, _size }, { _size * 0.5f, _size * 0.5f }, 0.0f, _color);
}
void ECS::Box2dDraw::Debug(const char* title)
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

// RIGIDBODY /////////////////////////////////////////////////


ECS::RigidBody::RigidBody()
	: body(NULL)
{
	fixDef.density = 1.0f;
}

ECS::RigidBody::~RigidBody()
{
	if (body != NULL)
	{
		body->DestroyFixture(fixture);
		body->GetWorld()->DestroyBody(this->body);
	}
	body = NULL;
	fixture = NULL;
}


int ECS::RigidBody::configureBodyDef(lua_State* L)
{
	return 0;
}

int ECS::RigidBody::configureFixtureDef(lua_State* L)
{
	return 0;
}

b2Body* ECS::RigidBody::SetBody(b2World* world, const ECS::Transform& t, int shape)
{
	if (body != NULL)
	{
		body->DestroyFixture(fixture);
		fixture = NULL;
		world->DestroyBody(body);
	}
	bdyDef.position.Set(t.position.x, t.position.y);
	bdyDef.angle = DEG2RAD * t.rotation;
	body = world->CreateBody(&bdyDef);
	fixture = createFixture(fixDef, t, shape);

	return body;
}

b2Fixture* ECS::RigidBody::createFixture(b2FixtureDef fixtureDefinition, const ECS::Transform& t, int shape)
{
	/*return this->createFixtureEx(
		fixtureDefinition, 
		b2Vec2{ t.size.x * 0.5f, t.size.y * 0.5f }, 
		b2Vec2{ 0.5f- t.origin.x / t.size.x , 0.5f-t.origin.y/t.size.y }, 
		shape
	);*/
	b2Vec2 v1 = b2Vec2_zero, v2 = b2Vec2_zero;
	b2Fixture* fix = NULL;
	switch (shape)
	{
	case shape_Rectangle:
	{
		v1 = b2Vec2(t.size.x * 0.5f, t.size.y * 0.5f);
		v2 = b2Vec2(0.5f - t.origin.x / t.size.x, 0.5f - t.origin.y / t.size.y);
		b2PolygonShape shape;
		shape.SetAsBox(v1.x, v1.y, v2, 0.0f);

		fixtureDefinition.shape = &shape;
		fix = body->CreateFixture(&fixtureDefinition);
	}
	break;
	case shape_Circle:
	{

		b2CircleShape shape;
		shape.m_radius = t.size.x;
		fixtureDefinition.shape = &shape;
		fix = body->CreateFixture(&fixtureDefinition);
	}
	break;
	case shape_edge:
	{
		v1 = b2Vec2(0.0f, 0.0f);// / t.size.x, 0.5f - t.origin.y / t.size.y);
		v2 = b2Vec2(t.size.x, t.size.y);
		b2EdgeShape shape;
		shape.m_vertex1 = v1;
		shape.m_vertex2 = v2;
		fixtureDefinition.shape = &shape;
		fix = body->CreateFixture(&fixtureDefinition);
	}
	break;
	default:
		break;
	}
	return fix;



}

b2Fixture* ECS::RigidBody::createFixtureEx(b2FixtureDef fixtureDefinition, b2Vec2 v1, b2Vec2 v2, int shape)
{
	b2Fixture* fix = NULL;
	switch (shape)
	{
	case shape_Rectangle:
	{
		b2PolygonShape shape;
		shape.SetAsBox(v1.x, v1.y);
		shape.SetAsBox(v1.x, v1.y, v2, 0.0f);
		fixtureDefinition.shape = &shape;
		fix = body->CreateFixture(&fixtureDefinition);
	}
	break;
	case shape_Circle:
	{

		b2CircleShape shape;
		shape.m_radius =v2.x;
		fixtureDefinition.shape = &shape;
		fix = body->CreateFixture(&fixtureDefinition);
	}
	break;
	case shape_edge:
	{
		b2EdgeShape shape;
		shape.m_vertex1 = v1;
		shape.m_vertex2 = v2;
		fixtureDefinition.shape = &shape;
		fix = body->CreateFixture(&fixtureDefinition);
	}
	break;
	default:
		break;
	}
	return fix;
}

static const char* b2BodyTypeNames[] = { "Static", "Kinematic", "Dynamic" };

void ECS::RigidBody::Debug(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		int res = 0;
		if (ImGui::TreeNode("Body Def"))
		{
			if (ImGui::Checkbox("fixedRotation", &bdyDef.fixedRotation) && this->enabled()) { body->SetFixedRotation(bdyDef.fixedRotation); }
			if (ImGui::SliderFloat("angle", &bdyDef.angle, -2 * PI, 2 * PI) && this->enabled()) { body->SetTransform(body->GetTransform().p, bdyDef.angle); }
			if (ImGui::InputFloat2("linearVelocity", &bdyDef.linearVelocity.x) && this->enabled()) { body->SetLinearVelocity(bdyDef.linearVelocity); }
			if (ImGui::InputFloat("angularVelocity", &bdyDef.angularVelocity, 0.5f, 5.0f) && this->enabled()) { body->SetAngularVelocity(bdyDef.angularVelocity); }
			if (ImGui::SliderFloat("linearDamping", &bdyDef.linearDamping, 0.0f, 1.0f) && this->enabled()) { body->SetLinearDamping(bdyDef.linearDamping); }
			if (ImGui::SliderFloat("angularDamping", &bdyDef.angularDamping, 0.0f, 1.0f) && this->enabled()) { body->SetAngularDamping(bdyDef.angularDamping); }
			if (ImGui::SliderFloat("gravityScale", &bdyDef.gravityScale, 0.0f, 10.0f) && this->enabled()) { body->SetGravityScale(bdyDef.gravityScale); }
			int bt = (int)bdyDef.type;

			bool a = ImGui::SliderInt("Body Type", &bt, 0, b2_dynamicBody, b2BodyTypeNames[bdyDef.type]);
			if(a) {
				bdyDef.type = (b2BodyType)bt;
				if (this->enabled()) body->SetType(bdyDef.type);
			} 
	
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Fixture Def"))
		{
			ImGui::Checkbox("isSensor", &fixDef.isSensor);
			ImGui::SliderFloat("Friction", &fixDef.friction, 0.0f, 1.0f);
			ImGui::SliderFloat("restitution", &fixDef.restitution, 0.0f, 1.0f);
			ImGui::SliderFloat("restitutionThreshold", &fixDef.restitutionThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("density", &fixDef.density, 0.0f, 10.0f);
			ImGui::SliderInt("Shape", &this->shape, 0, ECS::shape_edge, shapeNames[shape]);

			ImGui::InputFloat2("v1", &v1.x);
			ImGui::InputFloat2("v2", &v2.x);


			if (!this->enabled()) ImGui::BeginDisabled();
			bool a = ImGui::Button("Add Fixture");
			if (a) {
				createFixtureEx(fixDef, v1, v2, shape);
			}
			if (!this->enabled()) ImGui::EndDisabled();

			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Fixture") && this->enabled())
		{
			b2FixtureDef fd;
			fd.isSensor = fixture->IsSensor();
			fd.friction = fixture->GetFriction();
			fd.restitution = fixture->GetRestitution();
			fd.restitutionThreshold = fixture->GetRestitutionThreshold();
			fd.density = fixture->GetDensity();

			if (ImGui::Checkbox("isSensor", &fd.isSensor)) {
				fixture->SetSensor(fd.isSensor);
			}
			if(ImGui::SliderFloat("Friction", &fd.friction, 0.0f, 1.0f)){
				fixture->SetFriction(fd.friction);

			}
			if(ImGui::SliderFloat("restitution", &fd.restitution, 0.0f, 1.0f)){
				fixture->SetRestitution(fd.restitution);

			}
			if(ImGui::SliderFloat("restitutionThreshold", &fd.restitutionThreshold, 0.0f, 1.0f)){
				fixture->SetRestitutionThreshold(fd.restitutionThreshold);

			}
			if(ImGui::SliderFloat("density", &fd.density, 0.0f, 10.0f)){
				fixture->SetDensity(fd.density);

			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}



// LUA HELPERS ///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <LuaBridge/LuaBridge.h>
bool ECS::CallLuaFunction(lua_State* L, const char* funcName)
{
	if (L == NULL) return false;
	try {
		luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
		func();
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}

bool ECS::CallLuaFunctionf(lua_State* L, const char* funcName, float v)
{
	if (L == NULL) return false;
	try {
		luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
		func(v);
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}

bool ECS::CallLuaFunctioni(lua_State* L, const char* funcName, int v)
{
	if (L == NULL) return false;
	try {
		luabridge::LuaRef func = luabridge::getGlobal(L, funcName);
		func(v);
		return true;
	}
	catch (luabridge::LuaException const& e) {
		printf("error in '%s'\t%s\n", funcName, e.what());
		return false;
	}
}

// Text ///////////////////////////////////////////////////////////////////////////////////////////////////////
void ECS::Text::update(const float& dt)
{
	if (!visible) return;


	if (cursor >= string.length() && expires)
	{
		timer += dt;
		if (timer >= expirationLimit)
		{
			visible = false;
			timer = 0.0f;
			expires = false;
		}
	}
	else if (cursor < string.length())
	{
		timer += dt;
		if (timer >= writeSpeed)
		{
			timer = 0.0f;
			cursor += inc_size;
		}
	}
}

void ECS::Text::inspect(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::Checkbox("visible", &visible);
		ImGui::Checkbox("expires", &expires);

		ImGui::InputText("string", &string);
		ImGui::SliderInt("cursor", &cursor, 0, string.length());
		ImGui::InputInt("increment", &inc_size, 1, 10);
		ImGui::SliderFloat("timer", &timer, 0, expirationLimit);
		ImGui::SliderFloat("writeSpeed", &writeSpeed, 0, 1.0f);
		ImGui::SliderFloat("expirationLimit", &expirationLimit, 1, 30.0f);
		ImGui::InputFloat("fontSize", &fontSize, 1.0f, 10.0f);
		ImGui::InputFloat("fontSpacing", &fontSpacing, 1.0f, 10.0f);

		Vector4 a = ColorNormalize(color);
		if (ImGui::ColorEdit4("Color", &a.x))
		{
			color = ColorFromNormalized(a);
		}
		ImGui::TreePop();
	}
}