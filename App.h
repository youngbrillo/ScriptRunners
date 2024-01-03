#pragma once
#include "RenderPipeline.h"
#include "Inspector.h"
#include <string>
#include "Scene.h"

#include <map>
#include <vector>
#include <string>

struct FrameData
{
	float currentFrame = 0.0f;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float accumalator = 0.0f;
	float hertz_progress = 0.0f;
	float hertz_target = 1.0f / 60.0f;

	void Advance();
};
enum AppState_
{
	AppState_Play = 0,
	AppState_FullPause,
	AppState_PartialPause
};

struct PauseSettings
{
	float stepTime = 0.1f;
	float stepProg = 0.0f;
	bool inStep = false;
	bool quit = false;
	bool restart = false;
	int pause_key = KEY_P;
	int ctrl_key = KEY_LEFT_CONTROL;
	int step_key = KEY_RIGHT_BRACKET;

	void BeginFrame(AppState_& state) { if (inStep) { state = AppState_Play; } }
	void EndFrame(AppState_& state, const float& dt){
		if(inStep && state == AppState_Play && stepProg < stepTime){
			stepProg += dt;
			if (stepProg >= stepTime)
			{
				stepProg = 0.0f;
				inStep = false;
				state = AppState_FullPause;
			}
		} 
	}
	void PollEvents(AppState_& state)
	{
		if (IsKeyReleased(step_key) && state != AppState_Play)
		{
			this->inStep = true;
		}
	}
};
struct WindowSettings
{
	int width = 800;
	int height = 400;
	Color color = RAYWHITE;
	std::string title = "Template";

	bool undecorated = false;
	bool resizeable = true;
	bool transparent = false;

	int exitKey = KEY_ESCAPE;
	bool quit = false;
	bool restart_scene = false;
	AppState_ state = AppState_Play;
	bool autoSave = true;

	int getFlags()
	{
		int flags = 0;
		if (resizeable) flags = flags | FLAG_WINDOW_RESIZABLE;
		if (undecorated) flags = flags | FLAG_WINDOW_UNDECORATED;
		if (transparent) flags = flags | FLAG_WINDOW_TRANSPARENT;
		return flags;
	}
	void debug();
};

struct SceneManager
{
	std::map<std::string, std::vector<std::string>> scenes_by_category;
	std::map<std::string, int> scene_to_globalIndex;
	int currentSceneId = 0;
	bool restart = false;
};
class App
{
public:
	static void Start(const char* Title, const char* settings_path, const char* icon_path);
	static void Run();
	static void End();
	static App* Get();
	static Scene* GetCurrentScene() { return Get()->currentScene; }
	static void QuitApp() { Get()->winSettings.quit = true; }
	static AppState_ GetState() { return Get()->winSettings.state; }
	static AppState_ SetState(AppState_ s) { return Get()->winSettings.state = s; }
private:
	App();
	~App();
	void Initialize(const char* settings_path);
	void AdvanceFrame();
	void Update(const float& deltaTime);
	void FixedUpdate(const float& timeStamp);
	void Render();
	void PollEvents();
	void startScene(int index);
	void Debug();
	void DebugSettings();
	void DebugComponents();
	void LoadSettings(const char* inifilepath);
	void SaveSettings(const char* inifilepath);
	static App* Instance;

	FrameData fData;
	WindowSettings winSettings;
	Inspector inspector;
	SceneManager sceneManager;
	PauseSettings mPauseSettings;
	Scene* currentScene;
};

