#include "Tilemap.h"
#include "JsonHandler.h"
#include "Scene2d.h"
Tilemap::Tilemap()

{
}

Tilemap::Tilemap(const char* config)
{
}

Tilemap::Tilemap(const char* texturePath, const char* config)
{
	SetTexture(texturePath);
	LoadConfig(config);
}

Tilemap::~Tilemap()
{
	if (ownsTexture)
	{
		UnloadTexture(this->texture);
	}

	clearBody();
}

void Tilemap::clearBody()
{
	if (!ownsBody)
	{
		mbody = NULL;
	}
	else 
	{
		if (mbody != NULL)
		{
			GetWorld()->DestroyBody(mbody);
			mbody = NULL;
		}

	}
	ownsBody = true;
}

b2World* Tilemap::GetWorld()
{
	return Scene2d::Instance()->world;
}

void Tilemap::SetTexture(const char* texturePath)
{
	texture = LoadTexture(texturePath);
}

void Tilemap::LoadConfig(const char* config_path)
{
	jsonObjects loader;
	if (!JSONParse::ParseJSONFile(loader, config_path))
	{
		//something went wrong...
		return;
	}
	jsonObjects verticies = loader.find("verts");
	for (auto& vert : verticies.children)
	{
		b2Vec2 v;
		vert.find("p1").GetNumber("x", v.x).GetNumber("y", v.y);
		edges.push_back(v);
		vert.find("p2").GetNumber("x", v.x).GetNumber("y", v.y);
		edges.push_back(v);
	}
	bool autoGen = false;
	loader.Getboolean("autogenerate", autoGen);

	if(autoGen)
		Generate();
}

void Tilemap::Generate()
{
	if (edges.size() == 0) return;
	if (mbody == NULL)
	{
		b2BodyDef bd;
		bd.type = b2_staticBody;
		bd.position = b2Vec2_zero;
		mbody = Tilemap::GetWorld()->CreateBody(&bd);
		ownsBody = true;
	}

	for (int i = 0; i < edges.size()-1; i +=2)
	{
		if (i >= edges.size() - 1) continue;


		b2EdgeShape shape;
		shape.SetTwoSided(edges[i], edges[i + 1]);

		b2FixtureDef fx;
		fx.shape = &shape;
		mbody->CreateFixture(&fx);
	}

	legacy_edges.insert(legacy_edges.end(), edges.begin(), edges.end());
	edges.clear();
}

void Tilemap::Draw() const
{
	for (auto& tile : tiles)
	{
		Rectangle destination = Rectangle
		{
			offset.x + tile.position.x, offset.y + tile.position.y,
			(float)tile.size.x, (float)tile.size.y
		};

		Rectangle source = Rectangle{ (float)tile.frame.x, (float)tile.frame.y, (float)tile.frame.z, (float)tile.frame.w};
		DrawTexturePro(
			texture,
			source,
			destination,
			Vector2{ 0.0f, 0.0f },
			0.0,
			tile.tint
		);
	}

	for (auto& v : edges)
	{
		DrawRectangleV(Vector2{ v.x - 0.25f, v.y - 0.25f }, Vector2(0.5f, 0.5f), YELLOW);
	}
}

void Tilemap::AddEdgeMouseClick(int x, int y, int key)
{

	if (IsMouseButtonPressed(key))
	{
		edges.emplace_back(x, y);
		mFistClick = true;
	}

	if (IsMouseButtonReleased(key) && mFistClick)
	{
		edges.emplace_back(b2Vec2(x, y));
		mFistClick = false;
	}
}

void Tilemap::Inspect(const char* title)
{
}
