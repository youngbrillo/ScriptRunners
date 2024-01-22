#include "TilemapNode2d.h"
#include <LuaBridge/LuaBridge.h>

ECS::TilemapNode2d::TilemapNode2d(const char* name)
	: Node2d(name)
{
}

ECS::TilemapNode2d::~TilemapNode2d()
{
}

void ECS::TilemapNode2d::Import(const char* texture_alias, const char* dataFile_path)
{
}

void ECS::TilemapNode2d::LoadData(const char* dataFile_path)
{
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

	for (auto& tile : mTiles)
	{
		Rectangle destination = Rectangle
		{
			tile.position.x, tile.position.y,
			tile.Size.x, tile.Size.y
		};
		DrawTexturePro(
			material.texture,
			tile.frame,
			destination,
			Vector2{0.0f, 0.0f},
			transform.rotation,
			material.color
		);
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
	default:
		break;
	}
}

void ECS::TilemapNode2d::Poll()
{
	if (IsKeyReleased(KEY_TAB))
	{
		mTileMode = mTileMode == ECS::tile_mode_draw ?
			tile_mode_create : mTileMode == ECS::tile_mode_create ?
			tile_mode_edit : tile_mode_create;
	}


	if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && mTileMode == tile_mode_create)
	{
		//add tile
		//ECS::Tile n = newTile;
		//n.position.x = (int)
	}
}

void ECS::TilemapNode2d::inspect()
{
}

void ECS::TilemapNode2d::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.deriveClass<ECS::TilemapNode2d, ECS::Node2d>("TilemapNode")

		.endClass();
}
