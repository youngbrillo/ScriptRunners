#pragma once
#include "Node2d.h"
#include "AnimationHelper.h"

namespace ECS
{
	class Sprite2d : public ECS::Node2d
	{
	public:
		Sprite2d(const char* name, const char* alias);
		~Sprite2d();

		virtual void SetTexture(const char* alias);
		virtual void Update(const float& deltaTime);
		virtual void updateSprite(const float& dt);
		virtual void Draw();
		virtual void inspect();

		static void Extend(lua_State* L);

	public:
		bool SetState(const char*, bool);
		bool SetState(const char*, float);
		bool SetState(const char*, std::string);
	public:
		Animation::Resource animator;
		std::vector<FrameData> frames;
		float timer = 0.0f;
		int frame_index = 0;
		bool frameDone = false;
		bool frameDoOnce = false;
	};
}

