#pragma once
#include "RenderPipeline.h"
#include <lua.hpp>

namespace ERaylib
{
	inline void DrawRectangleHex(float x, float y, float w, float h, unsigned int hex)
	{
		DrawRectangleV({ x, y }, { w, h }, GetColor(hex));
	}

	inline void DrawCircleHex(float x, float y, float r, unsigned int hex)
	{
		DrawCircleV({ x, y }, r, GetColor(hex));
	}

	inline void DrawLineHex(float x, float y, float x1, float y1, unsigned int hex)
	{
		DrawLineV({ x, y }, { x1, y1 }, GetColor(hex));
	}
	inline bool HandleCollisionCircleRec(float x1, float y1, float r, float x2, float y2, float w, float h)
	{
		return CheckCollisionCircleRec(Vector2{ x1, y1 }, r, Rectangle{ x2, y2, w, h });
	}

	inline bool HandleCollisionCircleCircle(float x1, float y1, float r1, float x2, float y2, float r2)
	{
		return CheckCollisionCircles(Vector2{ x1, y1 }, r1, Vector2{x2, y2}, r2);
	}
	inline bool HandleCollisionRecRec(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
	{
		return CheckCollisionRecs(Rectangle{ x1, y1, w1, h1}, Rectangle{ x2, y2, w2, h2 });
	}

	void Extend(lua_State* L);
}

