#pragma once
#include "Manager.h"
#include <map>
#include <memory>
#include <lua.hpp>
#include "Mission.h"

class MissionManager : public Manager
{
public:
	MissionManager();
	~MissionManager();

	virtual void init() override;
	virtual void clear()override;

public:
	static std::shared_ptr<MissionManager> Instance();
	static Manager Register();
	static void Extend(lua_State* L);

	static unsigned int AddMission(const char* name, const char* path) { return Instance()->addMission(name, path); }
	static SR::Mission* GetMission(std::string name) { return Instance()->getMission(name); };

public:
	int addMission(const char* name, const char* path);
	SR::Mission* getMission(std::string name);

public:
	std::map<std::string, SR::Mission*> mission_list;
	lua_State* L;

};

