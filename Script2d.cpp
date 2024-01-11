#include "Script2d.h"
#include "Components2d.h"

ECS::Script2d::Script2d()
	: path("")
	, startFunc("") , endFunc("")
	, updateFunc(""), drawFunc("") , uiDrawFunc("") , pollFunc("")
	, L(NULL)
	, enabled(false)
	, tryUpdate(false), tryDraw(false), tryUIDraw(false), tryPoll(false)
{
}

ECS::Script2d::Script2d(const char* script_path) 
	: path(script_path)
	, startFunc("onSceneStart"), endFunc("onSceneEnd")
	, updateFunc("Update"), drawFunc("Draw"), uiDrawFunc("UIDraw"), pollFunc("onKeyPress")
	, L(NULL)
	, enabled(false)
	, tryUpdate(true), tryDraw(true), tryUIDraw(true), tryPoll(true)
{
	this->configureScript(path);
}

ECS::Script2d::~Script2d()
{
	if (L != NULL)
	{
		lua_close(L);
	}
	L = NULL;
}

void ECS::Script2d::Begin()
{
	if (!this->enabled) return;

	ECS::CallLuaFunction(L, startFunc);

}

void ECS::Script2d::End()
{
	if (!enabled) return;
	ECS::CallLuaFunction(L, endFunc);
}

void ECS::Script2d::Update(const float& delta)
{
	if (!this->enabled || !tryUpdate) return;
	tryUpdate = ECS::CallLuaFunctionf(L, updateFunc, delta);
}

void ECS::Script2d::Draw()
{
	if (!this->enabled || !tryDraw) return;
	tryDraw = ECS::CallLuaFunction(L, drawFunc);
}

void ECS::Script2d::UIDraw()
{
	if (!this->enabled || !tryUIDraw) return;
	tryUIDraw = ECS::CallLuaFunction(L, uiDrawFunc);
}

void ECS::Script2d::Poll()
{
	if (!this->enabled || !tryPoll) return;

	int key_pressed = GetKeyPressed();
	if (key_pressed == KEY_NULL) return;

	tryPoll = ECS::CallLuaFunctioni(L, pollFunc, key_pressed);
}

void ECS::Script2d::Inspect(const char* title)
{
}

void ECS::Script2d::configureScript(const char* p_for_path)
{
	if (enabled) {
		this->End();
		lua_close(L);
		L = NULL;
	}
	this->enabled = false;
	L = luaL_newstate();
	luaL_openlibs(L);
	int rv = luaL_dofile(L, p_for_path);
	this->enabled = rv == LUA_OK;

	if(!this->enabled)
	{
		printf("LuaScripted Scene found an Error on load:\n\t%s\n", lua_tostring(L, -1));
		lua_close(L);
		L = NULL;
	}
}
