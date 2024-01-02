
#include "Scene.h"
#include <lua.hpp>
#include "RenderPipeline.h"
#include <vector>
#include <string>
#include <map>
#include <imgui.h>
#include <LuaBridge/LuaBridge.h>
#include "GlobalManager.h"
#include "TextureManager.h"
template<typename T>
struct VectorWrapper
{
	std::vector<T> vec;

	void AddEntry(T obj)
	{
		vec.emplace_back(obj);
	}

	void RemoveEntry(T obj)
	{
		auto it = vec.begin();
		while (it != vec.end())
		{
			if ((*it) == obj) it = vec.erase(it);
			else it++;
		}
	}
};

namespace LSE
{
	struct Transform
	{
		Vector2 Position = Vector2{ 0.0f, 0.0f };
		Vector2 Size = Vector2{1.0f, 1.0f };
		Vector2 Origin = Vector2{ 0.0f, 0.0f };
		float angle = 0.0f;
		
		Rectangle Destination() const
		{
			Rectangle r;
			r.x = Position.x;
			r.y = Position.y;
			r.width = Size.x;
			r.height = Size.y;
			return r;
		}

		void Debug(const char* title = "Transform")
		{
			if (ImGui::TreeNode(title))
			{
				ImGui::SliderFloat("x", &Position.x, 0, GetScreenWidth() - Size.x);
				ImGui::SliderFloat("y", &Position.y, 0, GetScreenHeight()- Size.y);
				ImGui::SliderFloat("w", &Size.x, 1, GetScreenWidth()-Position.x);
				ImGui::SliderFloat("h", &Size.y, 1, GetScreenHeight()- Position.y);
				ImGui::SliderFloat("Origin.x", &Origin.x, -Size.x, Size.x);
				ImGui::SliderFloat("Origin.y", &Origin.y, -Size.y, Size.y);
				ImGui::SliderFloat("angle", &angle, -360, 360);

				
				ImGui::TreePop();
			}

		}
	};

	struct Material
	{
		Texture2D texture = Texture2D{ 1, 1, 1 };
		Color color = RAYWHITE;
		Rectangle source = { 0, 0, 1, 1 };
		Vector2 uv_scroll = { 0, 0 };
		void SetTextureByAlias(const char* alias) {
			this->texture = TextureManager::Instance()->GetTexture(alias);
			this->source = { 0, 0, (float)texture.width, (float)texture.height };
		}
		void SetTexture(Texture2D t)
		{
			texture = t;
		}
		void SetColor(unsigned int hex)
		{
			color = GetColor(hex);
		}
		void SetColorVec(float r, float b, float g, float a)
		{
			Vector4 v = { r, b, g, a };
			color = ColorFromNormalized(v);

		}
		void Debug(const char* title = "Material")
		{
			if (ImGui::TreeNode(title))
			{

				Vector4 a = ColorNormalize(color);
				if (ImGui::ColorEdit4("Window Clear Color", &a.x))
				{
					color = ColorFromNormalized(a);
				}
				int tid = texture.id;
				if (ImGui::InputInt("textureID", &tid)) texture.id = tid;
				ImGui::SliderFloat2("Uv scroll", &uv_scroll.x, -20, 20.0f);

				ImGui::SliderFloat("x", &source.x, -texture.width, texture.width);
				ImGui::SliderFloat("y", &source.y, -texture.height, texture.height);
				ImGui::SliderFloat("width", &source.width, -texture.width, texture.width);
				ImGui::SliderFloat("height", &source.height, -texture.height, texture.height);
				ImGui::TreePop();
			}

		}
	};

	class GameObject
	{
	public:
		static VectorWrapper<GameObject*>* owner;

		GameObject(std::string n)
			: name(n)
		{
			if (!!owner) owner->AddEntry(this);
		}

		GameObject(std::string n, float x, float y, float w, float h )
			: name(n)
			, transform()
		{
			transform.Position = Vector2{ x, y };
			transform.Size = Vector2{ w, h };
			transform.Origin = Vector2Scale(transform.Size, 0.5f);
			if (!!owner) owner->AddEntry(this);
		}
		GameObject(std::string n, Transform t)
			: name(n)
			, transform(t)
		{
			if (!!owner) owner->AddEntry(this);
		}
		~GameObject()
		{
			if (!!owner) owner->RemoveEntry(this);
		}

		std::string name = "Game Object";
		bool visible = true;
		bool enabled = true;
		bool destroy = false;
		Transform transform;
		Material mat;

		Vector2 velocity = Vector2{ 0.0f, 0.0f };
		float AngularVelocity = 0.0f;;

		void Update(const float& dt) 
		{
			if (!this->enabled) return;
			transform.Position = Vector2Add(transform.Position, Vector2Scale(velocity, dt));

			transform.angle += AngularVelocity * dt;

			if (transform.Position.x < 0.0f || transform.Position.x > GetScreenWidth())
			{
				velocity.x *= -1;
			}
			if (transform.Position.y < 0.0f || transform.Position.y > GetScreenHeight())
			{
				velocity.y *= -1;
			}

			if (transform.angle > 360.0f) transform.angle = -360.0f;
			if (transform.angle < -360.0f) transform.angle = 360.0f;

			if (!this->visible) return;
			mat.source.x += mat.uv_scroll.x * dt;
			mat.source.y += mat.uv_scroll.y * dt;
		};
		void Draw()
		{
			if (!this->visible) return;
			DrawTexturePro(
				mat.texture,
				mat.source,
				transform.Destination(),
				transform.Origin,
				transform.angle,
				mat.color
			);

			
		}
		void Debug()
		{
			if (ImGui::TreeNode(name.c_str()))
			{
				ImGui::Checkbox("visible", &visible);
				ImGui::Checkbox("enabled", &enabled);
				transform.Debug();
				mat.Debug();
				ImGui::SliderFloat2("Velocity", &velocity.x, -50.0f, 50.0f);
				ImGui::SliderFloat("Angle Velocity", &AngularVelocity, -90.0f, 90.0f);
				ImGui::SliderFloat("Angle Velocity", &AngularVelocity, -90.0f, 90.0f);
				ImGui::TreePop();
			}
		}

		static void Extend(lua_State* L)
		{
			luabridge::getGlobalNamespace(L)
				//Rectangle
				.beginClass<Rectangle>("Rectangle")
					.addData("x", &Rectangle::x)
					.addData("y", &Rectangle::y)
					.addData("width", &Rectangle::width)
					.addData("height", &Rectangle::height)
					.addFunction("set",
						std::function<void(Rectangle*, float, float, float, float)>(
							[](Rectangle* r, float x, float y, float w, float h) {
								r->x = x; r->y = y; r->width = w; r->height = h;
							}
							))
				.endClass()
				//vector2
				.beginClass<Vector2>("Vector2")
					.addData("x", &Vector2::x)
					.addData("y", &Vector2::y)
					.addFunction("set", std::function<void(Vector2*, float, float)>([](Vector2* r, float x, float y) { r->x = x; r->y = y;  }))
				.endClass()
				//Texture2d
				.beginClass<Texture2D>("Texture2D")
					.addData("id", &Texture2D::id)
					.addData("width", &Texture2D::width)
					.addData("height", &Texture2D::height)
				.endClass()
				.beginNamespace("LSE")
				// Transform
					.beginClass<LSE::Transform>("Transform")
						.addData("Position", &LSE::Transform::Position)
						.addData("Size", &LSE::Transform::Size)
						.addData("Origin", &LSE::Transform::Origin)
						.addData("angle", &LSE::Transform::angle)
					.endClass()
				// Material
					.beginClass<LSE::Material>("Material")
								.addFunction("SetTexture", &LSE::Material::SetTexture)
								.addFunction("SetTextureByAlias",& LSE::Material::SetTextureByAlias)
								.addFunction("SetColor", &LSE::Material::SetColor)
								.addFunction("SetColorVec", &LSE::Material::SetColorVec)
								.addData("source", &LSE::Material::source)
								.addData("uv_scroll", &LSE::Material::uv_scroll)
					.endClass()
				// GameObject
					.beginClass<LSE::GameObject>("GameObject")
						.addConstructor<void(*)(std::string)>()
						.addConstructor<void(*)(std::string, float, float, float, float)>()
						//.addConstructor<void(*)(std::string, LSE::Transform)>()
						.addData("name", &LSE::GameObject::name)
						.addData("enabled", &LSE::GameObject::enabled)
						.addData("visible", &LSE::GameObject::visible)
						.addData("transform", &LSE::GameObject::transform)
						.addData("mat", &LSE::GameObject::mat)
						.addData("destroy", &LSE::GameObject::destroy)
						.addData("velocity", &LSE::GameObject::velocity)
						.addData("AngularVelocity", &LSE::GameObject::AngularVelocity)
					.endClass()
				.endNamespace()
				.beginNamespace("Raylib")
					.addFunction("GetScreenWidth", GetScreenWidth)
					.addFunction("GetScreenHeight", GetScreenHeight)
				.endNamespace();
		}

	};


	bool CheckLua(lua_State* State, int returnValue)
	{
		if (returnValue != LUA_OK)
		{
			printf("LuaSceneTemplate::Error::\t%s\n", lua_tostring(State, -1));
			return false;
		}

		return true;
	}
}

VectorWrapper<LSE::GameObject*>* LSE::GameObject::owner = NULL;

class LuaSceneTemplate : public Scene
{
public: 
	lua_State* L;
	VectorWrapper<LSE::GameObject*> gameObjects;
	std::map<std::string, std::string> scenes;
	std::map<std::string, Texture2D> textures;
public:
	LuaSceneTemplate(const char* scriptPath, const char* startFunctionName)
		: Scene()
		, L(NULL)
		, gameObjects({})
		, scenes({})
	{
		scenes["Default"] = scriptPath;
		LSE::GameObject::owner = &this->gameObjects;
		GlobalManager::Set();

		LoadScript(scenes["Default"], startFunctionName);
	}
	~LuaSceneTemplate()
	{
		unloadScript("onSceneEnd");
		lua_close(L);
		L = NULL;
		LSE::GameObject::owner = NULL;
		gameObjects.vec.clear();
		GlobalManager::Reset();

	}
	void LoadScript(std::string path, const char* startFunctionName)
	{
		if (L != NULL) {
			unloadScript("onSceneEnd");

			lua_close(L);
			L = NULL;
		}
		L = luaL_newstate();
		luaL_openlibs(L);

		if (LSE::CheckLua(L, luaL_dofile(L, path.c_str())))
		{
			LSE::GameObject::Extend(L);

			luabridge::LuaRef func = luabridge::getGlobal(L, startFunctionName);
			try {
				func();
			}
			catch (luabridge::LuaException const& e) {
				printf("%s\n", e.what());
			}
		}
	}
	void unloadScript(const char* functionName)
	{
		//LSE::GameObject::Extend(L);

		luabridge::LuaRef func = luabridge::getGlobal(L, functionName);
		try {
			func();
		}
		catch (luabridge::LuaException const& e) {
			printf("%s\n", e.what());
		}


	}

	virtual void Draw()
	{
		//GlobalManager::Draw();
		for (auto&& object : gameObjects.vec) object->Draw();
		GlobalManager::CanvasDraw();
	}
	virtual void Update(const float& deltaTime)
	{
		for (auto&& object : gameObjects.vec) object->Update(deltaTime);
		GlobalManager::Update(deltaTime);

	}
	virtual void FixedUpdate(const float& timeStep)
	{
	}

	virtual void Debug()
	{
		if (ImGui::TreeNode(TextFormat("Game Objects: %d", gameObjects.vec.size())))
		{
			for (auto&& object : gameObjects.vec) object->Debug();
			ImGui::TreePop();
		}

	}

	static Scene* Create() { return new LuaSceneTemplate("Assets/Scripts/TemplateScene.lua", "onSceneStart"); }
};
static int scene000 = RegisterScene("Template", "Lua: box drawing", LuaSceneTemplate::Create);
