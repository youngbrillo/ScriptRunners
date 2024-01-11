#pragma once
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

namespace ECS
{
	class Script2d
	{
	public:
		Script2d();
		Script2d(const char* path);
		~Script2d();
		void Begin();
		void End();
		void Update(const float& delta);
		void Draw();
		void UIDraw();
		void Poll();
		void Inspect(const char* title = "Script2d");

	public:
		void configureScript(const char* path);
	public:
		lua_State* L;
		const char* path;
		bool enabled;
		bool tryUpdate, tryDraw, tryUIDraw, tryPoll;
		const char* startFunc, * endFunc, * updateFunc, * drawFunc, * uiDrawFunc, * pollFunc;

	};
}


