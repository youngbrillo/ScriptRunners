#pragma once
#include <string>
#include <vector>
#include <map>
#include <lua.hpp>

#include "JsonHandler.h"

/*
This should work as the 'save' so that:
	- we can store data in memory/on the disk
	- the scene scripts can access this class and determine what assets to load or destroy
	- we have a sense of object and event persistence.
*/
class AppSave
{
public:
	AppSave();
	AppSave(std::string path);
	~AppSave();

	void LoadFile(std::string path);
	void SaveFile(std::string path);
public: //helper functions
	//setters
	bool AddParameter_string(std::string, std::string);
	bool AddParameter_int(std::string, int);
	bool AddParameter_float(std::string, float);
	bool AddParameter_bool(std::string, bool);
	//getters
	std::string GetParameterString(std::string key);
	int	GetParameterint(std::string key);
	float GetParameterfloat(std::string key);
	bool GetParameterBool(std::string key);

public:

	static void Extend(lua_State* L);

public:
	std::string filePath;
	jsonObjects data;
	bool save_on_close = false;
};

