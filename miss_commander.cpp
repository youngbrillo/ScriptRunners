#include "Scene2d.h"

static Scene* GenPlatformer() { return Scene2d::Create("Scripts/Scenes/platformer.lua"); }
static int scene000 = RegisterScene("Platforming", "Example", GenPlatformer);
