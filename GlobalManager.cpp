#include "GlobalManager.h"
//#include <algorithm>
#include <imgui.h>
GlobalManager* GlobalManager::Instance = NULL;

GlobalManager::GlobalManager()
{
}

GlobalManager::~GlobalManager()
{
}

GlobalManager* GlobalManager::Get()
{
	if (Instance == NULL)
	{
		Instance = new GlobalManager();
	}
	return Instance;
}

void GlobalManager::Begin()
{
	std::vector<ManagerEntry> entries;

	for (int i = 0; i < gManagerCount; i++)
		entries.push_back(gManagerList[i]);

	for (auto&& i : entries)
		i.creationFunction();
}

void GlobalManager::End()
{
	delete Instance;
	Instance = NULL;
}

void GlobalManager::Set()
{

	for (auto& system_pair : Get()->mManagers)
	{
		system_pair.second->init();
	}
}

void GlobalManager::Reset()
{
	for (auto& system_pair : Get()->mManagers)
	{
		system_pair.second->clear();
	}

}

void GlobalManager::_update(const float& dt)
{
	for (auto& system_pair : this->mManagers)
	{
		system_pair.second->Update(dt);
	}
}

void GlobalManager::_draw(Camera2D& cam)
{
	for (auto& system_pair : this->mManagers)
	{
		system_pair.second->CameraDraw(cam);
	}
}

void GlobalManager::_canvasDraw()
{
	for (auto& system_pair : this->mManagers)
	{
		system_pair.second->CanvasDraw();
	}
}

void GlobalManager::debug(const char* title)
{
	//if (ImGui::TreeNode(title))
	if (ImGui::BeginTabItem(title))
	{
		for (auto& system_pair : this->mManagers)
		{
			system_pair.second->Debug();
		}
		ImGui::EndTabItem();
		//ImGui::TreePop();
	}
}
