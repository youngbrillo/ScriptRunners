#pragma once
#include "Scene.h"
#include <vector>
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <string>
#include "imgui_string_wrap_extension.h"
#include <memory>
namespace test { class Node; }

namespace luaVar
{
	struct iVar 
	{
		iVar() {};
		virtual void Inspect(lua_State* L) = 0;
	};


	template <class T>
	struct Inspector : luaVar::iVar
	{
		const char* key;
		T val;
		Inspector() : iVar() { key = "no key"; }
		Inspector(const char* k, T v) : iVar()
		{
			key = k;
			val = v;
		}
		void Set(lua_State* L)
		{
			luabridge::LuaRef ref = luabridge::getGlobal(L, key);
			try {
				luabridge::setGlobal(L, val, key);
			}
			catch (luabridge::LuaException const& e) {
				printf("error in '%s'\t%s\n", key, e.what());
			}
		}
	};

	struct INumber: public luaVar::Inspector <float> {
		INumber() :Inspector() {};
		INumber(const char* k, float v) : Inspector(k, v) {}
		virtual void Inspect(lua_State* L) { if (ImGui::InputFloat(key, &val, 1.0f, 10.0f)) Set(L); }
	};
	struct IString : public luaVar::Inspector <std::string> {
		IString() :Inspector() {};
		IString(const char* k, std::string v) : Inspector(k, v) {}
		virtual void Inspect(lua_State* L) { if (ImGui::InputText(key, &val)) Set(L); }
	};
	struct Ibool : public luaVar::Inspector <bool> {
		Ibool() :Inspector() {};
		Ibool(const char* k, bool v) : Inspector(k, v) {}
		virtual void Inspect(lua_State* L) {if (ImGui::Checkbox(key, &val)) Set(L);}
	};
}
class LuaScene : public Scene
{
public:
	LuaScene(const char* script, const char* startFunction);
	virtual ~LuaScene();

	void loadScript(const char* script, const char* startFunction);
	void unloadScript();

	virtual void Draw();
	virtual void Update(const float& deltaTime);
	void updateObjects(const float& delta);
	virtual void FixedUpdate(const float& timeStep);
	virtual void PollEvents() override;
	virtual void Debug();

	static Scene* Create(const char* script, const char* startFunction = "onSceneStart")
	{
		return new LuaScene(script, startFunction);
	}
	
	static bool CallLuaFunction(lua_State* L, const char* funcName);
	static bool CallLuaFunctionFloat(lua_State* L, const char* funcName, float val);
	static int assignInspector(lua_State* L);
public:
	lua_State* L;
	std::vector<test::Node*> SceneNodes;
	std::vector<test::Node*> collidingNodes;
	bool canUpdate = true;
	bool canDraw = true;


	std::map<const char*, std::shared_ptr<luaVar::iVar>> m_inspectors;

	static LuaScene* Instance;
};
