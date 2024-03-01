#include "TaskManagerNode.h"
#include <LuaBridge/LuaBridge.h>
#include "Extentions2d.h"
#include "Scene2d.h"
#include <imgui.h>

ECS::TaskManagerNode::TaskManagerNode(std::string n)
	: ECS::Node2d(n)
	, Deliveries({})
	, DeliveryIndex(0)
	, currentObjective(-1)
{
}

ECS::TaskManagerNode::~TaskManagerNode()
{
	
}

void ECS::TaskManagerNode::Update(const float& deltaTime)
{
}

void ECS::TaskManagerNode::Draw()
{
}

void ECS::TaskManagerNode::UIDraw()
{
	if (currentObjective != -1)
	{
		DrawTextEx(
			GetFontDefault(),
			Deliveries.at(currentObjective).title.c_str(),
			coPos,
			coFontSize,
			1,
			coColor
		);
		Vector2 coDPos = coPos;
		coDPos.y += coFontSize * 1.5f;
		coDPos.x += 25;
		DrawTextEx( GetFontDefault(), Deliveries.at(currentObjective).description.c_str(), coDPos, coFontSize,1, coColor);
	}
}

void ECS::TaskManagerNode::Poll()
{
}

void ECS::TaskManagerNode::BeginContact(b2Contact* contact, ECS::Node2d* other)
{
}

void ECS::TaskManagerNode::EndContact(b2Contact* contact, ECS::Node2d* other)
{
}

int ECS::TaskManagerNode::CreateDeliveryTask(std::string t, std::string d)
{
	++DeliveryIndex;
	Deliveries[DeliveryIndex] = Job::Delivery(t, d);
	return DeliveryIndex;
}

Job::Delivery* ECS::TaskManagerNode::GetDelivery(int id)
{
	if (Deliveries.find(id) != Deliveries.end())
		return &Deliveries.at(id);
	else
		return nullptr;
}

void ECS::TaskManagerNode::SetCurrentObjective(int id)
{
	if (Deliveries.find(id) != Deliveries.end())
	{
		currentObjective = id;
	}
	else
		currentObjective = -1;
}

int ECS::TaskManagerNode::getCurrentObjective() const
{
	return currentObjective;
}

void ECS::TaskManagerNode::inspect()
{
	Node2d::inspect();
	ImGui::SliderFloat("coPos.x", &coPos.x, 0, GetScreenWidth());
	ImGui::SliderFloat("coPos.y", &coPos.y, 0, GetScreenHeight());
	ImGui::InputFloat("coFontSize", &coFontSize, 1.0f, 10.0f);

	Vector4 a = ColorNormalize(coColor);
	if (ImGui::ColorEdit4("Color", &a.x))
	{
		coColor = ColorFromNormalized(a);
	}

	if (ImGui::TreeNode("Deliveries"))
	{
		for (auto&& p : Deliveries)
		{
			p.second.Inspect();
		}

		ImGui::TreePop();
	}
}
static ECS::TaskManagerNode* CreateTaskManagerNode(std::string name)
{
	auto n = std::make_shared<ECS::TaskManagerNode>(name);
	Scene2d::Instance()->Nodes.emplace_back(n);
	return n.get();
}

void ECS::TaskManagerNode::Extend(lua_State* L)
{
	Job::Task::Extend(L);
	Job::Delivery::Extend(L);

	luabridge::getGlobalNamespace(L)
	.beginNamespace("Scene")
		.addFunction("CreateTaskManagerNode", CreateTaskManagerNode)
	.endNamespace()
	.beginNamespace("ECS")
		.deriveClass<ECS::TaskManagerNode, ECS::Node2d>("TaskManagerNode")
			.addFunction("CreateDeliveryTask", &ECS::TaskManagerNode::CreateDeliveryTask)
			.addFunction("GetDelivery", &ECS::TaskManagerNode::GetDelivery)
			.addProperty("currentObjective", &ECS::TaskManagerNode::getCurrentObjective, &ECS::TaskManagerNode::SetCurrentObjective)
		.endClass()
	.endNamespace();
}

static int tmn_registration = ExtensionManager::Register(ECS::TaskManagerNode::Extend);
