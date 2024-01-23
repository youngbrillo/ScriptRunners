#pragma once
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <vector>
#include "RenderPipeline.h"

namespace tilemap {
	struct Tile
	{
		glm::ivec2 position = glm::ivec2(0, 0);
		glm::ivec2 size		= glm::ivec2(1);
		glm::ivec4 frame	= glm::ivec4(0, 0, 32, 32);
		Color tint			= RAYWHITE;
		int layer = 0;
	};
}


class Tilemap
{
public:
	Tilemap();
	Tilemap(const char* config);
	Tilemap(const char* texturePath, const char* config);
	~Tilemap();

	void SetBody(b2Body* b) { mbody = b; ownsBody = false; }
	b2Body* GetBody() const { return mbody; }
	void clearBody();	
	static b2World* GetWorld();
	void SetTexture(const char* texturePath);
	void LoadConfig(const char* config_path);

	void Generate();
	void Draw() const;
	void AddEdgeMouseClick(int x, int y, int key = MOUSE_BUTTON_LEFT);

	void Inspect(const char* title = "Tilemap");

public:
	Texture2D texture = { 1, 1, 1 };
	std::vector<tilemap::Tile> tiles;
	std::vector<b2Vec2> edges;
	glm::vec2 offset = glm::vec2(0.0f);
private:
	bool ownsTexture = false;
	b2Body* mbody = NULL;
	bool ownsBody = true;
	std::vector<b2Vec2> legacy_edges;
	bool mFistClick = false;
};

