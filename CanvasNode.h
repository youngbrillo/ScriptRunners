#pragma once
#include "Node2d.h"
namespace ECS
{
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
