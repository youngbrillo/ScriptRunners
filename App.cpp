#include "App.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <cassert>
#include "rlImGui.h"
#include "GlobalManager.h"
#include "SceneManager.h"

App* App::Instance = NULL;
void App::Start()
{
	assert(App::Instance == NULL && "Cannot Call Application Start once an Instance has been created!");
	Get()->Initialize();
}

void App::Run()
{
    //std::cout << "Running Project" << std::endl;
	while (Get()->settings.quit == false && WindowShouldClose() != true)
	{
		Get()->AdvanceFrame();
	}
}

void App::End()
{
    std::cout << "Closing Project" << std::endl;
	CloseWindow();
	delete Instance;
	Instance = NULL;
	GlobalManager::End();
}

App* App::Get()
{
	if (Instance == NULL)
	{
		Instance = new App();
	}
	return Instance;
}

void App::GoToScene(std::string name)
{
#if false
	if (Get()->sceneManager.scene_to_globalIndex.find(name) !=
		Get()->sceneManager.scene_to_globalIndex.end())
	{
		int id = Get()->sceneManager.scene_to_globalIndex[name];
		Get()->settings.lastScene_id = id;
		Get()->settings.restart = true;
	}
#else

	SceneEntryPro* entry = GlobalSceneManager::GetScene(name.c_str());
	if (!!entry)
	{
		int id = entry->id;
		Get()->settings.lastScene_id = id;
		Get()->settings.restart = true;
	}

#endif


}

App::App()
	: fData()
	, currentScene(NULL)
{
}

App::~App()
{
	if (!!currentScene) delete currentScene;
	currentScene = NULL;
	settings.AutoSave();
}

void App::Initialize()
{	
	settings.LoadData();
	SetConfigFlags(settings.windowFlags());
	InitWindow(settings.window_width, settings.window_height, settings.wintitle.c_str());

	rlImGuiSetup(true);
	SetExitKey(settings.exitKey);

	GlobalManager::Begin();
	startScene(settings.lastScene_id);
}
void FrameSettings::Advance()
{
	this->currentFrame = (float)glfwGetTime();
	this->deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	//accumalator += deltaTime;
}



void App::AdvanceFrame()
{
	fData.Advance();
	settings.BeginFrame();
	this->Update(fData.deltaTime);
	this->Render();
	this->PollEvents();
	settings.EndFrame(fData.deltaTime);
}

void App::Update(const float& deltaTime)
{
	if (settings.state == AppState_FullPause) return;
	fData.hertz_progress += deltaTime;

	if (!!currentScene) currentScene->Update(deltaTime);
	GlobalManager::Update(deltaTime);


	if (fData.hertz_progress >= fData.hertz_target)
	{
		this->FixedUpdate(fData.hertz_target);
		fData.hertz_progress = 0.0f;
	}

}

void App::FixedUpdate(const float& timeStamp)
{
	if (settings.state == AppState_PartialPause) return;
	if (!!currentScene) currentScene->FixedUpdate(timeStamp);

}

void App::Render()
{
	BeginDrawing();
	ClearBackground(settings.window_color);
	if (!!currentScene) currentScene->Draw();
	GlobalManager::CanvasDraw();
	Debug();
	EndDrawing();
}

void App::PollEvents()
{
	settings.Poll();
	if (!!currentScene) currentScene->PollEvents();

	inspector.poll();

	if (settings.restart)
	{
		this->startScene(settings.lastScene_id);
		settings.restart = false;
	}
}

void App::startScene(int index)
{
	settings.lastScene_id = index;
	if (!!currentScene)
	{
		delete currentScene;
		currentScene = NULL;
		settings.AutoSave();
		GlobalManager::Reset();
	}
#if false
	if (GlobalSceneCount > 0 && index > -1 && index < GlobalSceneCount)
	{
		GlobalManager::Set();
		this->currentScene = GlobalSceneList[index].creationFunction();
	}
#else
	SceneEntryPro* entry = GlobalSceneManager::GetScene(index);
	if (!!entry)
	{
		GlobalManager::Set();
		this->currentScene = entry->func(entry->path);

		settings.state = AppState_Play;
		this->currentScene->Initialize();
	}

#endif
}



void App::Debug()
{
	rlImGuiBegin();
	inspector.Render(&this->settings, NULL, NULL);

	ImGui::Begin(TextFormat("%s Debug", settings.wintitle.c_str()));
	settings.Debug();
	this->DebugSettings();
	this->DebugComponents();

	ImGui::End();
	rlImGuiEnd();
}
void App::DebugSettings()
{
	ImVec2 button_sz = ImVec2(-1, 0);
	if (ImGui::Button(this->inspector.menu_visible ? "Hide Menu (F1)" : "Display Menu (F1)", button_sz))
	{
		this->inspector.menu_visible = !this->inspector.menu_visible;
	}

}


void App::DebugComponents()
{
	if (ImGui::BeginTabBar("ControlTabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Controls"))
		{
			ImGui::Separator();
			if (!!currentScene) currentScene->Debug();
			ImGui::EndTabItem();
		}
		GlobalManager::Debug("Global Manager");
		ImGui::EndTabBar();
	}
}


