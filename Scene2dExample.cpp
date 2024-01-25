#include "Scene2d.h"
#include "SceneManager.h"
static Scene* GenerateMC01() { return Scene2d::Create("Scripts/Scenes/MissleCommander.lua"); }
static Scene* GenPlatformer() { return Scene2d::Create("Scripts/Scenes/platformer.lua"); }
static Scene* GenPlatformerII() { return Scene2d::Create("Scripts/Scenes/platformerII.lua"); }
static Scene* GenPlatformerIII() { return Scene2d::Create("Scripts/Scenes/platformerIII.lua"); }
static Scene* GenTilemap() { return Scene2d::Create("Scripts/Scenes/tilemaps.lua"); }

static int scene000 = RegisterScene("Demo", "[lua] Missle Commander", Scene2d::Create, "Scripts/Scenes/MissleCommander.lua");
static int scene001 = RegisterScene("Demo", "Platforming", Scene2d::Create, "Scripts/Scenes/platformer.lua");
static int scene002 = RegisterScene("Demo", "Platforming II", Scene2d::Create, "Scripts/Scenes/platformerII.lua");
static int scene003 = RegisterScene("Demo", "Platforming III", Scene2d::Create, "Scripts/Scenes/platformerIII.lua");
static int scene004 = RegisterScene("Demo", "tile map", Scene2d::Create, "Scripts/Scenes/tilemaps.lua");

static int srs000 = RegisterScene("Script Runners", "ToDo", Scene2d::Create, "Scripts/Scenes/ScriptRunners/todo.lua");
