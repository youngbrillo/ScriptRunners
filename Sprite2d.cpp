#include "Sprite2d.h"
#include "TextureManager.h"
#include "AnimationStateManager.h"
#include <LuaBridge/LuaBridge.h>

ECS::Sprite2d::Sprite2d(const char* name, const char* alias)
	: ECS::Node2d(name)
{
	SetTexture(alias);
}


ECS::Sprite2d::~Sprite2d()
{
}

void ECS::Sprite2d::SetTexture(const char* alias)
{
	this->material.texture = TextureManager::Instance()->GetTexture(alias);
	this->animator = AnimationStateManager::Instance()->GenerateResource(this->material.texture.id);
	this->frames = animator.GetFrameCurrent();
	this->frame_index = 0;
	this->material.source = this->frames[frame_index].frame;
}

void ECS::Sprite2d::Update(const float& deltaTime)
{
	if (!this->enabled) return;
	Node2d::Update(deltaTime);
	this->updateSprite(deltaTime);


}
void ECS::Sprite2d::updateSprite(const float& dt)
{
	if (frameDone && frameDoOnce) return;

	timer += dt;
	if (timer >= this->frames[frame_index].duration)
	{
		timer = 0;
		frame_index++;
		frameDone = frame_index == frames.size() - 1;
		if (frame_index >= frames.size())
		{
			frame_index = 0;
		}
		this->material.source = this->frames[frame_index].frame;

	}
}
void ECS::Sprite2d::Draw()
{
	Node2d::Draw();
}


void ECS::Sprite2d::inspect()
{
	Node2d::inspect();
	animator.Inspect("local State machine", AnimationStateManager::Instance()->GetAnimator(this->material.texture.id));
	ImGui::SliderFloat("timer", &timer, 0.0f, 1.0f);
	ImGui::SliderInt("frame_index", &frame_index, 0, frames.size() - 1);

	if (animator.lastState != animator.currentState)
	{
		animator.lastState = animator.currentState;
		this->frames = animator.GetFrameCurrent();
		this->frame_index = 0;
		this->material.source = this->frames[frame_index].frame;
	}
}

void ECS::Sprite2d::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("ECS")
			.deriveClass<ECS::Sprite2d, ECS::Node2d>("Sprite2d")
				.addData("SetTexture", &ECS::Node2d::direction)
			.endClass()
		.endNamespace();
}

bool ECS::Sprite2d::SetState(const char* s, bool v)
{
	if (animator.SetFieldb(s, v) && animator.lastState != animator.currentState)
	{
		animator.lastState = animator.currentState;
		this->frames = animator.GetFrameCurrent();
		this->frame_index = 0;
		this->material.source = this->frames[frame_index].frame;
		return true;
	}
	return false;
}

bool ECS::Sprite2d::SetState(const char* s, float v)
{
	if (animator.SetFieldf(s, v) && animator.lastState != animator.currentState)
	{
		animator.lastState = animator.currentState;
		this->frames = animator.GetFrameCurrent();
		this->frame_index = 0;
		this->material.source = this->frames[frame_index].frame;
		return true;
	}
	return false;
}

bool ECS::Sprite2d::SetState(const char* s,  std::string v)
{
	if (animator.SetFields(s, v) && animator.lastState != animator.currentState)
	{
		animator.lastState = animator.currentState;
		this->frames = animator.GetFrameCurrent();
		this->frame_index = 0;
		this->material.source = this->frames[frame_index].frame;
		return true;
	}
	return false;
}
