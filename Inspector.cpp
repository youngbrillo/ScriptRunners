#include "Inspector.h"
#include "App.h"
#include "AppSettings.h"
#include <imgui.h>
Inspector::Inspector()
	: menu_visible(true)
	, view_demo(false)
	, gameObjects({})
{
}

Inspector::~Inspector()
{
}

void Inspector::Render(AppSettings* settings, SceneManager* sm, GameObjectManager* gom)
{
	if (!this->menu_visible) return;

	if (ImGui::BeginMainMenuBar())
	{
		InspectWindows(settings);
		InspectScenes(sm, settings);
		InspectObjects(gom);

		ImGui::EndMainMenuBar();
	}

	if(this->view_demo) ImGui::ShowDemoWindow(&this->view_demo);
}

void Inspector::poll()
{
	if (IsKeyReleased(KEY_F1))
	{
		this->menu_visible = !this->menu_visible;
	}

}

void Inspector::InspectWindows(AppSettings* settings)
{
	if (ImGui::BeginMenu("File"))
	{
		ImGui::MenuItem("New", NULL, false, false);
		if (ImGui::MenuItem("Open", "Ctrl+O", false, false)) {}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {
			if (settings) settings->SaveData();
		}
		if (ImGui::MenuItem("Save As..", NULL, false, false)) {}
		if (ImGui::MenuItem("Restart", "Ctrl+R", false, true)) { 
			if(settings) settings->restart = true; 
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Quit", "Alt+F4")) { 
			if (settings) settings->quit = true;
		}
		if (ImGui::MenuItem("Hide Menu", "F1")) { 
			this->menu_visible = false; 
		}
		if (ImGui::MenuItem(view_demo ?"Hide Demo": "View Demo", NULL, view_demo)){
			this->view_demo = !this->view_demo;
		}
		//settings->Debug();

		ImGui::EndMenu();
	}

}
#include "SceneManager.h"
void Inspector::InspectScenes(SceneManager* sm, AppSettings* s)
{
#if false
	if (ImGui::BeginMenu("Scenes"))
	{
		if (!!sm)
		{
			for (auto&& cat : sm->scenes_by_category)
			{
				if (ImGui::BeginMenu(cat.first.c_str()))
				{
					for (auto&& scene : cat.second)
					{
						if (ImGui::MenuItem(scene.c_str(), TextFormat("id: %d", sm->scene_to_globalIndex[scene]), sm->scene_to_globalIndex[scene] == s->lastScene_id))
						{
							s->lastScene_id = sm->scene_to_globalIndex[scene];
							s->last_scene_name = scene;
							s->last_scene_cat = cat.first;
							s->restart = true;
						}
					}
					ImGui::EndMenu();
				}
			}
		}
		ImGui::EndMenu();
	}
#else
	if (GlobalSceneManager::Get()->Inspect(s->lastScene_id, "Scenes"))
	{
		SceneEntryPro* entry = GlobalSceneManager::GetScene(s->lastScene_id);
		s->last_scene_name = entry->name;
		s->last_scene_cat = entry->category;
		s->restart = true;
	}
#endif
}

void Inspector::InspectObjects(GameObjectManager* objectManager)
{
	if (ImGui::BeginMenu("Game Objects"))
	{

		ImGui::EndMenu();
	}
}

void Inspector::InspectGameObject(unsigned int gameObject)
{
}
