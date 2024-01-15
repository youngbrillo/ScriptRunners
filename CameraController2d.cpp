#include "CameraController2d.h"
#include "Scene2d.h"
#include <LuaBridge/LuaBridge.h>
#include <imgui.h>

unsigned int ECS::CameraController2d::Instances = 0;
unsigned int ECS::CameraController2d::Locked_Id = 0;
unsigned int ECS::CameraController2d::Alive = 0;

ECS::CameraController2d::CameraController2d(const char* name)
	:Node2d(name)
	, target(NULL)
{
	auto& gCam = Scene2d::Instance()->camera.cam;
	onEnter = gCam;
	onExit = gCam;

	CameraController2d::Instances++;
	CameraController2d::Alive++;
	this->Instance_Id = CameraController2d::Instances;
}

ECS::CameraController2d::~CameraController2d()
{
	target = NULL;
	CameraController2d::Alive--;
	if (CameraController2d::Alive == 0)
	{
		CameraController2d::Instances = 0.0;
	}
	if (CameraController2d::Locked_Id == this->Instance_Id)
	{
		CameraController2d::Locked_Id = 0;
	}
}

void ECS::CameraController2d::Standardize()
{
	this->rigidbody.bdyDef.type = b2_kinematicBody;
	this->rigidbody.fixDef.isSensor = true;
	this->rigidbody.SetBody(Scene2d::Instance()->world, this->transform, this->material.shape);
}

void ECS::CameraController2d::Update(const float& deltaTime)
{
	if (!this->enabled) return;
	ECS::Node2d::Update(deltaTime);
	if (this->hasTarget())
	{
		if (target->alive == false)
		{
			target = NULL;
		}
	}
	if (!this->canTrack()) return;

	Camera2D* c = camSettings.onEnterCamActive ? &onEnter : camSettings.onExitCamActive ? &onExit : NULL;
	lerpCam(c, deltaTime);
	this->Action(deltaTime);
}

void ECS::CameraController2d::Draw()
{
	ECS::Node2d::Draw();
}

void ECS::CameraController2d::UIDraw()
{
}

void ECS::CameraController2d::Action(const float& dt)
{
	const float DIST_THRESHOLD = 0.0001f;
	if (!camSettings.target_locked) return;
	auto& gCam = Scene2d::Instance()->camera.cam;
	if (this->camSettings.isfollowing)
	{
		if (gCam.target.x != target->transform.position.x)
		{
			float dist = target->transform.position.x - gCam.target.x;
			gCam.target.x += dist * camSettings.follow_speed * dt;
		}

		if (gCam.target.y != target->transform.position.y)
		{
			float dist = target->transform.position.y - gCam.target.y;
			gCam.target.y += dist * camSettings.follow_speed * dt;
		}
	}
}
 template <typename T> 
 static int _sgn(T val) {
	return (T(0) < val) - (val < T(0));
}
void ECS::CameraController2d::lerpCam(Camera2D* cam, const float& dt)
{
	if (cam == NULL) return;
	auto& gCam = Scene2d::Instance()->camera.cam;
	gCam.offset = cam->offset;
	gCam.rotation = cam->rotation;

	const float DIST_THRESHOLD = 0.003f;
	if(gCam.zoom != cam->zoom)
	{
		float dist = _sgn(cam->zoom - gCam.zoom);
		gCam.zoom += dist * camSettings.zoom_adjust_speed * dt;
		float min = cam->zoom - DIST_THRESHOLD;
		float max = cam->zoom + DIST_THRESHOLD;
		if (gCam.zoom >= min && gCam.zoom <= max)
		{
			//camSettings.onEnterCamActive = false;
			//camSettings.onExitCamActive = false;
			gCam.zoom = cam->zoom;
		}
	}

}

void ECS::CameraController2d::SetTarget(ECS::Node2d* _target)
{
	target = _target;
}

void ECS::CameraController2d::BeginContact(b2Contact* contact, ECS::Node2d* other)
{
	if (!this->hasTarget() || other != target)  return;
	camSettings.target_locked = true;
	camSettings.onEnterCamActive = true;
	camSettings.onExitCamActive = false;
	this->Locked_Id = this->Instance_Id;
}

void ECS::CameraController2d::EndContact(b2Contact* contact, ECS::Node2d* other)
{
	if (!this->hasTarget() || other != target)  return;
	camSettings.target_locked = false;
	camSettings.onEnterCamActive = false;
	camSettings.onExitCamActive = true;

}

void ECS::CameraController2d::inspect()
{
	Node2d::inspect();
	if (ImGui::TreeNode("Camera Settings"))
	{
		ImGui::Checkbox("always follow", &camSettings.alwaysFollow);
		ImGui::Checkbox("isfollowing", &camSettings.isfollowing);
		ImGui::Checkbox("bounds_enabled", &camSettings.bounds_enabled);
		ImGui::Checkbox("adjust_on_y_set", &camSettings.adjust_on_y_set);
		ImGui::Checkbox("target_locked", &camSettings.target_locked);
		ImGui::Checkbox("onEnterCamActive", &camSettings.onEnterCamActive);
		ImGui::Checkbox("onExitCamActive", &camSettings.onExitCamActive);

		ImGui::InputFloat("follow_speed", &camSettings.follow_speed, 1.0f, 10.0f);
		ImGui::InputFloat("zoom_adjust_speed", &camSettings.zoom_adjust_speed, 1.0f, 10.0f);
		ImGui::InputFloat2("offset", &camSettings.offset.x);
		ImGui::InputFloat4("bounds", &camSettings.bounds.x);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("onEnter"))
	{
		ImGui::SliderFloat2("offset", &onEnter.offset.x, -GetScreenWidth(), GetScreenWidth());
		ImGui::InputFloat2("target", &onEnter.target.x);
		ImGui::SliderFloat("rotation", &onEnter.rotation, -360, 360);
		ImGui::SliderFloat("zoom", &onEnter.zoom, 1, 100);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("onExit"))
	{
		ImGui::SliderFloat2("offset", &onExit.offset.x, -GetScreenWidth(), GetScreenWidth());
		ImGui::InputFloat2("target", &onExit.target.x);
		ImGui::SliderFloat("rotation", &onExit.rotation, -360, 360);
		ImGui::SliderFloat("zoom", &onExit.zoom, 1, 100);
		ImGui::TreePop();
	}
}

void ECS::CameraController2d::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("ECS")
		.deriveClass<ECS::CameraController2d, ECS::Node2d>("CameraController2d")
		.addFunction("SetTarget", &ECS::CameraController2d::SetTarget)
		.addFunction("Standardize", &ECS::CameraController2d::Standardize)
		.addData("onEnter", &ECS::CameraController2d::onEnter)
		.addData("onExit", &ECS::CameraController2d::onExit)
		.endClass()
		.endNamespace();
}

