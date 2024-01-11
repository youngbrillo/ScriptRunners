#include "Scene2d.h"

static Scene* GenPlatformer() { return Scene2d::Create("Scripts/Scenes/platformer.lua"); }
static int scene000 = RegisterScene("Test", "Platforming", GenPlatformer);

static Scene* GenerateMC01() { return Scene2d::Create("Scripts/Scenes/MissleCommander.lua"); }
static int scene001 = RegisterScene("M.Commander", "Action", GenerateMC01);


static Scene* GenerateMC02() { return Scene2d::Create("Scripts/Scenes/Action.lua"); }
static int scene002 = RegisterScene("M.Commander", "Action-Test", GenerateMC02);

