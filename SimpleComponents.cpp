#include "SimpleComponents.h"
#include <LuaBridge/LuaBridge.h>
#include "TextureManager.h"


void test::Material::SetTextureByAlias(const char* alias)
{
	this->SetTexture(TextureManager::Instance()->GetTexture(alias));
}


static const char* bodyTypeNames[b2BodyType::b2_dynamicBody + 1] = { "Static",  "Kinematic", "Dynamic" };

test::Rigidbody::Rigidbody()
	: body(NULL)
	, mainFixure(NULL)
{
}

test::Rigidbody::~Rigidbody()
{
	if (this->isEnabled())
	{
		b2World* w = body->GetWorld();
		if (!!w) w->DestroyBody(body);
	}
	body = NULL;
	mainFixure = NULL;
}

 void test::Rigidbody::SetBody(b2World* world, const test::Transform& t)
{
	bodyDef.position = b2Vec2(t.position.x, t.position.y);
	bodyDef.angle = t.angle * DEG2RAD;
	if (!!body) { world->DestroyBody(body); }
	body = world->CreateBody(&bodyDef);
	mainFixure = SetFixture(mainFixureDef, t.shape_type, t.size);
	enabled = true;
}

 b2Fixture* test::Rigidbody::SetFixture(b2FixtureDef, int type, Vector2 t)
{
	b2FixtureDef fd = mainFixureDef;

	switch (type)
	{
	case B2_SHAPE_TYPE_BOX:
	{
		b2PolygonShape shape;
		shape.SetAsBox(t.x, t.y);
		fd.shape = &shape;
		return body->CreateFixture(&fd);
	}

	case B2_SHAPE_TYPE_CIRCLE:
	{
		b2CircleShape shape;
		shape.m_radius = t.x;
		fd.shape = &shape;
		return body->CreateFixture(&fd);
	}
	case B2_SHAPE_TYPE_EDGE:
	default:
		break;
	}

	return NULL;
}
void test::Rigidbody::Debug(const char* title)
{
	if (!this->isEnabled())return;
	if (ImGui::TreeNode(title))
	{
		if (ImGui::TreeNode("Body Def"))
		{
			if (ImGui::Checkbox("fixed Rotation", &bodyDef.fixedRotation))
			{
				body->SetFixedRotation(bodyDef.fixedRotation);
			}
			int type = bodyDef.type;
			if (ImGui::SliderInt("type", &type, b2_staticBody, b2_dynamicBody, bodyTypeNames[bodyDef.type])) { 
				bodyDef.type = (b2BodyType)type; 
				body->SetType(bodyDef.type);
			}
			
			if(ImGui::SliderFloat("linearDamping", &bodyDef.linearDamping, 0, 1.0f)){
				body->SetLinearDamping(bodyDef.linearDamping);
			}
			if(ImGui::SliderFloat("angularDamping", &bodyDef.angularDamping, 0, 1.0f)){
				body->SetAngularDamping(bodyDef.angularDamping);
			}
			if (ImGui::SliderFloat("gravityScale", &bodyDef.gravityScale, 0, 1.0f)) {
				body->SetGravityScale(bodyDef.gravityScale);

			}

			ImGui::TreePop();
		}



		if (ImGui::TreeNode("Main Fixture Def"))
		{
			if(ImGui::Checkbox("is sensor", &mainFixureDef.isSensor)){
				mainFixure->SetSensor(mainFixureDef.isSensor);
			}
			if(ImGui::SliderFloat("friction", &mainFixureDef.friction, 0, 1.0f)){
				mainFixure->SetFriction(mainFixureDef.friction);

			}
			if(ImGui::SliderFloat("restitution", &mainFixureDef.restitution, 0, 1.0f)){
				mainFixure->SetRestitution(mainFixureDef.restitution);

			}
			if(ImGui::SliderFloat("restitution Threshold", &mainFixureDef.restitutionThreshold, 0, 1.0f)){
				mainFixure->SetRestitutionThreshold(mainFixureDef.restitutionThreshold);

			}
			if (ImGui::SliderFloat("density", &mainFixureDef.density, 0, 1.0f)) {
				mainFixure->SetDensity(mainFixureDef.density);
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
void test::Material::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("test")
			.beginClass<test::Material>("Material")
				.addFunction("SetTexture", &test::Material::SetTexture)
				.addFunction("SetTextureByAlias", &test::Material::SetTextureByAlias)
				.addFunction("SetColor", &test::Material::SetColor)
				.addFunction("SetColorVec", &test::Material::SetColorVec)
				.addData("source", &test::Material::source)
				.addData("uv_scroll", &test::Material::uv_scroll)
			.endClass()
		.endNamespace();
}

void test::Transform::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("test")
		.beginClass<test::Transform>("Transform")
			.addData("position", &test::Transform::position)
			.addData("size", &test::Transform::size)
			.addData("origin", &test::Transform::origin)
			.addData("angle", &test::Transform::angle)
			.addData("shape_type", &test::Transform::shape_type)
		.endClass()
		.endNamespace();
}
 void test::Rigidbody::Extend(lua_State* L)
{
	 luabridge::getGlobalNamespace(L)
		 .beginNamespace("test")
		 .beginClass<test::Rigidbody>("Rigidbody")
		 .addFunction("isEnabled", &test::Rigidbody::isEnabled)
		 .addFunction("SetBody", &test::Rigidbody::luaSetBody)
		 .addFunction("SetFixture", &test::Rigidbody::luaSetFixture)
		 .endClass()
		 .endNamespace();
}

 int test::Rigidbody::luaSetBody(lua_State* L)
 {
	 int top = lua_gettop(L); int count = 1;
	 return 0;
 }

 int test::Rigidbody::luaSetFixture(lua_State* L)
 {
	 int top = lua_gettop(L); int count = 1;
	 return 0;
 }


#include "SimpleNode.h"
 void test::ExtendAll(lua_State* L)
 {
	 test::Material::Extend(L);
	 test::Transform::Extend(L);
	 test::Rigidbody::Extend(L);
	 test::Node::Extend(L);

	 luabridge::getGlobalNamespace(L)
	//Rectangle
	.beginClass<Rectangle>("Rectangle")
		.addData("x", &Rectangle::x)
		.addData("y", &Rectangle::y)
		.addData("width", &Rectangle::width)
		.addData("height", &Rectangle::height)
		.addFunction("set",
			std::function<void(Rectangle*, float, float, float, float)>(
				[](Rectangle* r, float x, float y, float w, float h) {
					r->x = x; r->y = y; r->width = w; r->height = h;
				}
				))
	.endClass()
	//vector2
	.beginClass<Vector2>("Vector2")
		.addData("x", &Vector2::x)
		.addData("y", &Vector2::y)
		.addFunction("set", std::function<void(Vector2*, float, float)>([](Vector2* r, float x, float y) { r->x = x; r->y = y;  }))
		.endClass()
		//Texture2d
		.beginClass<Texture2D>("Texture2D")
		.addData("id", &Texture2D::id)
		.addData("width", &Texture2D::width)
		.addData("height", &Texture2D::height)
	.endClass()
	.beginNamespace("Raylib")
		.addFunction("GetScreenWidth", GetScreenWidth)
		.addFunction("GetScreenHeight", GetScreenHeight)
	.endNamespace();
 }



 static std::pair<bool, Vector2> CheckCollisionCircle(Vector2 position, Vector2 speed, float radius, const test::Transform& b, const float& deltaTime)
 {
	 std::pair<bool, Vector2> pair(false, Vector2{0.0f, 0.0f});

	 bool result = false;

	 bool contact = CheckCollisionCircleRec(position, radius, b.Destination());
	 if (!contact) return pair;


	 float ballTop = position.y - radius;
	 float ballBottom = position.y + radius;
	 float ballLeft = position.x - radius;
	 float ballRight = position.x + radius;

	 float brickTop = b.position.y - b.size.y / 2;
	 float brickBottom = b.position.y + b.size.y / 2;
	 float brickLeft = b.position.x - b.size.x / 2;
	 float brickRight = b.position.x + b.size.x / 2;


	 bool hit_below = ((ballTop <= brickBottom) && (ballTop > brickBottom + speed.y * deltaTime));
	 bool hit_above = ballBottom <= brickTop && (ballBottom < (brickTop + speed.y * deltaTime));
	 bool hit_left = ballRight >= brickLeft && (ballRight < brickLeft + speed.x * deltaTime);
	 bool hit_right = ballLeft <= ballRight && (ballLeft > brickLeft + speed.x * deltaTime);

	 bool isFalling = speed.y > 0;
	 bool isTravelingRight = speed.x > 0;

	 result = contact;
	 if (hit_below && !isFalling)
		 pair.second.y *= -1;
	 else if (hit_above && isFalling)
		 pair.second.y *= -1;
	 else if (hit_left && isTravelingRight)
		 pair.second.x *= -1;
	 else if (hit_right && !isTravelingRight)
		 pair.second.x *= -1;

	 return pair;
 }
 



 std::pair<bool, Vector2> test::CheckCollision(const Transform& a, const Vector2& aSpeed, const Transform& b, const float& dt)
 {
	 return CheckCollisionCircle(a.position, aSpeed, a.size.x, b, dt);
 }
