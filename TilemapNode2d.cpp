#include "TilemapNode2d.h"
#include <LuaBridge/LuaBridge.h>
#include "Scene2d.h"


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
	if (!this->visible) return;

	for (auto& tile : mTiles)
	{
		Rectangle destination = Rectangle
		{
			(float)tile.position.x, (float)tile.position.y,
			(float)tile.size.x, (float)tile.size.y
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
			tile_mode_edit : tile_mode_draw;
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
		ECS::Tile n = newTile;
		n.position = iMousePosition;
		mTiles.push_back(n);
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && mTileMode != tile_mode_draw)
	{
		auto it = mTiles.begin();
		while (it != mTiles.end())
		{
			if ((it)->position.x == iMousePosition.x && (it)->position.y == iMousePosition.y)
			{
				it = mTiles.erase(it);
			}
			else
				it++;
		}
	}

	if (IsKeyReleased(KEY_SPACE) && mTileMode != tile_mode_draw)
	{
		GenerateFixtures();
	}
}

void ECS::TilemapNode2d::inspect()
{
	Node2d::inspect();
}

void ECS::TilemapNode2d::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.deriveClass<ECS::TilemapNode2d, ECS::Node2d>("TilemapNode")

		.endClass();
}

void ECS::TilemapNode2d::GenerateFixtures()
{
	if (mTiles.size() < 1) return;
	std::vector<b2Vec2> edges;

	for (auto& t : mTiles)
	{
		edges.push_back(b2Vec2(t.position.x, t.position.y));
		edges.push_back(b2Vec2(t.position.x + t.size.x, t.position.y));
		edges.push_back(b2Vec2(t.position.x + t.size.x, t.position.y + t.size.y));
		edges.push_back(b2Vec2(t.position.x, t.position.y+t.size.y));
	}
	b2World* world = Scene2d::Instance()->world;
	if (rigidbody.body != NULL)
	{
		world->DestroyBody(rigidbody.body);
		rigidbody.body = NULL;
		rigidbody.fixture = NULL;
	}

	rigidbody.bdyDef.type = b2_staticBody;
	rigidbody.bdyDef.position = b2Vec2(this->transform.position.x, this->transform.position.y);

	rigidbody.body = world->CreateBody(&rigidbody.bdyDef);


	for (int i = 0; i <= edges.size() - 2; i++)
	{
		b2EdgeShape shape;
		shape.SetTwoSided(edges[i], edges[i + 1]);

		b2FixtureDef fx;
		fx.shape = &shape;
		rigidbody.body->CreateFixture(&fx);
	}




}
