#include "FontManager.h"
#include "GlobalManager.h"
#include <LuaBridge/LuaBridge.h>
#include "Components2d.h"
FontManager::FontManager()
	: Manager("Font Manager", "Configs/FontManager.json", 
		"Scripts/Manager/FontManager.lua")
	, fonts({})
	, alias_to_Font({})
	, L(NULL)
{

	//add default raylib font
	FONT::FontWrapper n = { GetFontDefault(), "no path", "default" };
	fonts[0] = n;
	alias_to_Font["default"] = 0;
}

FontManager::~FontManager()
{
	if (L != NULL) lua_close(L);
	L = NULL;
}

void FontManager::init()
{

	L = luaL_newstate();
	luaL_openlibs(L);
	this->Extend(L);

	if (LUA_OK == luaL_dofile(L, scriptPath.c_str()))
	{
		ECS::CallLuaFunction(L, "LoadFonts");
	}
	else
	{
		printf("Error in %s\t%s\n", scriptPath.c_str(), lua_tostring(L, -1));
	}

}

void FontManager::clear()
{
	ECS::CallLuaFunction(L, "unLoadFonts");
	lua_close(L);
	L = NULL;
}

unsigned int FontManager::addFont(const char* path, const char* alias)
{

	if (alias_to_Font.find(alias) != alias_to_Font.end()) return alias_to_Font[alias];

	Font nFont = LoadFont(path);
	FONT::FontWrapper wrapper = { nFont, path, alias };

	fonts[nFont.texture.id] = wrapper;
	alias_to_Font[alias] = nFont.texture.id;
	return 0;
}

void FontManager::removeFont(unsigned int id)
{
	if (id == 0) return;
	if (fonts.find(id) == fonts.end()) return;
	
	Font k = fonts[id].font;
	UnloadFont(k);

	alias_to_Font.erase(fonts[id].alias);
	fonts.erase(id);
}

Font FontManager::getFont(const char* alias) const
{
	if (alias_to_Font.find(alias) == alias_to_Font.end())
		return fonts.at(0).font;

	return fonts.at(alias_to_Font.at(alias)).font;
}

Font FontManager::getFont(unsigned int id) const
{
	if (fonts.find(id) == fonts.end())
		return fonts.at(0).font;

	return fonts.at(id).font;
}

unsigned int FontManager::GetFont(const char* alias)
{
	if (Instance()->alias_to_Font.find(alias) == Instance()->alias_to_Font.end())
		return 0;
	return Instance()->alias_to_Font.at(alias);
}

void FontManager::innerdebug()
{
	for (auto&& pair : fonts)
	{
		if (ImGui::TreeNode(pair.second.alias.c_str()))
		{
			ImGui::Text("ID: %d", pair.first);
			ImGui::Text("Path: %s", pair.second.path.c_str());
			//ImGui::Text("Path: %s", pair.second.alias);

			ImGui::TreePop();
		}
	}
}

std::shared_ptr<FontManager> FontManager::Instance()
{
	return GlobalManager::Get_Manager<FontManager>();
}

Manager FontManager::Register()
{
	GlobalManager::Register_Manager<FontManager>();
	return FontManager();
}

void FontManager::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("FontManager")
			.addFunction("AddFont", FontManager::AddFont)
			.addFunction("RemoveFont", FontManager::RemoveFont)
			.addFunction("GetFont", FontManager::GetFont)
		.endNamespace();
}
std::vector<std::pair<std::string, unsigned int>> FontManager::GetAllFonts() const
{
	std::vector<std::pair<std::string, unsigned int>> k;

	for (auto&& pair : alias_to_Font)
	{
		k.push_back(pair);
	}

	return k;
}
static int gManagerInstance = RegisterManager("Font Manager", "", "", FontManager::Register);
#include "Extentions2d.h"
static int ck = ExtensionManager::Register(FontManager::Extend);
