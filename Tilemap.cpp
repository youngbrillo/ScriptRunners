#include "Tilemap.h"
#include "JsonHandler.h"
#include "Scene2d.h"
#include <imgui.h>

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
		v.x += this->offset.x;v.y += this->offset.y;
		edges.push_back(v);
		vert.find("p2").GetNumber("x", v.x).GetNumber("y", v.y);
		v.x += this->offset.x; v.y += this->offset.y;
		edges.push_back(v);
	}
	bool autoGen = false;
	loader.Getboolean("autogenerate", autoGen);

	if(autoGen)
		Generate();

	valid_path = true;
	m_config_path = config_path;

	this->AddTileData(&loader);
}

void Tilemap::SaveConfig(const char* config_path)
{
	jsonObjects saver;
	saver.SetBooleanAt("autogenerate", true);
	jsonObjects& verts =  saver.findOrCreate("verts");

	if (legacy_edges.size() > 1)
	{
		for (int i = 0; i < legacy_edges.size()-1; i+=2)
		{
			if (i >= edges.size() - 1) continue;
			auto& entry = verts.AddArrayEntry();
			entry.findOrCreate("p1").SetNumberAt("x", legacy_edges[i].x).SetNumberAt("y", legacy_edges[i].y);
			entry.findOrCreate("p2").SetNumberAt("x", legacy_edges[i+1].x).SetNumberAt("y", legacy_edges[i+1].y);

		}
	}

	saver.SaveToFile(config_path);
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


void Tilemap::AddTileData(jsonObjects* data)
{
	jsonObjects& chunkArray = data->find("layers").find("chunks");
	int tile_width = 1, tile_height = 1;
	int origin_x = 0, origin_y = 0;
	data->GetInteger("tilewidth", tile_width).GetInteger("tileheight", tile_height)
		.find("layers")
		.GetInteger("startx", origin_x).GetInteger("starty", origin_y);

	for (int y = 0; y < texture.height / tile_height; y++)
	{
		for (int x = 0; x < texture.width / tile_width; x++)
		{
			Frames.push_back(Rectangle{(float)x*tile_width, (float)y*tile_height, (float)tile_width, (float)tile_height});
		}
	}


	for (auto& child : chunkArray.children)
	{
		jsonObjects& dataArray = child.find("data");
		int width = 0;
		int height = 0;
		int x_offset = 0;
		int y_offset = 0;

		child.GetInteger("height", height).GetInteger("width", width)
			 .GetInteger("x", x_offset).GetInteger("y", y_offset);
#if false
		for (int i = 0; i < dataArray.integers.size(); i++)// : dataArray.integers)
		{
			int index = dataArray.integers[i];
			tilemap::Tile r;
			r.frame = glm::ivec4(tile_width* index / tile_width, tile_height* index % tile_height, tile_width, tile_height);

			if (index >= 0 && index < Frames.size())
			{
				r.frame.x = Frames[index].x;
				r.frame.y = Frames[index].y;
			}
			else
			{
				printf(">\tchild array index out of bounds :/ %d\n", index);
			}
			r.position = glm::ivec2(
				origin_x + x_offset + i/width,
				origin_y + y_offset + i%height
			);
			this->tiles.push_back(r);
		}
#else
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int index = dataArray.integers[width * y + x];
				if (index <= 0) continue;
				index--;
				tilemap::Tile r;
				//r.frame = glm::ivec4(tile_width * index / tile_width, tile_height * y % tile_height, tile_width, tile_height);
				r.frame.z = tile_width;
				r.frame.w = tile_height;
				if (index >= 0 && index < Frames.size())
				{
					r.frame.x = Frames[index].x;
					r.frame.y = Frames[index].y;
					r.frame_id = index;
				}
				else
				{
					printf(">\tchild array index out of bounds :/ %d\n", index);
				}
				r.position = glm::ivec2(x_offset + x, y_offset + y);

				this->tiles.push_back(r);

			}
		}
		
#endif
	}

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
			Frames[tile.frame_id],
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
	if (ImGui::TreeNode(title))
	{

		if (ImGui::TreeNode(TextFormat("Tiles: %d", tiles.size())))
		{
			for(int i = 0; i < tiles.size();i++)
			{ 

				if (ImGui::TreeNode(TextFormat("tile-%04d", i)))
				{
					auto& t = tiles[i];
					ImGui::SliderInt("frame index", &t.frame_id, 0, this->Frames.size());
					ImGui::InputInt2("position", &t.position.x);
					ImGui::InputInt4("frame", &t.frame.x);

					Vector4 c = ColorNormalize(t.tint);
					if (ImGui::ColorEdit4("tint", &c.x))
					{
						t.tint = ColorFromNormalized(c);
					}

					ImGui::TreePop();
				}

			}

			ImGui::TreePop();
		}

		if (!valid_path) ImGui::BeginDisabled();
		if (ImGui::Button("Save"))
		{
			this->SaveConfig(m_config_path.c_str());
		}
		if (!valid_path) ImGui::EndDisabled();


		if (ImGui::TreeNode("frames"))
		{
			for (int i = 0; i < Frames.size(); i++)
			{
				if (ImGui::TreeNode(TextFormat("Frame-%03d", i)))
				{
					ImGui::SliderFloat("x", &Frames[i].x, 0, texture.width);
					ImGui::SliderFloat("y", &Frames[i].y, 0, texture.height);
					ImGui::SliderFloat("w", &Frames[i].width, 0, texture.width);
					ImGui::SliderFloat("h", &Frames[i].height, 0, texture.height);

					if (ImGui::Button("set as current frame"))
					{

						currentFrame = i;
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		//draw image
		ImTextureID my_tex_id = &texture.id;
		float zoom = 3.0f;

		ImVec2 uv0 = ImVec2( 
			Frames[currentFrame].x / (float)texture.width, 
			Frames[currentFrame].y / (float)texture.height
		);
		ImVec2 uv1 = ImVec2(
			Frames[currentFrame].width / texture.width,
			Frames[currentFrame].height / texture.height
		);

		ImGui::Image(my_tex_id, ImVec2(texture.width * zoom, texture.height * zoom));
		ImGui::SameLine();
		ImGui::Image(my_tex_id, ImVec2(texture.width * zoom, texture.height * zoom), uv0, uv1);

		ImGui::TreePop();
	}
}
