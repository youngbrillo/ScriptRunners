#pragma once
#include "Sprite2d.h"
#include "PlatformerController.h"
#include "Player.h";
namespace ECS
{
	class PlayerController : public ECS::Sprite2d
	{
	public:
		PlayerController(const char* name, const char* alias);
		~PlayerController();

		//virtual overrides
		virtual void Update(const float& dt) override;
		virtual void FixedUpdate(const float& dt) override;
		virtual void Draw() override;
		virtual void UIDraw() override;
		virtual void Poll() override;
		virtual void inspect() override;


		void handleMovement(const float& dt);
		void handleXmovement(const float& dt);
		void handleYmovement(const float& dt);
		void handleAnimState(const float& dt);
		void handleActions(const float& dt);

	public:
		static void Extend(lua_State* L);
		SR::Player mPlayer;
	};
}


