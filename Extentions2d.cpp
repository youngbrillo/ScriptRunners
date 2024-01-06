#include "Extentions2d.h"
#include <LuaBridge/LuaBridge.h>
#include "Components2d.h"
#include "RayExtend.h"
#include "Node2d.h"

void ECS::ExtendRayLib(lua_State* L){
	ERaylib::Extend(L);
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
		//Camera2d
		.beginClass<Camera2D>("Camera2D")
			.addData("offset", &Camera2D::offset)
			.addData("target", &Camera2D::target)
			.addData("zoom", &Camera2D::zoom)
			.addData("rotation", &Camera2D::rotation)
		.endClass();
}
namespace ECS
{
	static void ExtendTransform(lua_State* L) {

		luabridge::getGlobalNamespace(L)
			.beginNamespace("ECS")
			.beginClass<ECS::Transform>("Transform")
			.addData("position", &ECS::Transform::position)
			.addData("size", &ECS::Transform::size)
			.addData("origin", &ECS::Transform::origin)
			.addData("rotation", &ECS::Transform::rotation)
			.addFunction("Center", &ECS::Transform::Center)
			.addFunction("Align", &ECS::Transform::Align)
			.endClass()
			.endNamespace();
	}
	static void ExtendMaterial(lua_State* L) {

		auto setShapeFunc = std::function<void(ECS::Material*, int)>([](ECS::Material* r, int i) { r->shape = (ECS::shape_)i; });
		auto getShapeFunc = std::function<int(const ECS::Material*)>([](const ECS::Material* r) { int x = (int)r->shape; return x; });
		luabridge::getGlobalNamespace(L)
			.beginNamespace("ECS")
			.beginClass<ECS::Material>("Material")
			.addFunction("SetTexture", &ECS::Material::SetTexture)
			.addFunction("SetTextureByAlias", &ECS::Material::SetTexture)
			.addFunction("SetColor", &ECS::Material::SetColorH)
			.addFunction("SetColorVec", &ECS::Material::SetColorV)
			.addData("source", &ECS::Material::source)
			.addData("uv_scroll", &ECS::Material::uv_scroll)
			.addProperty("shape", getShapeFunc, setShapeFunc)
			.endClass()
			.endNamespace();
	}
	void ExtendInput(lua_State* L) {
		luabridge::getGlobalNamespace(L)
			.beginNamespace("ECS")
			.beginClass<ECS::KeyInput>("KeyInput")
			.addData("just_pressed", &ECS::KeyInput::just_pressed)
			.addData("just_released", &ECS::KeyInput::just_released)
			.addData("isDown", &ECS::KeyInput::isDown)
			.endClass()
			.endNamespace();
	}
	void ExtendCamera(lua_State* L) {

		luabridge::getGlobalNamespace(L)
			.beginNamespace("ECS")
			.beginClass<ECS::Camera2d>("Camera2d")
			.addData("cam", &ECS::Camera2d::cam)
			.addData("input", &ECS::Camera2d::input)
			.endClass()
			.endNamespace();
	}
	void ExtendNode2d(lua_State* L) {

		luabridge::getGlobalNamespace(L)
			.beginNamespace("ECS")
			.beginClass<ECS::Node2d>("Node2d")
			.addData("transform", &ECS::Node2d::transform)
			.addData("material", &ECS::Node2d::material)
			.addData("alive", &ECS::Node2d::alive)
			.addData("enabled", &ECS::Node2d::enabled)
			.addData("visible", &ECS::Node2d::visible)
			.addData("direction", &ECS::Node2d::direction)
			.addData("speed", &ECS::Node2d::speed)
			.endClass()
			.endNamespace();
	}
}

void ECS::ExtendAll(lua_State* L){
	ECS::ExtendRayLib(L);
	ECS::ExtendTransform(L);
	ECS::ExtendMaterial(L);
	ECS::ExtendInput(L);
	ECS::ExtendNode2d(L);
	ECS::ExtendCamera(L);
}
