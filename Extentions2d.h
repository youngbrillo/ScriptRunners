#pragma once;
#include <lua.hpp>
#include <vector>
#include <functional>

namespace ECS
{
	void ExtendRayLib(lua_State* L);
	void ExtendAll(lua_State* L);
}

class ExtensionManager
{
public:
	static void Run(lua_State* L);
	static int Register(std::function<void(lua_State*)> func);
private:
	static std::vector<std::function<void(lua_State*)>> function_list;
};