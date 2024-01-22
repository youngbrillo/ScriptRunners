#pragma once
#include "Manager.h"
#include <map>
#include <vector>
#include <string>
#include <lua.hpp>
#include <memory>
namespace FONT
{
	struct FontWrapper
	{
		Font font;
		std::string path;
		std::string alias;
	};

}


class FontManager : public Manager
{
public:
	FontManager();
	~FontManager();
	virtual void init() override;
	virtual void clear() override;

	static unsigned int AddFont(const char* path, const char* alias) { return Instance()->addFont(path, alias); }
	static void RemoveFont(unsigned int id) { Instance()->removeFont(id); }
	unsigned int addFont(const char* path, const char* alias);
	void removeFont(unsigned int id);
	Font getFont(const char* alias) const;
	Font getFont(unsigned int id) const;
	static unsigned int GetFont(const char* alias);
protected:

	virtual void innerdebug() override;
public:
	static std::shared_ptr<FontManager> Instance();
	static Manager Register();
	static void Extend(lua_State* L);


	std::vector <std::pair<std::string, unsigned int>> GetAllFonts() const;
public:
	std::map<unsigned int, FONT::FontWrapper> fonts;
	std::map<std::string, unsigned int> alias_to_Font;
protected:
	lua_State* L;

};

