#pragma once

#include "Manager.h"
#include <string>
#include <map>
#include "RenderPipeline.h"
#include <imgui_string_wrap_extension.h>
#include <lua.hpp>
#include <vector>
#include <memory>
#include "AnimationComponents.h"
namespace textureDebug
{
	struct Sprite;

	struct Texture
	{
		std::string path = "path here";
		std::string alias = "png file";
		Texture2D t;
		bool persistant = true;

		void debug();
		void ActiveDebug(textureDebug::Sprite* sprite);

		//debug tools
		bool ActiveView = false;
		float _scale = 1.0f;
		ImVec2 _uv_min = ImVec2(0.0f, 0.0f);
		ImVec2 _uv_max = ImVec2(1.0f, 1.0f);

		Rectangle _frame = { 0, 0, 16, 16 };
		float _frame_zoom = 1.0f;

	};

	struct Sprite
	{
		unsigned int texture_id = 0;
		std::vector<FrameData> frames;
		std::map<std::string, std::vector<int>> animations;
		std::vector<FrameData> getAnimationFrames(std::string animation_name)
		{
			std::vector<FrameData> fd = std::vector<FrameData>();
			if (animations.find(animation_name) != animations.end())
			{
				std::vector<int>& af = animations[animation_name];
				for (int i : af)
				{
					fd.push_back(frames[i]);
				}
			}
			return fd;
		}
		Rectangle GetFrame(const char* alias)
		{
			Rectangle rec = Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
			for (auto& fd : frames)
			{
				if (strcmp(fd.name.c_str(), alias) == 0)
				{
					rec = fd.frame;
					break;
				}
			}

			return rec;
		}
		void Debug(Rectangle& rec, const char* title = "Sprite");
	};
}


class TextureManager : public Manager
{
public:
	TextureManager();
	~TextureManager();
	virtual void init() override;
	virtual void clear() override;
	// // management functions //////////////////////////////////////////////////
		//Add a new texture by path
	Texture2D AddTexturePath(const char* path, const char* alias);
	Texture2D AddTextureTex(Texture2D texture, const char* alias);
	void removeTexture(unsigned int textureID);
	Texture2D GetTexture(const char* alias);
	textureDebug::Sprite& getSprite(unsigned int texture_id);

protected:
	textureDebug::Sprite AddSprite(unsigned int texture_id, const char* path);
	bool canAddTexture(const char* alias);
	// // frame/debug functions /////////////////////////////////////////////////
	//additional helper functions

public:
	virtual void innerdebug() override;
	static Manager Register();
	static void Extend(lua_State* L);
private:
	std::map<std::string, textureDebug::Texture> textures;
	std::map<unsigned int, textureDebug::Sprite> sprites;

	void populate_spriteSheet(textureDebug::Sprite& sprite, const char* path);
	void populate_spriteSheet(textureDebug::Sprite& sprite, int rows, int columns);
public:
	static std::shared_ptr<TextureManager> Instance();
private:
	lua_State* L;
};
