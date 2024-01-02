#include "Manager.h"

ManagerEntry gManagerList[MAX_MANAGERS] = { {nullptr} };
int gManagerCount = 0;

void Manager::Debug()
{
    if (ImGui::TreeNode(title.c_str()))
    {
        this->innerdebug();
        ImGui::TreePop();
    }
}

int RegisterManager(const char* Name, const char* config, const char* script, ManagerCreationFunction func)
{
	int index = gManagerCount;
	if (index < MAX_MANAGERS)
	{
		gManagerList[index] = { Name, config, script, func };
		++gManagerCount;
		return index;
	}
	return -1;
}
