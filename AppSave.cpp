#include "AppSave.h"

AppSave::AppSave()
	: filePath("./configs/autoSave.json")
{
}

AppSave::AppSave(std::string path)
	:filePath(path)
{
	this->LoadFile(path);
}

AppSave::~AppSave()
{
	if(this->save_on_close)
		this->SaveFile(filePath);
}

void AppSave::LoadFile(std::string path)
{
	JSONParse::ParseJSONFile(data, path.c_str());
}

void AppSave::SaveFile(std::string path)
{
	data.SaveToFile(path.c_str());
}

bool AppSave::AddParameter_string(std::string k , std::string v)
{
	bool r = &data == &data.find(k.c_str());
	if (!r) data.SetStringAt(k.c_str(), v);
	
	return r;
}

bool AppSave::AddParameter_int(std::string k, int v)
{
	bool r = &data == &data.find(k.c_str());;
	if (!r) data.SetIntegerAt(k.c_str(), v);
	return r;
}

bool AppSave::AddParameter_float(std::string k, float v)
{
	bool r = &data == &data.find(k.c_str());;
	if (!r) data.SetNumberAt(k.c_str(), v);
	return r;
}

bool AppSave::AddParameter_bool(std::string k, bool v)
{
	bool r = &data == &data.find(k.c_str());;
	if (!r) data.SetBooleanAt(k.c_str(), v);
	return r;
}

std::string AppSave::GetParameterString(std::string key)
{
	std::string r = "";
	data.GetString(key.c_str(), r);
	return r;
}

int AppSave::GetParameterint(std::string key)
{
	int r = -1;
	data.GetInteger(key.c_str(), r);
	return r;
}

float AppSave::GetParameterfloat(std::string key)
{
	float r = -1;
	data.GetNumber(key.c_str(), r);
	return r;
}

bool AppSave::GetParameterBool(std::string key)
{
	bool r = false;
	data.Getboolean(key.c_str(), r);
	return r;
}
#include "Extentions2d.h"
#include <LuaBridge/LuaBridge.h>

void AppSave::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("File")
			.beginClass<AppSave>("AppSave")
				.addConstructor <void (*) (std::string)>()
				.addFunction("LoadFile", &AppSave::LoadFile)
				.addFunction("SaveFile", &AppSave::SaveFile)
				.addFunction("AddParameter_string", &AppSave::AddParameter_string)
				.addFunction("AddParameter_int", &AppSave::AddParameter_int)
				.addFunction("AddParameter_float", &AppSave::AddParameter_float)
				.addFunction("AddParameter_bool", &AppSave::AddParameter_bool)
				.addFunction("GetParameterString", &AppSave::GetParameterString)
				.addFunction("GetParameterint", &AppSave::GetParameterint)
				.addFunction("GetParameterfloat", &AppSave::GetParameterfloat)
				.addFunction("GetParameterBool", &AppSave::GetParameterBool)
				.addData("filePath", &AppSave::filePath)
				.addData("save_on_close", &AppSave::save_on_close)
			.endClass()
		.endNamespace();
}
//static int kc = ExtensionManager::Register(AppSave::Extend);
