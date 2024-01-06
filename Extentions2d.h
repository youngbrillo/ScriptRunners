#pragma once;
#include <lua.hpp>

namespace ECS
{
	void ExtendRayLib(lua_State* L);
	void ExtendAll(lua_State* L);
}