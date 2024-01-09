#include "Scene2d.h"

static Scene* GenPlatformer() { return Scene2d::Create("Scripts/Scenes/platformer.lua"); }
static int scene000 = RegisterScene("Test", "Platforming", GenPlatformer);

static Scene* GenerateMC() { return Scene2d::Create("Scripts/Scenes/MissleCommander.lua"); }
static int scene001 = RegisterScene("Test", "Missle Commander (2)", GenerateMC);