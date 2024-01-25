#include "Scene2d.h"
#include "SceneManager.h"
#include <filesystem>
#include <string>

static int scene000 = RegisterScene("Demo", "[lua] Missle Commander", Scene2d::Create, "Scripts/Scenes/MissleCommander.lua");
static int scene001 = RegisterScene("Demo", "Platforming", Scene2d::Create, "Scripts/Scenes/platformer.lua");
static int scene002 = RegisterScene("Demo", "Platforming II", Scene2d::Create, "Scripts/Scenes/platformerII.lua");
static int scene003 = RegisterScene("Demo", "Platforming III", Scene2d::Create, "Scripts/Scenes/platformerIII.lua");
static int scene004 = RegisterScene("Demo", "tile map", Scene2d::Create, "Scripts/Scenes/tilemaps.lua");

int GetScenesFromDirectory(const char* directory, const char* category)
{
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory))
	{
		std::string filepath = entry.path().string();
		std::string alias = entry.path().stem().string();
		printf("File:\t'%s' (%s)\n", alias.c_str(), filepath.c_str());
		int k = RegisterScene(category, alias.c_str(), Scene2d::Create, filepath.c_str());
	}
	return 0;
}

static int srsAll = GetScenesFromDirectory("Scripts/Scenes/ScriptRunners/", "Script Runners");