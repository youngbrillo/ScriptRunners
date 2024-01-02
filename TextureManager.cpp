#include "TextureManager.h"
#include "GlobalManager.h"
#include <LuaBridge/LuaBridge.h>


TextureManager::TextureManager()
	: Manager(
		"Texture manager", 
		"Configs/TextureManager/json", 
		"Scripts/Manager/TextureManager.lua"
	)
	, textures({})
	, sprites({})
	, L(NULL)
{
}

TextureManager::~TextureManager()
{
	if(L != NULL) lua_close(L);
	L = NULL;
}

void TextureManager::init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	this->Extend(L);

	if ( LUA_OK == luaL_dofile(L, scriptPath.c_str()))
	{
		luabridge::LuaRef func = luabridge::getGlobal(L, "LoadTextures");
		try {
			func();
		}
		catch (luabridge::LuaException const& e) {
			printf("%s\n", e.what());
		}
	}
	else
	{
		printf("Error in %s\t%s\n", scriptPath.c_str(), lua_tostring(L, -1));
	}
}
static void callLuaFunction(lua_State* L, const char* functionName)
{
	luabridge::LuaRef func = luabridge::getGlobal(L, functionName);
	try {
		func();
	}
	catch (luabridge::LuaException const& e) {
		printf("%s\n", e.what());
	}
}
void TextureManager::clear()
{
	for (auto it = textures.begin(); it != textures.end();)
	{
		if (it->second.persistant == false)
			it = textures.erase(it);
		else
			it++;
	}
	callLuaFunction(L, "unLoadTextures");
	lua_close(L);
	L = NULL;
}

Texture2D TextureManager::AddTexturePath(const char* path, const char* alias)
{
	if (textures.find(alias) != textures.end())
		return textures[alias].t;
	std::string mpath = path; mpath += ".png";
	for (auto&& l : textures)
	{
		if (strcmp(mpath.c_str(), l.second.path.c_str()) == 0)
		{
			return l.second.t;
		}
	}

	textureDebug::Texture texture;
	texture.path = mpath;
	texture.alias = alias;
	texture.t = LoadTexture(texture.path.c_str());
	if (texture.t.id == 0)
	{
		texture.persistant = false;
	}
	textures.emplace(alias, texture);

	this->AddSprite(texture.t.id, path);
	return texture.t;
}

Texture2D TextureManager::AddTextureTex(Texture2D tex, const char* alias)
{
	if (textures.find(alias) != textures.end())
		return textures[alias].t;
	for (auto&& l : textures)
	{
		if (l.second.t.id == tex.id) return l.second.t;
	}

	textureDebug::Texture texture;
	texture.path = "no path";
	texture.alias = alias;
	texture.t = tex;
	textures.emplace(alias, texture);


	return texture.t;

}

void TextureManager::removeTexture(unsigned int textureID)
{
	std::string alias = "";
	for (auto& l : textures)
	{
		if (l.second.t.id == textureID) alias = l.first;
	}
	if (alias != "") {
		UnloadTexture(textures[alias].t);
		textures.erase(alias);
	}
}

Texture2D TextureManager::GetTexture(const char* alias)
{
	if (textures.find(alias) != textures.end()) return textures[alias].t;

	return Texture2D{ 1, 1, 1 };
}

textureDebug::Sprite TextureManager::AddSprite(unsigned int tid, const char* path)
{
	if (sprites.find(tid) != sprites.end())
	{
		return sprites[tid];
	}
	sprites[tid] = textureDebug::Sprite();
	textureDebug::Sprite& sprite = sprites[tid];
	if (path != NULL)
	{
		this->populate_spriteSheet(sprite, path);
	}
	return sprite;
}

bool TextureManager::canAddTexture(const char* alias)
{
	return (textures.find(alias) == textures.end());
}

textureDebug::Sprite& TextureManager::getSprite(unsigned int tid)
{
	if (sprites.find(tid) != sprites.end())
	{
		return sprites[tid];
	}

	textureDebug::Sprite k = textureDebug::Sprite();
	return k;
}

void TextureManager::innerdebug()
{
	if (ImGui::TreeNode("Textures"))
	{
		for (auto& t : textures)
		{
			t.second.debug();
		}

		ImGui::TreePop();
	}


	for (auto& t : textures)
	{
		t.second.ActiveDebug(
			sprites.find(t.second.t.id) != sprites.end() ?
			&sprites[t.second.t.id] : NULL
		);
	}
}

void textureDebug::Texture::debug()
{

	if (ImGui::TreeNode(this->alias.c_str()))
	{

		ImGui::TextColored({ 0, 1, 1, 1 }, path.c_str());

		ImGui::Text(
			"  ID: %3d \n| width: %05d \n| height: %05d \n| mipMaps: %d \n| data format type: %d",
			t.id, t.width, t.height, t.mipmaps, t.format
		);

		if (ImGui::Button("Toggle View")) { ActiveView = !ActiveView; }

		ImGui::SliderFloat2("UV min", &_uv_min.x, 0.0f, 1.0f);
		ImGui::SliderFloat2("UV max", &_uv_max.x, 0.0f, 1.0f);
		ImGui::SliderFloat("UV min width", &_uv_min.x, 0.0f, t.width);
		ImGui::SliderFloat("UV min height", &_uv_min.y, 0.0f, t.height);
		ImGui::SliderFloat("UV max width", &_uv_max.x, 0.0f, t.width);
		ImGui::SliderFloat("UV max height", &_uv_max.y, 0.0f, t.height);

		ImGui::TreePop();
	}
}
void textureDebug::Texture::ActiveDebug(textureDebug::Sprite* sprite)
{
	if (!ActiveView) return;
	if (ImGui::Begin("Sprite configuration", &ActiveView))//, ImGuiWindowFlags_MenuBar))
	{
		//Left Side
		{
			ImGui::BeginChild("Sprite Properties", ImVec2(300, 0), true);
			ImGui::TextColored(ImVec4(0, 1.0f, 0, 1.0f), "ID: %04d", this->t.id);

			ImGui::Text(
				"  ID: %3d \n| width: %05d \n| height: %05d \n| mipMaps: %d \n| data format type: %d",
				t.id, t.width, t.height, t.mipmaps, t.format
			);

			ImGui::SliderFloat("frame x position", &_frame.x, 0.0f, t.width - _frame.width);
			ImGui::SliderFloat("frame y position", &_frame.y, 0.0f, t.height - _frame.height);
			ImGui::SliderFloat2("frame size", &_frame.width, 0.0f, t.width > t.height ? t.width : t.height);
			ImGui::SliderFloat("frame zoom", &_frame_zoom, 0.0f, 30.0f);

			if (sprite != NULL)
			{
				sprite->Debug(this->_frame);
			}
			ImGui::EndChild();
		}
		ImGui::SameLine();

		// Right Side
		{
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("Alias: %s", alias.c_str());
			ImGui::Text("Path:  %s", path.c_str());
			ImGui::Separator();
			if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Atlas"))
				{
					//use this (_currentFrame >= 0 && _currentFrame < mFrames.size())  ? mFrames[_currentFrame].width * spriteRatio : 
					ImGui::SliderFloat("ratio", &_scale, 0.1f, 10.0f);
					ImTextureID my_tex_id = &this->t.id;
					float my_tex_w = t.width * _scale;
					float my_tex_h = t.height * _scale;

					ImGui::Text("Dimensions %d x %d", t.width, t.height);

					bool vf = true;//(_currentFrame >= 0 && _currentFrame < mFrames.size());
					ImVec2 pos = ImGui::GetCursorScreenPos();
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

					ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), _uv_min, _uv_max, tint_col, border_col);

					ImGui::EndTabItem();
				}


				if (ImGui::BeginTabItem("Frame"))
				{
					//use this (_currentFrame >= 0 && _currentFrame < mFrames.size())  ? mFrames[_currentFrame].width * spriteRatio : 
					ImGui::SliderFloat("ratio", &_scale, 0.1f, 10.0f);
					ImTextureID my_tex_id = &this->t.id;

					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
					ImGui::Text("Frame: %00d - %s", 999, "Debug Frame");
					ImVec2 pos = ImGui::GetCursorScreenPos();

					//float region_sz = 32.0f;
					float region_x = _frame.x;//io.MousePos.x - pos.x - region_sz * 0.5f;
					float region_y = _frame.y;//io.MousePos.y - pos.y - region_sz * 0.5f;
					float zoom = _frame_zoom;
					if (region_x < 0.0f) { region_x = 0.0f; }
					else if (region_x > t.width - _frame.width) { region_x = t.width - _frame.width; }
					if (region_y < 0.0f) { region_y = 0.0f; }
					else if (region_y > t.height - _frame.height) { region_y = t.height - _frame.height; }
					ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
					ImGui::Text("Max: (%.2f, %.2f)", region_x + _frame.width, region_y + _frame.height);
					ImVec2 uv0 = ImVec2((region_x) / (float)t.width, (region_y) / (float)t.height);
					ImVec2 uv1 = ImVec2((region_x + _frame.width) / (float)t.width, (region_y + _frame.height) / (float)t.height);
					ImGui::Image(my_tex_id, ImVec2(_frame.width * zoom, _frame.height * zoom), uv0, uv1, tint_col, border_col);

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}



			ImGui::EndChild();
			if (ImGui::Button("Revert")) {}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {}
			ImGui::EndGroup();
		}
	}
	ImGui::End();

}
Manager TextureManager::Register()
{
	GlobalManager::Register_Manager<TextureManager>();
	return TextureManager();
}

void TextureManager::Extend(lua_State* L)
{
	//add texture and alias
	std::function<void(const char* a, const char* b)> add1 = [](const char* c, const char* d) {
		TextureManager::Instance()->AddTexturePath(c, d);
	};
	//get texture id with alias
	std::function<int(const char* a)> get = [](const char* aforalias) {
		auto t = TextureManager::Instance()->GetTexture(aforalias);
		return t.id;
	};

	//remove texture by id
	std::function<void(unsigned int)> removeTexture = [](unsigned int id) {
		TextureManager::Instance()->removeTexture(id);
	};

	//generate a checked texture
	std::function<int(const char*, int, int, int, int, unsigned int, unsigned int)>
		generate_Checkered = [](
			const char* alias, int width, int height, int rows, int columns,
			unsigned int colorA, unsigned int colorB
			)
	{
		if (!GlobalManager::Get_Manager<TextureManager>()->canAddTexture(alias))
		{
			auto t = TextureManager::Instance()->GetTexture(alias);
			return t.id;
		}

		Image i = GenImageChecked(width, height, rows, columns, GetColor(colorA), GetColor(colorB));
		Texture2D t = LoadTextureFromImage(i);

		Texture2D t1 = TextureManager::Instance()->AddTextureTex(t, alias);

		UnloadImage(i);

		return t1.id;
	};

	luabridge::getGlobalNamespace(L)
		.beginNamespace("TextureManager")
		.addFunction("Add", add1)
		.addFunction("Get", get)
		.addFunction("Remove", removeTexture)
		.addFunction("generateCheckered", generate_Checkered)
		.endNamespace();
}
#include "JsonHandler.h"
void TextureManager::populate_spriteSheet(textureDebug::Sprite& sprite, const char* path)
{
	std::string mpath = path; mpath += ".json";
	jsonObjects data;
	JSONParse::ParseJSONFile(data, mpath.c_str());
	//get frames
	for (auto& frameObj : data.find("frames").children)
	{
		auto& frame = frameObj.find("frame");
		Rectangle newFrame;
		frame
			.GetInteger("x", newFrame.x)
			.GetInteger("y", newFrame.y)
			.GetInteger("w", newFrame.width)
			.GetInteger("h", newFrame.height);

		textureDebug::FrameData newFrameData;
		newFrameData.frame = newFrame;

		frameObj.GetInteger("duration", newFrameData.duration);

		newFrameData.duration = newFrameData.duration / 1000.0f;
		newFrameData.name = frameObj.name;
		sprite.frames.push_back(newFrameData);

	}
	//get animations
	for (auto& frameObj : data.find("meta").find("frameTags").children)
	{
		int frameStart = 0, frameEnd = 0;
		std::vector<int> newAnimation;
		std::string name;
		frameObj
			.GetInteger("from", frameStart)
			.GetInteger("to", frameEnd)
			.GetString("name", name);
		for (int i = frameStart; i <= frameEnd; i++)
		{
			newAnimation.push_back(i);
		}

		sprite.animations[name] = newAnimation;

	}


}

void TextureManager::populate_spriteSheet(textureDebug::Sprite& sprite, int rows, int columns)
{
	assert(false && "Not implemented yet");
}

std::shared_ptr<TextureManager> TextureManager::Instance()
{
	return GlobalManager::Get_Manager<TextureManager>();
}

void textureDebug::Sprite::Debug(Rectangle& rec, const char* title)
{
	if (ImGui::TreeNode(title))
	{
		if (ImGui::TreeNode("Frames"))
		{
			for (auto&& frame : frames)
			{
				if (ImGui::TreeNode(frame.name.c_str()))
				{
					ImGui::InputFloat("x", &frame.frame.x);
					ImGui::InputFloat("y", &frame.frame.y);
					ImGui::InputFloat("w", &frame.frame.width);
					ImGui::InputFloat("h", &frame.frame.height);
					ImGui::SliderFloat("time", &frame.duration, 0, 0.1f);
					if (ImGui::Button("Set"))
					{
						rec = frame.frame;
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		for (auto& a : animations)
		{
			if (ImGui::TreeNode(a.first.c_str()))
			{

				ImGui::TreePop();
			}
		}


		ImGui::TreePop();
	}
}



static int gManagerInstance = RegisterManager("Texture Manager", "", "", TextureManager::Register);
//static int gExtentionEntry = RegisterComponentExtention("TextureManager", TextureManager::Extend);
