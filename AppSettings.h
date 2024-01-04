#pragma once
#include "RenderPipeline.h"
#include <imgui.h>
#include <string>

enum AppState_
{
	AppState_Play = 0,
	AppState_FullPause,
	AppState_PartialPause
};

class AppSettings
{
public:
	//General Settings
	bool restart = false;
	bool quit = false;
	bool autoSave = true;
public: //scene settings
	AppState_ state = AppState_Play;
	int lastScene_id = 0;
	std::string last_scene_name = "";
	std::string last_scene_cat = "";
	std::string configPath = "./Configs/window.json";
public:

public: //pause settings
	float stepTime = 0.1f;
	float stepProg = 0.0f;
	bool inStep = false;

public: //Keys
	int exitKey = KEY_ESCAPE;
	int pause_key = KEY_P;
	int ctrl_key = KEY_LEFT_CONTROL;
	int step_key = KEY_RIGHT_BRACKET;


public:  //window settings

	std::string wintitle = "Template";
	int window_width = 800;
	int window_height = 400;

	bool win_resizeable = true;
	bool win_undecorated = false;
	bool win_transparent = false;

	Color window_color = RAYWHITE;

public: //functions

	void BeginFrame();
	void EndFrame(const float& dt);
	int windowFlags();
	void Poll();
	void Debug();
	void LoadData();
	void SaveData();
	void AutoSave();
};

