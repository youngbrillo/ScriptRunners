#include "Inspector.h"
#include "App.h"
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

void Inspector::Render(WindowSettings* settings, SceneManager* sm, GameObjectManager* gom)
{
	if (!this->menu_visible) return;

	if (ImGui::BeginMainMenuBar())
	{
		InspectWindows(settings);
		InspectScenes(sm);
		InspectObjects(gom);

		ImGui::EndMainMenuBar();
	}

	if(view_demo) ImGui::ShowDemoWindow(&view_demo);
}

void Inspector::poll()
{
	if (IsKeyReleased(KEY_F1))
	{
		this->menu_visible = !this->menu_visible;
	}

}

void Inspector::InspectWindows(WindowSettings* settings)
{
	if (ImGui::BeginMenu("File"))
	{
		ImGui::MenuItem("New", NULL, false, false);
		if (ImGui::MenuItem("Open", "Ctrl+O", false, false)) {}
		if (ImGui::MenuItem("Save", "Ctrl+S", false, false)) {}
		if (ImGui::MenuItem("Save As..", NULL, false, false)) {}
		if (ImGui::MenuItem("Restart", "Ctrl+R", false, true)) { 
			if(settings) settings->restart_scene = true; 
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
		settings->debug();

		ImGui::EndMenu();
	}

}

void Inspector::InspectScenes(SceneManager* sm)
{
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
						if (ImGui::MenuItem(scene.c_str(), TextFormat("id: %d", sm->scene_to_globalIndex[scene]), sm->scene_to_globalIndex[scene] == sm->currentSceneId))
						{
							sm->currentSceneId = sm->scene_to_globalIndex[scene];
							sm->restart = true;
						}
					}
					ImGui::EndMenu();
				}
			}
		}
		ImGui::EndMenu();
	}
}

void Inspector::InspectObjects(GameObjectManager* objectManager)
{
	if (ImGui::BeginMenu("Scenes"))
	{

		ImGui::EndMenu();
	}
}

void Inspector::InspectGameObject(unsigned int gameObject)
{
}
