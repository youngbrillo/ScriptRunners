#pragma once
#include "RenderPipeline.h"
#include <lua.hpp>

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
		int key = KEY_NULL;
		bool just_pressed = false;
		bool just_released = false;
		bool isDown = false;
		virtual bool Poll() {
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
}