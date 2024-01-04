#pragma once
#include "RenderPipeline.h"
#include <vector>
#include <string>
#include <map>
#include <imgui.h>
#include <lua.hpp>
#include <box2d/box2d.h>


#define B2_SHAPE_TYPE_BOX		0
#define B2_SHAPE_TYPE_CIRCLE	1
#define B2_SHAPE_TYPE_EDGE		2
namespace test
{
	struct Transform
	{
		Vector2 position = Vector2{ 0.0f, 0.0f };
		Vector2 size = Vector2{ 1.0f, 1.0f };
		Vector2 origin = Vector2{ 0.0f, 0.0f };
		float angle = 0.0f;
		int shape_type = B2_SHAPE_TYPE_BOX;
		Rectangle Destination() const
		{
			Rectangle r;
			r.x = position.x;// -size.x * origin.x;
			r.y = position.y;// -size.y * origin.y;
			r.width = size.x;
			r.height = size.y;
			return r;
		}

		Rectangle Rect() const
		{
			Rectangle r;
			r.x = position.x -  origin.x;
			r.y = position.y -  origin.y;
			r.width = size.x;
			r.height = size.y;
			return r;
		}

		void Debug(const char* title = "Transform", bool inScreenSpace = true)
		{
			if (ImGui::TreeNode(title))
			{
				ImGui::SliderFloat("x", &position.x, 0, GetScreenWidth() - size.x);
				ImGui::SliderFloat("y", &position.y, 0, GetScreenHeight() - size.y);
				ImGui::SliderFloat("w", &size.x, 1, GetScreenWidth() - position.x);
				ImGui::SliderFloat("h", &size.y, 1, GetScreenHeight() - position.y);
				ImGui::SliderFloat2("Origin", &origin.x, -1, 1);
				ImGui::SliderFloat("angle", &angle, -360, 360);


				ImGui::TreePop();
			}

		}
		static void Extend(lua_State* L);
	};

	struct Material
	{
		Texture2D texture = Texture2D{ 1, 1, 1 };
		Color color = RAYWHITE;
		Rectangle source = { 0, 0, 1, 1 };
		Vector2 uv_scroll = { 0, 0 };
		void SetTextureByAlias(const char* alias);
		void SetTexture(Texture2D t)
		{
			texture = t;
			this->source = { 0, 0, (float)t.width, (float)t.height };
		}
		void SetColor(unsigned int hex)
		{
			color = GetColor(hex);
		}
		void SetColorVec(float r, float b, float g, float a)
		{
			Vector4 v = { r, b, g, a };
			color = ColorFromNormalized(v);
		}
		void Debug(const char* title = "Material")
		{
			if (ImGui::TreeNode(title))
			{
				Vector4 a = ColorNormalize(color);
				if (ImGui::ColorEdit4("Window Clear Color", &a.x))
				{
					color = ColorFromNormalized(a);
				}
				int tid = texture.id;
				if (ImGui::InputInt("textureID", &tid)) texture.id = tid;
				ImGui::SliderFloat2("Uv scroll", &uv_scroll.x, -20, 20.0f);

				ImGui::SliderFloat("x", &source.x, -texture.width, texture.width);
				ImGui::SliderFloat("y", &source.y, -texture.height, texture.height);
				ImGui::SliderFloat("width", &source.width, -texture.width, texture.width);
				ImGui::SliderFloat("height", &source.height, -texture.height, texture.height);
				ImGui::TreePop();
			}

		}
		static void Extend(lua_State* L);
	};

	class Rigidbody
	{
	public:
		Rigidbody();
		~Rigidbody();
		b2BodyDef bodyDef;
		b2Body* body = NULL;
		bool enabled = false;
		b2FixtureDef mainFixureDef;
		b2Fixture* mainFixure;
		void Debug(const char* title = "RigidBody");
		void SetBody(b2World* world, const test::Transform& t);
		b2Fixture* SetFixture(b2FixtureDef, int type, Vector2 t);
		static void Extend(lua_State* L);

		int	luaSetBody(lua_State* L);
		int luaSetFixture(lua_State* L);
		bool isEnabled() const { return enabled; }//return body != NULL; }
	};

	void ExtendAll(lua_State* L);


	//bool CheckLua(lua_State* State, int returnValue)
	//{
	//	if (returnValue != LUA_OK)
	//	{
	//		printf("Check Lua Error::\t%s\n", lua_tostring(State, -1));
	//		return false;
	//	}

	//	return true;
	//}

	std::pair<bool, Vector2> CheckCollision(const Transform& a, const Vector2& aSpeed, const Transform& b, const float& dt);
}
