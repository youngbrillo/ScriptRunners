#include "MissionManager.h"
#include <LuaBridge/LuaBridge.h>
#include "GlobalManager.h"
#include "Components2d.h"

MissionManager::MissionManager()
	: Manager("Mission Manager", "Configs/MissionManager.json", "Scripts/Manager/MissionManager.lua")
	, mission_list({})
	, L(NULL)
{
}

MissionManager::~MissionManager()
{
	if (L != NULL) lua_close(L);
	L = NULL;

	auto i = mission_list.begin();

	while (i != mission_list.end())
	{
		SR::Mission* m = i->second;
		delete m;
		m = NULL;
		mission_list.erase(i);
	}
}

void MissionManager::init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	this->Extend(L);

	if (LUA_OK == luaL_dofile(L, scriptPath.c_str()))
	{
		ECS::CallLuaFunction(L, "PreLoadMissions");
	}
	else
	{
		printf("Error in %s\t%s\n", scriptPath.c_str(), lua_tostring(L, -1));
	}

}

void MissionManager::clear()
{
	ECS::CallLuaFunction(L, "UpdateMissions");
	lua_close(L);
	L = NULL;

}

std::shared_ptr<MissionManager> MissionManager::Instance()
{
	return GlobalManager::Get_Manager<MissionManager>();
}

Manager MissionManager::Register()
{
	GlobalManager::Register_Manager<MissionManager>();
	return MissionManager();
}

void MissionManager::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("MissionManager")
			.addFunction("AddMission", MissionManager::AddMission)
			.addFunction("GetMission", MissionManager::GetMission)
		.endNamespace();
}

int MissionManager::addMission(const char* name, const char* path)
{
	return 0;
}

SR::Mission* MissionManager::getMission(std::string name)
{
	return nullptr;
}
//static int gManagerInstance = RegisterManager("Mission Manager", "", "", MissionManager::Register);
#include "Extentions2d.h"
//static int ck = ExtensionManager::Register(MissionManager::Extend);
