#include "RayExtend.h"
#include <LuaBridge/LuaBridge.h>
void ERaylib::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Raylib")
			.addFunction("GetScreenWidth", GetScreenWidth)
			.addFunction("GetScreenHeight", GetScreenHeight)
			.addFunction("DrawRectangle", ERaylib::DrawRectangleHex)
			.addFunction("DrawCircle", ERaylib::DrawCircleHex)
			.addFunction("DrawLine", ERaylib::DrawLineHex)
			.addFunction("CheckCollisionCircleRec", ERaylib::HandleCollisionCircleRec)
			.addFunction("CheckCollisionCircles", ERaylib::HandleCollisionCircleCircle)
			.addFunction("CheckCollisionRecs", ERaylib::HandleCollisionRecRec)
			.addFunction("IsKeyPressed", IsKeyPressed)
			.addFunction("IsKeyDown", IsKeyDown)
			.addFunction("IsKeyReleased", IsKeyReleased)
		.endNamespace();
}
