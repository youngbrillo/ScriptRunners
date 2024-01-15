#include "Scene3d.h"
#include <imgui_string_wrap_extension.h>

void ECS::Node3d::Inspect()
{
	ImGui::Checkbox("visible", &visible);
	ImGui::Checkbox("wiremode", &wiremode);
	//position
	ImGui::InputFloat3("position", &position.x);
	ImGui::InputFloat("position.x", &position.x, 1.0f, 10.0f);
	ImGui::InputFloat("position.y", &position.y, 1.0f, 10.0f);
	ImGui::InputFloat("position.z", &position.z, 1.0f, 10.0f);
	//rotation
	ImGui::SliderFloat("rotation Angle", &rotationAngle, -360, 360);
	ImGui::InputFloat3("rotationAxis", &rotationAxis.x);
	ImGui::InputFloat("rotationAxis.x", &rotationAxis.x, 1.0f, 1.0f);
	ImGui::InputFloat("rotationAxis.y", &rotationAxis.y, 1.0f, 1.0f);
	ImGui::InputFloat("rotationAxis.z", &rotationAxis.z, 1.0f, 1.0f);
	//scale
	ImGui::InputFloat3("size", &size.x);
	ImGui::InputFloat("size.x", &size.x, 0.1f, 10.0f);
	ImGui::InputFloat("size.y", &size.y, 0.1f, 10.0f);
	ImGui::InputFloat("size.z", &size.z, 0.1f, 10.0f);



	ImGui::InputFloat("scale", &scale, 0.1f, 10.0f);

	Vector4 c = ColorNormalize(color);
	if (ImGui::ColorEdit4("color", &c.x)) color = ColorFromNormalized(c);
	
}
ECS::liteModelManager::liteModelManager()
	: nextId(0)
{
	{
		Image i = GenImageChecked(32, 32, 8, 8, WHITE, BLACK);
		Texture t = LoadTextureFromImage(i);
		Models[nextId] = LoadModelFromMesh(GenMeshTorus(0.4f, 1.0f, 16, 32));
		ModelNames[nextId] = "Torus";
		DiffuseTextures[nextId] = t;
		Models[nextId].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = DiffuseTextures[nextId];
		nextId++;
		UnloadImage(i);
	}

	{
		Image i = GenImageChecked(16, 16, 8, 8, LIGHTGRAY, DARKBLUE);
		Texture t = LoadTextureFromImage(i);
		Models[nextId] = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
		ModelNames[nextId] = "Cube";
		DiffuseTextures[nextId] = t;
		Models[nextId].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = DiffuseTextures[nextId];
		nextId++;
		UnloadImage(i);
	}


	{
		Image i = GenImageChecked(32, 32, 8, 8, WHITE, BLACK);
		Texture t = LoadTextureFromImage(i);
		Models[nextId] = LoadModelFromMesh(GenMeshSphere(0.5f, 32, 32));
		ModelNames[nextId] = "Sphere";
		DiffuseTextures[nextId] = t;
		Models[nextId].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = DiffuseTextures[nextId];
		nextId++;
		UnloadImage(i);
	}

	
		this->AddModel("church", "assets/models/church.obj", "assets/models/church_diffuse.png");
		this->AddModel("barracks",  "assets/models/barracks.obj", "assets/models/barracks_diffuse.png" );
		this->AddModel("watermill",  "assets/models/watermill.obj", "assets/models/watermill_diffuse.png" );
		this->AddModel("bridge",  "assets/models/obj/bridge.obj", "assets/models/obj/bridge_diffuse.png" );
		this->AddModel("castle",  "assets/models/obj/castle.obj",  "assets/models/obj/castle_diffuse.png" );
		this->AddModel("house",  "assets/models/obj/house.obj",   "assets/models/obj/house_diffuse.png" );
		this->AddModel("market",  "assets/models/obj/market.obj",  "assets/models/obj/market_diffuse.png" );
		this->AddModel("plane",  "assets/models/obj/plane.obj",   "assets/models/obj/plane_diffuse.png" );
		this->AddModel("turret",  "assets/models/obj/turret.obj",  "assets/models/obj/turret_diffuse.png" );
		this->AddModel("well", "assets/models/obj/well.obj", "assets/models/obj/well_diffuse.png");

}
ECS::liteModelManager::~liteModelManager()
{
	for (auto&& p : Models)
	{
		unsigned int id = p.first;
		if(DiffuseTextures[id].id > 0)
			UnloadTexture(DiffuseTextures[id]);     // Unload texture
			UnloadModel(Models[id]);         // Unload model
	}
}

unsigned int ECS::liteModelManager::AddModel(const char* name, const char* model_path, const char* texture_path)
{
	unsigned int id = nextId++;
	Models[id] = LoadModel(model_path);

	DiffuseTextures[id] = LoadTexture(texture_path);
	Models[id].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = DiffuseTextures[id];
	ModelNames[id] = name;

	return id;
}

void ECS::liteModelManager::Inspect(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		for (auto&& p : Models)
		{
			unsigned int id = p.first;
			if (ImGui::TreeNode(ModelNames[id].c_str()))
			{
				ImGui::Text("Id: %d", id);

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}

Scene3d::Scene3d(const char* path)
{

	// Define the camera to look into our 3d world
	camera.position = Vector3{ 2.0f, 2.0f, 6.0f };    // Camera position
	camera.target = Vector3{ 0.0f, 0.5f, 0.0f };      // Camera looking at point
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

}

Scene3d::~Scene3d()
{
}

void Scene3d::Initialize()
{
}

void Scene3d::Update(const float& deltaTime)
{
	UpdateCamera(&camera, mCameraMode);

}

void Scene3d::FixedUpdate(const float& timeStep)
{
}
static const char* camModeNames[5] = { "CUSTOM", "FREE", "ORBITAL", "FIRST PERSON", "THRID PERSON" };

void Scene3d::Draw()
{

	BeginMode3D(camera);
		DrawGrid(20, 10.0f);         // Draw a grid

		for (auto&& n : Nodes)
		{
			if (!n.visible) continue;
			if (n.wiremode)
			{
				DrawModelWiresEx(modelManager.Models.at(n.modelId), n.position, n.rotationAxis, n.rotationAngle, Vector3Scale(n.size, n.scale), n.color);
			}
			else {
				DrawModelEx(modelManager.Models.at(n.modelId), n.position, n.rotationAxis, n.rotationAngle, Vector3Scale(n.size, n.scale), n.color);
			}

		}
	EndMode3D();

	DrawText(camModeNames[mCameraMode], 25, 25, 20, RAYWHITE);
}

void Scene3d::Debug()
{
	if (ImGui::TreeNode("camera"))
	{
		ImGui::InputFloat3("Position", &camera.position.x);
		ImGui::InputFloat3("target", &camera.target.x);
		ImGui::InputFloat3("up", &camera.up.x);
		ImGui::InputFloat3("fovy", &camera.fovy);

		ImGui::TreePop();
	}
	modelManager.Inspect();

	if (ImGui::TreeNode("Nodes"))
		{
		for (auto&& n : Nodes)
		{
			if (ImGui::TreeNode(n.name.c_str()))
			{
				n.Inspect();
				ImGui::TreePop();
			}

		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Template Node"))
	{
		ImGui::InputText("Name", &templateNode.name);
		templateNode.Inspect();

		if (ImGui::BeginCombo("Model", modelManager.ModelNames[templateModel].c_str()))
		{
			for (auto&& p : modelManager.Models)
			{
				unsigned int id = p.first;
				const char* pv = modelManager.ModelNames[id].c_str();
				const bool is_selected = templateModel == id;
				if (ImGui::Selectable(pv, is_selected))
				{
					templateModel = id;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}




		bool a = ImGui::Button("Add");
		if (a)
		{
			templateNode.modelId = templateModel;
			if (templateNode.name == "")
			{
				templateNode.name = modelManager.ModelNames[templateModel];
			}
			Nodes.push_back(templateNode);
			templateNode.name = "";
		}

		ImGui::TreePop();
	}

}

void Scene3d::PollEvents()
{
	if (IsKeyPressed(KEY_TAB))
	{
		int mode = mCameraMode;
		mode++;
		if (mode >= CAMERA_THIRD_PERSON)
		{
			mCameraMode = CAMERA_CUSTOM;
		}
		else
		{
			mCameraMode = (CameraMode)mode;
		}
	}

	if (IsKeyPressed(KEY_CAPS_LOCK))
	{
		lock_cursor = !lock_cursor;
		if (lock_cursor) DisableCursor();
		else EnableCursor();
	}
}
