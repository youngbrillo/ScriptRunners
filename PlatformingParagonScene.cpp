#include "Scene2d.h"

class PlatformingParagonScene : Scene2d
{
public:
	PlatformingParagonScene(const char* Path);// : Scene2d(Path) {};
	~PlatformingParagonScene();


   static Scene* Create(const char* path) { return new Scene2d(path); }

};

PlatformingParagonScene::PlatformingParagonScene(const char* Path)
	:Scene2d(Path)
{
}

PlatformingParagonScene::~PlatformingParagonScene()
{
}

static Scene* GenPlatformer() { return PlatformingParagonScene::Create("Scripts/Scenes/platformer.lua"); }
static int scene000 = RegisterScene("Platforming", "Example", GenPlatformer);
