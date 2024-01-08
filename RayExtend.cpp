#include "RayExtend.h"
#include <LuaBridge/LuaBridge.h>

namespace ERaylib
{

	static void DrawRectangleHex(float x, float y, float w, float h, unsigned int hex)
	{
		DrawRectangleV({ x, y }, { w, h }, GetColor(hex));
	}

	static void DrawCircleHex(float x, float y, float r, unsigned int hex)
	{
		DrawCircleV({ x, y }, r, GetColor(hex));
	}

	static void DrawLineHex(float x, float y, float x1, float y1, unsigned int hex)
	{
		DrawLineV({ x, y }, { x1, y1 }, GetColor(hex));
	}
	static bool HandleCollisionCircleRec(float x1, float y1, float r, float x2, float y2, float w, float h)
	{
		return CheckCollisionCircleRec(Vector2{ x1, y1 }, r, Rectangle{ x2, y2, w, h });
	}

	static bool HandleCollisionCircleCircle(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		return CheckCollisionCircles(Vector2{ x1, y1 }, r1, Vector2{ x2, y2 }, r2);
	}
	static bool HandleCollisionRecRec(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
	{
		return CheckCollisionRecs(Rectangle{ x1, y1, w1, h1 }, Rectangle{ x2, y2, w2, h2 });
	}
	static void HandleTextDraw(const char* string, float x, float y, float fontSize, unsigned int hex)
	{
		DrawTextEx(GetFontDefault(), string, Vector2{ x, y }, fontSize, 1.0f, GetColor(hex));
	}
	static int HandleMeasureText(lua_State* L)
	{
		int top = lua_gettop(L); int count = 1;
		const char* text = lua_tostring(L, count++);
		float fontSize =  lua_tonumber(L, count++);
		Vector2 measurements = MeasureTextEx(GetFontDefault(), text, fontSize, 1.0f);

		lua_pushnumber(L, measurements.x);
		lua_pushnumber(L, measurements.y);

		return 2;
	}
}


void ERaylib::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Raylib")
			.addFunction("GetScreenWidth", GetScreenWidth)
			.addFunction("GetScreenHeight", GetScreenHeight)
			.addFunction("DrawRectangle", ERaylib::DrawRectangleHex)
			.addFunction("DrawCircle", ERaylib::DrawCircleHex)
			.addFunction("DrawLine", ERaylib::DrawLineHex)
			.addFunction("DrawText", ERaylib::HandleTextDraw)
			.addFunction("CheckCollisionCircleRec", ERaylib::HandleCollisionCircleRec)
			.addFunction("CheckCollisionCircles", ERaylib::HandleCollisionCircleCircle)
			.addFunction("CheckCollisionRecs", ERaylib::HandleCollisionRecRec)
			.addFunction("IsKeyPressed", IsKeyPressed)
			.addFunction("IsKeyDown", IsKeyDown)
			.addFunction("IsKeyReleased", IsKeyReleased)
			.addCFunction("MeasureText", ERaylib::HandleMeasureText)
		.endNamespace();
}
