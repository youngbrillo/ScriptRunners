#include "RayExtend.h"
#include <LuaBridge/LuaBridge.h>

namespace ERaylib
{

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
