#include "Scene2d.h"
#include "PlatformerController.h"
class PlatformingParagonScene : Scene2d
{
public:
	PlatformingParagonScene(const char* Path);// : Scene2d(Path) {};
	~PlatformingParagonScene();

    virtual void Initialize() override;

    //virtual void Update(const float& deltaTime);
    //virtual void FixedUpdate(const float& timeStep);
    //virtual void Draw();
    //virtual void Debug() override;
    //virtual void PollEvents() override;


   static Scene* Create(const char* path) { return new PlatformingParagonScene(path); }
};

PlatformingParagonScene::PlatformingParagonScene(const char* Path)
	:Scene2d(Path)
{
}

PlatformingParagonScene::~PlatformingParagonScene()
{
}
void PlatformingParagonScene::Initialize()
{
    Scene2d::Initialize();
    Nodes.emplace_back(std::make_shared<ECS::PlatformerController>());
}

static Scene* GenPlatformer() { return PlatformingParagonScene::Create("Scripts/Scenes/platformer.lua"); }
static int scene000 = RegisterScene("Platforming", "Example", GenPlatformer);
