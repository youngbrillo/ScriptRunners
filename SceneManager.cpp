#include "SceneManager.h"

GlobalSceneManager* GlobalSceneManager::Instance = NULL;

int GlobalSceneManager::RegisterScene(const char* category, const char* name, const char* path, std::function<Scene* (const char*)> func)
{
	SceneEntryPro entry{ category, name, path, func };
	return Get()->AddScene(entry);
}

 SceneEntryPro* GlobalSceneManager::GetScene(const char* name)
{
	 return Get()->get_scene(name);
}

 SceneEntryPro* GlobalSceneManager::GetScene(const int& id)
{
	 return Get()->get_scene(id);
}

GlobalSceneManager* GlobalSceneManager::Get()
{
	if (Instance == NULL)
	{
		Instance = new GlobalSceneManager();
	}
	return Instance;
}
GlobalSceneManager::GlobalSceneManager()
	: sceneCount(0)
{

}

GlobalSceneManager::~GlobalSceneManager()
{
}

int GlobalSceneManager::AddScene(SceneEntryPro& entry)
{
	int id = this->sceneCount++;
	entry.id = id;
	entries[id] = entry;
	entry_name_lookup[entry.name] = id;
	entry_name_reverse_lookup[id] = entry.name;
	category_entry_lookup[entry.category].push_back(id);

	return id;
}

SceneEntryPro* GlobalSceneManager::get_scene(const char* name)
{
	for (auto&& pair : entry_name_lookup)
	{
		if (strcmp(name, pair.first) == 0)
		{
			return &entries.at(entry_name_lookup.at(pair.first));
		}
	}
	return nullptr;
}

SceneEntryPro* GlobalSceneManager::get_scene(const int& id)
{
	if (entries.find(id) != entries.end())
	{
		return &entries.at(id);
	}
	return nullptr;
}
#include  <imgui.h>

bool GlobalSceneManager::Inspect(int& current_scene, const char* title)
{
	bool changed = false;
	if (ImGui::BeginMenu("Scenes"))
	{
		for (auto&& pair : category_entry_lookup)
		{
			if (ImGui::BeginMenu(pair.first))
			{
				for (auto&& scene_id : pair.second)
				{
					if (ImGui::MenuItem(entry_name_reverse_lookup[scene_id], TextFormat("id: %d", scene_id), current_scene == scene_id))
					{
						changed = true;
						current_scene = scene_id;
					}
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenu();
	}
	return changed;
}