#include "App.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <cassert>
#include "rlImGui.h"

App* App::Instance = NULL;
void App::Start(const char* Title, const char* settings_path, const char* icon_path)
{
	assert(App::Instance == NULL && "Cannot Call Application Start once an Instance has been created!");
	std::cout << "Begining Project: " << Title << std::endl;
	Get()->Initialize(settings_path);
}

void App::Run()
{
    //std::cout << "Running Project" << std::endl;
	while (Get()->winSettings.quit == false && WindowShouldClose() != true)
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

	if (winSettings.autoSave) this->SaveSettings("./Configs/window.json");
	
}

void App::Initialize(const char* settings_path)
{
	this->LoadSettings(settings_path);

	SetConfigFlags(winSettings.getFlags());
	InitWindow(winSettings.width, winSettings.height, winSettings.title.c_str());

	rlImGuiSetup(true);
	SetExitKey(winSettings.exitKey);


	for (int i = 0; i < GlobalSceneCount; i++)
	{
		sceneManager.scenes_by_category[GlobalSceneList[i].category].push_back(GlobalSceneList[i].name);
		sceneManager.scene_to_globalIndex[GlobalSceneList[i].name] = i;
	}

	startScene(sceneManager.currentSceneId);
}
void FrameData::Advance()
{
	this->currentFrame = (float)glfwGetTime();
	this->deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	accumalator += deltaTime;
}


void App::AdvanceFrame()
{
	fData.Advance();
	mPauseSettings.BeginFrame(winSettings.state);
	this->Update(fData.deltaTime);
	this->Render();
	this->PollEvents();
	mPauseSettings.EndFrame(winSettings.state, fData.deltaTime);
}

void App::Update(const float& deltaTime)
{
	if (winSettings.state == AppState_FullPause) return;
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
	if (winSettings.state == AppState_PartialPause) return;
	if (!!currentScene) currentScene->FixedUpdate(timeStamp);

}

void App::Render()
{
	BeginDrawing();
	ClearBackground(winSettings.color);
	if (!!currentScene) currentScene->Draw();
	Debug();
	EndDrawing();
}

void App::PollEvents()
{
	if (!!currentScene) currentScene->PollEvents();
	if (IsKeyReleased(winSettings.exitKey))
	{
		winSettings.quit = true;
	}

	inspector.poll();


	if (IsKeyReleased(KEY_R) && IsKeyDown(KEY_LEFT_CONTROL))
	{
		sceneManager.restart = true;
	}

	if (sceneManager.restart || winSettings.restart_scene)
	{
		this->startScene(sceneManager.currentSceneId);
		sceneManager.restart = false;
		winSettings.restart_scene = false;
	}

	if (IsKeyPressed(KEY_P))
	{
		this->winSettings.state = this->winSettings.state == AppState_Play ? AppState_FullPause : AppState_Play;
	}

	this->mPauseSettings.PollEvents(winSettings.state);
}

void App::startScene(int index)
{
	sceneManager.currentSceneId = index;
	if (!!currentScene)
	{
		delete currentScene;
		currentScene = NULL;
		this->SaveSettings("./Configs/window.json");
	}

	if (GlobalSceneCount > 0 && index > -1 && index < GlobalSceneCount)
	{
		this->currentScene = GlobalSceneList[index].creationFunction();
	}
	if (winSettings.autoSave)
	{
	}
	winSettings.state = AppState_Play;
}



void App::Debug()
{
	rlImGuiBegin();
	inspector.Render(&this->winSettings, &this->sceneManager, NULL);

	ImGui::Begin(TextFormat("%s Debug", winSettings.title.c_str()));

	this->DebugSettings();
	this->DebugComponents();

	ImGui::End();
	rlImGuiEnd();
}
static const char* state_names[AppState_::AppState_PartialPause + 1] = { "Play",  "Full Pause", "Fixed Pause" };
void App::DebugSettings()
{
	//DRAW FRAMERATE
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//MODIFY THE WINDOW COLOR
	Vector4 winColor = ColorNormalize(winSettings.color);
	if (ImGui::ColorEdit4("Window Clear Color", &winColor.x))
	{
		winSettings.color = ColorFromNormalized(winColor);
	}
	//MODIFY WINDOW SETTINGS TODO: MOVE TO A MENU ITEM IN THE INSPECTOR
	if (ImGui::TreeNode("configs"))
	{
		ImGui::Checkbox("Auto Save", &winSettings.autoSave);
		bool fs0 = ImGui::Checkbox("resizeable", &winSettings.resizeable);
		bool fs1 = ImGui::Checkbox("undecorated", &winSettings.undecorated);
		bool fs2 = ImGui::Checkbox("transparent", &winSettings.transparent);
		if (fs0 || fs1 || fs2)
		{
			ClearWindowState(winSettings.getFlags());
			SetWindowState(winSettings.getFlags());
		}

		ImGui::TreePop();
	}
	if (ImGui::Button("Save Settings"))
	{
		this->SaveSettings("./Configs/window.json");
	}

	ImVec2 button_sz = ImVec2(-1, 0);
	ImVec2 button_sz_2 = ImVec2(0, 0);

	ImGui::SliderFloat("step limit", &mPauseSettings.stepTime, 0.01f, 0.25f);
	int elem = winSettings.state;

	if (ImGui::SliderInt("State", &elem, AppState_Play, AppState_PartialPause, state_names[winSettings.state]))
	{
		winSettings.state = (AppState_)elem;
	}
	bool isPaused = winSettings.state != AppState_Play;
	const char* pause_status = isPaused? "Play (P)" : "Pause (P)";
	if (ImGui::Button(pause_status, button_sz_2))
	{
		winSettings.state = isPaused ? AppState_Play : AppState_FullPause;
	}
	if (isPaused)
	{
		ImGui::SameLine();
		if (ImGui::Button("Step (])", button_sz_2))
		{
			mPauseSettings.inStep = true;
		}
	}
	if (ImGui::Button("Restart (LCTRL + R)", button_sz))
	{
		this->sceneManager.restart = true;
	}

	if (ImGui::Button("Quit (Q)", button_sz))
	{
		winSettings.quit = true;
	}

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
				bool categorySelected = strcmp(category, GlobalSceneList[sceneManager.currentSceneId].category) == 0;
				ImGuiTreeNodeFlags nodeSelectionFlags = categorySelected ? ImGuiTreeNodeFlags_Selected : 0;
				bool nodeOpen = ImGui::TreeNodeEx(category, nodeFlags | nodeSelectionFlags);

				if (nodeOpen)
				{
					while (i < GlobalSceneCount && strcmp(category, GlobalSceneList[i].category) == 0)
					{
						ImGuiTreeNodeFlags selectionFlags = 0;
						if (sceneManager.currentSceneId == i)
						{
							selectionFlags = ImGuiTreeNodeFlags_Selected;
						}
						ImGui::TreeNodeEx((void*)(intptr_t)i, leafNodeFlags | selectionFlags, "%s", GlobalSceneList[i].name);
						if (ImGui::IsItemClicked())
						{
							sceneManager.currentSceneId = i;
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

		ImGui::EndTabBar();
	}
}
#include "JsonHandler.h"

void App::LoadSettings(const char* filepath)
{
	jsonObjects obj;
	JSONParse::ParseJSONFile(obj, filepath);
	Vector4 bgColor = ColorNormalize(winSettings.color);
	obj.Getboolean("autoSave", winSettings.autoSave)
		.GetInteger("index", sceneManager.currentSceneId)
		.GetInteger("width", winSettings.width)
		.GetInteger("height", winSettings.height)
		.Getboolean("undecorated", winSettings.undecorated)
		.Getboolean("resizeable", winSettings.resizeable)
		.Getboolean("transparent", winSettings.transparent)
		.GetString("title", winSettings.title)
		.find("bgColor")
			.GetNumber("r", bgColor.x)
			.GetNumber("g", bgColor.y)
			.GetNumber("b", bgColor.z)
			.GetNumber("a", bgColor.w);

	winSettings.color = ColorFromNormalized(bgColor);
}

void App::SaveSettings(const char* filepath)
{
	jsonObjects obj;

	Vector4 bgColor = ColorNormalize(winSettings.color);
	obj
		.SetBooleanAt("autoSave", winSettings.autoSave)
		.SetIntegerAt("index", sceneManager.currentSceneId)
		.SetIntegerAt("width", winSettings.width)
		.SetIntegerAt("height", winSettings.height)
		.SetBooleanAt("undecorated", winSettings.undecorated)
		.SetBooleanAt("resizeable", winSettings.resizeable)
		.SetBooleanAt("transparent", winSettings.transparent)
		.findOrCreate("bgColor")
			.SetNumberAtAndAdd("r", bgColor.x)
			.SetNumberAtAndAdd("g", bgColor.y)
			.SetNumberAtAndAdd("b", bgColor.z)
			.SetNumberAtAndAdd("a", bgColor.w);

	obj.SaveToFile(filepath);

}

void WindowSettings::debug()
{
}
#include <sstream>
