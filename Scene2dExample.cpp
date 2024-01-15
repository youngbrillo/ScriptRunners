#include "Scene2d.h"
#include "SceneManager.h"
static Scene* GenPlatformer() { return Scene2d::Create("Scripts/Scenes/platformer.lua"); }


static int scene000 = RegisterScene("MC Demo", "Platforming", GenPlatformer);

static Scene* GenerateMC01() { return Scene2d::Create("Scripts/Scenes/MissleCommander.lua"); }
static int scene001 = RegisterScene("MC Demo", "Action", GenerateMC01);


static Scene* GenerateMC02() { return Scene2d::Create("Scripts/Scenes/Action.lua"); }
static int scene002 = RegisterScene("MC Demo", "Action-Test", GenerateMC02);

//static int scene003 = GlobalSceneManager::RegisterScene("Demo", "Platformer", Scene2d::Create, "Scripts/Scenes/platformer.lua");