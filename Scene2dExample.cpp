#include "Scene2d.h"
#include "SceneManager.h"
static Scene* GenerateMC01() { return Scene2d::Create("Scripts/Scenes/MissleCommander.lua"); }
static Scene* GenPlatformer() { return Scene2d::Create("Scripts/Scenes/platformer.lua"); }
static Scene* GenPlatformerII() { return Scene2d::Create("Scripts/Scenes/platformerII.lua"); }
static Scene* GenPlatformerIII() { return Scene2d::Create("Scripts/Scenes/platformerIII.lua"); }
static Scene* GenTilemap() { return Scene2d::Create("Scripts/Scenes/tilemaps.lua"); }

static int scene000 = RegisterScene("Demo", "[lua] Missle Commander", GenerateMC01);
static int scene001 = RegisterScene("Demo", "Platforming", GenPlatformer);
static int scene002 = RegisterScene("Demo", "Platforming II", GenPlatformerII);
static int scene003 = RegisterScene("Demo", "Platforming III", GenPlatformerIII);
static int scene004 = RegisterScene("Demo", "tile map", GenTilemap);


//static int scene003 = GlobalSceneManager::RegisterScene("Demo", "Platformer", Scene2d::Create, "Scripts/Scenes/platformer.lua");