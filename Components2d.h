#pragma once
#include "RenderPipeline.h"
#include <lua.hpp>
#include <box2d/box2d.h>

namespace ECS
{
	enum shape_
	{
		shape_Rectangle = 0,
		shape_Circle,
		shape_edge
	};
	struct Transform
	{
		Vector2 position = Vector2{ 0.0f, 0.0f };
		Vector2 size = Vector2{ 1.0f, 1.0f };
		Vector2 origin = Vector2{ 0.5f, 0.5f };
		float rotation = 0.0f;

		Rectangle Destination() const;
		Rectangle Rect() const;
		void Center();
		void Align(float x, float y);
		void Debug(const char* title = "Transform");

	};

	struct Material
	{
		Texture2D texture = Texture2D{ 1, 1, 1 };
		Color color = RAYWHITE;
		Rectangle source = { 0, 0, 1, 1 };
		shape_ shape = shape_Rectangle;
		Vector2 uv_scroll = { 0, 0 };
		void Update(const float& dt);
		void SetTexture(const char* alias);
		void SetTextureT(Texture2D t);
		void SetColorH(unsigned int hex);
		void SetColorV(Vector4 vec);
		void SetColorf(float r, float g, float b, float a);
		void Debug(const char* title = "Material");
	};
#if false
	struct RigidBody
	{

	};

	struct Script
	{
		lua_State* L = NULL;
		bool enabled = false;

		const char* onStartFunc = "onStart";
		const char* onEndFunc = "onEnd";
		const char* onUpdateFunc = "onUpdate";
		const char* onDrawFunc = "onDraw";
		const char* onPollEventFunc = "onPollEvent";

		
	};
#endif
	struct KeyInput
	{
		KeyInput() {};
		KeyInput(int k) { key = k;}
		int key = KEY_NULL;
		bool just_pressed = false;
		bool just_released = false;
		bool isDown = false;
		bool canDefer = false;
		virtual bool Poll() {
			if (canDefer) return false;
			just_pressed = IsKeyPressed(key);
			just_released = IsKeyReleased(key);
			isDown = IsKeyDown(key);

			return just_pressed || just_released;
		}

		void Debug(const char* title = "Input");
	};
	struct MouseInput : public KeyInput
	{
		virtual bool Poll() override {
			just_pressed = IsMouseButtonPressed(key);
			just_released = IsMouseButtonReleased(key);
			isDown = IsMouseButtonDown(key);
			return just_pressed || just_released;
		}
	};


	class Camera2d
	{
	public:
		Camera2d();
		~Camera2d();

		void Update();
		void HandleInputs();
		
		void Debug(const char* title = "2d Camera");
	protected:
		void PanCamera();
	public:
		Camera2D	cam;
		Vector2		startPos;
		MouseInput	input;
	};



	class Box2dDraw : public b2Draw
	{
	public:
		bool e_shape, e_joint, e_aabb, e_pair, e_centerOfMass;
	public:
		Box2dDraw()
			:b2Draw()
			, e_shape(true)
			, e_joint(false)
			, e_aabb(false)
			, e_pair(false)
			, e_centerOfMass(false)
		{
			setInternalFlags();
		}
		~Box2dDraw() {

		}
		void setInternalFlags()
		{
			int flags = 0;
			if (e_shape) flags += b2Draw::e_shapeBit;
			if (e_joint) flags += b2Draw::e_jointBit;
			if (e_aabb) flags += b2Draw::e_aabbBit;
			if (e_pair) flags += b2Draw::e_pairBit;
			if (e_centerOfMass) flags += b2Draw::e_centerOfMassBit;

			this->SetFlags(flags);
		}
		virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		virtual void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);
		virtual void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
		virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
		virtual void DrawTransform(const b2Transform& xf);
		virtual void DrawPoint(const b2Vec2& p, float size, const b2Color& color);

		void Debug(const char* title = "Ray b2Draw");
	};
}