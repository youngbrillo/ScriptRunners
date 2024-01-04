#include "App.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <cassert>
#include "rlImGui.h"
#include "GlobalManager.h"

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
	GlobalManager::End();
	delete Instance;
	Instance = NULL;
}

App* App::Get()
{
	if (Instance == NULL)
	{
		Instance = new App();
	}
	return Instance;
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


	for (int i = 0; i < GlobalSceneCount; i++)
	{
		sceneManager.scenes_by_category[GlobalSceneList[i].category].push_back(GlobalSceneList[i].name);
		sceneManager.scene_to_globalIndex[GlobalSceneList[i].name] = i;
	}

	GlobalManager::Begin();
	startScene(settings.lastScene_id);
}
void FrameData::Advance()
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
	}

	if (GlobalSceneCount > 0 && index > -1 && index < GlobalSceneCount)
	{
		this->currentScene = GlobalSceneList[index].creationFunction();
	}

	settings.state = AppState_Play;
}



void App::Debug()
{
	rlImGuiBegin();
	inspector.Render(&this->settings, &this->sceneManager, NULL);

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

		ImGuiTreeNodeFlags leafNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		leafNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (ImGui::BeginTabItem("Scenes"))
		{
			int categoryIndex = 0;
			const char* category = GlobalSceneList[categoryIndex].category;
			int i = 0;
			while (i < GlobalSceneCount)
			{
				bool categorySelected = strcmp(category, GlobalSceneList[settings.lastScene_id].category) == 0;
				ImGuiTreeNodeFlags nodeSelectionFlags = categorySelected ? ImGuiTreeNodeFlags_Selected : 0;
				bool nodeOpen = ImGui::TreeNodeEx(category, nodeFlags | nodeSelectionFlags);

				if (nodeOpen)
				{
					while (i < GlobalSceneCount && strcmp(category, GlobalSceneList[i].category) == 0)
					{
						ImGuiTreeNodeFlags selectionFlags = 0;
						if (settings.lastScene_id == i)
						{
							selectionFlags = ImGuiTreeNodeFlags_Selected;
						}
						ImGui::TreeNodeEx((void*)(intptr_t)i, leafNodeFlags | selectionFlags, "%s", GlobalSceneList[i].name);
						if (ImGui::IsItemClicked())
						{
							settings.lastScene_id = i;
							this->startScene(i);
						}
						++i;
					}
					ImGui::TreePop();
				}
				else
				{
					while (i < GlobalSceneCount && strcmp(category, GlobalSceneList[i].category) == 0)
					{
						++i;
					}
				}

				if (i < GlobalSceneCount)
				{
					category = GlobalSceneList[i].category;
					categoryIndex = i;
				}
			}
			ImGui::EndTabItem();
		}
		GlobalManager::Debug("Global Manager");
		ImGui::EndTabBar();
	}
}


