#pragma once
#include "Node2d.h"
namespace ECS
{
	struct Text
	{
		std::string string = "";
		int cursor = 0;
		int inc_size = 2;
		float writeSpeed = 0.1f;
		float expirationLimit = 5.0f;
		float timer = 0.0f;
		float fontSize = 20.0f;
		float fontSpacing = 1.0f;
		bool visible = false;
		bool expires = true;

		Color color = RAYWHITE;
		void update(const float& dt);
		void setText(std::string s, bool resetCursor) { string = s; cursor = resetCursor ? 0 : s.length(); visible = true; }
		void inspect(const char* title = "Text");
	};
	class CanvasNode : public ECS::Node2d
	{
	public:
		CanvasNode(const char* name);
		~CanvasNode();

		virtual void Update(const float& deltaTime) override;
		virtual void Draw() override;
		virtual void UIDraw() override;

		virtual void inspect() override;
		static void Extend(lua_State* L);
	public:
		ECS::Text mText;
	};
}
