#pragma once
#include "Scene.h"
#include "RenderPipeline.h"
#include <string>
#include <map>
#include <vector>

namespace ECS
{
	class Node3d
	{
	public:
		std::string name = "Node3d";
		unsigned int modelId = 0;
		bool visible = true;
		bool wiremode = false;
		Vector3 position = Vector3{ 0.0f, 0.0f, 0.0f };
		Vector3 size = { 1.0f, 1.0f, 1.0f };
		Vector3 rotationAxis = { 0.0f, 1.0f, 0.0f };
		float rotationAngle;
		float scale = 1.0f;
		Color color = RAYWHITE;

		void Inspect();
	};

	class liteModelManager
	{
	public:
		liteModelManager();
		~liteModelManager();

		std::map<unsigned int, Model> Models;
		std::map<unsigned int, std::string> ModelNames;
		std::map<unsigned int, Texture> DiffuseTextures;

		unsigned int AddModel(const char* name, const char* model_path, const char* texture_path);
		unsigned int nextId;

		void Inspect(const char* title = "Model Manager");
	};
}


class Scene3d : public Scene
{
public:
	Scene3d(const char* path);
	virtual ~Scene3d();
	virtual void Initialize() override;
	virtual void Update(const float& deltaTime) override;
	virtual void FixedUpdate(const float& timeStep) override;
	virtual void Draw() override;
	virtual void Debug() override;
	virtual void PollEvents() override;

public:
	std::vector<ECS::Node3d> Nodes;
	ECS::liteModelManager modelManager;
	Camera camera = { 0 };
	CameraMode mCameraMode = CAMERA_FIRST_PERSON;

	ECS::Node3d templateNode;
	unsigned int templateModel;
	bool lock_cursor = false;
};

