#pragma once
#include <string>
#include "RenderPipeline.h"
#include <imgui.h>
#define MAX_MANAGERS 16

class Manager
{
public:
	Manager(std::string title_, std::string configPath_, std::string scriptPath_)
		: title(title_)
		, configPath(configPath_)
		, scriptPath(scriptPath_)
		, isInitialized(false)
	{
	}
	virtual ~Manager() {}

	virtual void init() {}
	virtual void clear() {};

public:
	virtual void Update(const float& dt) {};
	virtual void CameraDraw(Camera2D& camera) {};
	virtual void CanvasDraw() {};
	void Debug();
protected:
	virtual void innerdebug() {};
	std::string title, configPath, scriptPath;
	bool isInitialized;
};

typedef Manager ManagerCreationFunction();
int RegisterManager(const char* Name, const char* config, const char* script, ManagerCreationFunction func);

struct ManagerEntry
{
	const char* name;
	const char* config_path;
	const char* script_path;
	ManagerCreationFunction* creationFunction;
};

extern	ManagerEntry gManagerList[MAX_MANAGERS];
extern	int gManagerCount;