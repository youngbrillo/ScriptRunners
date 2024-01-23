#include "TilemapNode2d.h"
#include <LuaBridge/LuaBridge.h>
#include "Scene2d.h"
#include <imgui.h>

ECS::TilemapNode2d::TilemapNode2d(const char* name)
	: Node2d(name)
{

	b2World* world = Scene2d::Instance()->world;
	if (rigidbody.body == NULL)
	{
		rigidbody.bdyDef.type = b2_staticBody;
		rigidbody.bdyDef.position = b2Vec2(this->transform.position.x, this->transform.position.y);
		rigidbody.body = world->CreateBody(&rigidbody.bdyDef);
	}
	map.SetBody(rigidbody.body);
}

ECS::TilemapNode2d::~TilemapNode2d()
{
}

void ECS::TilemapNode2d::Import(const char* texture_alias, const char* dataFile_path)
{
	map.SetTexture(texture_alias);
	map.LoadConfig(dataFile_path);
}

void ECS::TilemapNode2d::LoadData(const char* dataFile_path)
{
	map.LoadConfig(dataFile_path);
}

void ECS::TilemapNode2d::saveData()
{
}

void ECS::TilemapNode2d::Update(const float& deltaTime)
{
}

void ECS::TilemapNode2d::FixedUpdate(const float& timestep)
{
}

void ECS::TilemapNode2d::Draw()
{
	if (!this->visible) return;
	map.Draw();

	if (mTileMode != tile_mode_draw)
	{

		Vector2 p;

		DrawRectangleV(Vector2{ (float)iMousePosition.x, (float)iMousePosition.y }, Vector2{ 1, 1 }, Color{ 0, 255, 0, 255 / 6 });

	}

}

void ECS::TilemapNode2d::UIDraw()
{
	switch (mTileMode)
	{
	case ECS::tile_mode_draw:
		break;
	case ECS::tile_mode_create:
		DrawText("Create", 25, 25, 20, RED);
		break;
	case ECS::tile_mode_edit:
		DrawText("Edit", 25, 25, 20, BLUE);
		break;
	case ECS::tile_mode_point:
		DrawText("Point", 25, 25, 20, BLUE);
		break;
	default:
		break;
	}
}

void ECS::TilemapNode2d::Poll()
{
	if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard) return;
	if (IsKeyReleased(KEY_TAB))
	{
		mTileMode = mTileMode == ECS::tile_mode_draw ?
			tile_mode_create: mTileMode == ECS::tile_mode_create ?
			tile_mode_edit	: mTileMode == ECS::tile_mode_edit ?
			tile_mode_point	:
			tile_mode_draw;
	}

	Vector2 mp = GetMousePosition();
	fMousePosition = GetScreenToWorld2D(mp, Scene2d::Instance()->camera.cam);

	int x = fMousePosition.x;
	int y = fMousePosition.y;

	if (fMousePosition.x < 0 && fMousePosition.x < x)
	{
		x -= 1;
	}
	if (fMousePosition.y < 0 && fMousePosition.y < y)
	{
		 y -= 1;
	}

	iMousePosition = glm::ivec2(x, y);

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && mTileMode == tile_mode_create)
	{
		//add tile
		tilemap::Tile n = newTile;
		n.position = iMousePosition;
		map.tiles.push_back(n);
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && mTileMode != tile_mode_draw)
	{
		auto it = map.tiles.begin();
		while (it != map.tiles.end())
		{
			if ((it)->position.x == iMousePosition.x && (it)->position.y == iMousePosition.y)
			{
				it = map.tiles.erase(it);
			}
			else
				it++;
		}
	}

	if (IsKeyReleased(KEY_SPACE) && mTileMode != tile_mode_draw)
	{
		map.Generate();
	}

	GeneratePoint();
}

void ECS::TilemapNode2d::inspect()
{
	Node2d::inspect();
	map.Inspect();
}


void ECS::TilemapNode2d::GeneratePoint()
{
	if (mTileMode != tile_mode_point)return;
	map.AddEdgeMouseClick(iMousePosition.x, iMousePosition.y, MOUSE_BUTTON_LEFT);

}



#include "Extentions2d.h"
#include "Scene2d.h"

static ECS::TilemapNode2d* CreateTilemapNode(const char* name)
{
	auto  node = std::make_shared<ECS::TilemapNode2d>(name);
	Scene2d::Instance()->Nodes.emplace_back(node);
	return node.get();
}
void ECS::TilemapNode2d::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Scene")
			.addFunction("CreateTilemapNode", CreateTilemapNode)
		.endNamespace()
		.beginNamespace("ECS")
			.deriveClass<ECS::TilemapNode2d, ECS::Node2d>("TilemapNode")
				.addFunction("Import", &ECS::TilemapNode2d::Import)
				.addFunction("LoadData", &ECS::TilemapNode2d::LoadData)
			.endClass()
		.endNamespace();
}

static int kc = ExtensionManager::Register(ECS::TilemapNode2d::Extend);