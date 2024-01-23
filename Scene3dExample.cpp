#include "Scene3d.h"


class PSXStyledScene : public Scene3d
{
public:
	PSXStyledScene(const char* title);
	~PSXStyledScene();


	static Scene* Register(const char* path) { return new PSXStyledScene(""); }
};
PSXStyledScene::PSXStyledScene(const char* title)
	: Scene3d(title)
{
	//add some cubes
	ECS::Node3d a;
	a.name = "Cube"; a.position = Vector3{ 10.0f, 0.0f, 10.0f }; a.modelId = 1; Nodes.push_back(a);
	a.name = "church";		a.scale = 0.25f; a.position = Vector3{20.0f, 0.0f, 0.0f}; a.modelId = 3; Nodes.push_back(a);
	a.name = "barracks";	a.scale = 0.25f; a.position = Vector3{30.0f, 0.0f, 0.0f}; a.modelId =4; Nodes.push_back(a);
	a.name = "watermill";	a.scale = 0.25f; a.position = Vector3{40.0f, 0.0f, 0.0f}; a.modelId = 5; Nodes.push_back(a);
	a.name = "bridge";		a.scale = 0.25f; a.position = Vector3{50.0f, 0.0f, 0.0f}; a.modelId = 6; Nodes.push_back(a);
	a.name = "castle";		a.scale = 0.25f; a.position = Vector3{60.0f, 0.0f, 0.0f}; a.modelId = 7; Nodes.push_back(a);
	a.name = "house";		a.scale = 0.25f; a.position = Vector3{70.0f, 0.0f, 0.0f}; a.modelId = 8; Nodes.push_back(a);
	a.name = "market";		a.scale = 0.25f; a.position = Vector3{80.0f, 0.0f, 0.0f}; a.modelId = 9; Nodes.push_back(a);
	a.name = "plane";		a.scale = 0.25f; a.position = Vector3{90.0f, 0.0f, 0.0f}; a.modelId = 10; Nodes.push_back(a);
	a.name = "turret";		a.scale = 0.25f; a.position = Vector3{100.0f, 0.0f, 0.0f}; a.modelId = 11; Nodes.push_back(a);
	a.name = "well";		a.scale = 0.25f; a.position = Vector3{110.0f, 0.0f, 0.0f}; a.modelId = 12; Nodes.push_back(a);
	
}

PSXStyledScene::~PSXStyledScene()
{
}


static int scene000 = RegisterScene("3D", "PSX style", PSXStyledScene::Register);
