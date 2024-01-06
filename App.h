#pragma once
#include "AppSettings.h"
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
	void Debug() {
		ImGui::SliderFloat("Hertz", &hertz_target, 0.0f, 1.0f);
	}
};
struct SceneManager
{
	std::map<std::string, std::vector<std::string>> scenes_by_category;
	std::map<std::string, int> scene_to_globalIndex;
};
class App
{
public:
	static void Start();
	static void Run();
	static void End();
	static App* Get();
	static Scene* GetCurrentScene() { return Get()->currentScene; }
	static void QuitApp() { Get()->settings.quit = true; }
	static void RestartScene() { Get()->settings.restart = true; }
	static AppState_ GetState() { return Get()->settings.state; }
	static AppState_ SetState(AppState_ s) { return Get()->settings.state = s; }
private:
	App();
	~App();
	void Initialize();
	void AdvanceFrame();
	void Update(const float& deltaTime);
	void FixedUpdate(const float& timeStamp);
	void Render();
	void PollEvents();
	void startScene(int index);
	void Debug();
	void DebugSettings();
	void DebugComponents();
	static App* Instance;

	FrameData fData;
	Inspector inspector;
	AppSettings settings;
	SceneManager sceneManager;
	Scene* currentScene;
};

