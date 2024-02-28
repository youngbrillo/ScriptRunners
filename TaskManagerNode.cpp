#include "TaskManagerNode.h"

ECS::TaskManagerNode::TaskManagerNode(std::string n)
	: ECS::Node2d(n)
	, Deliveries({})
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
	return 0;
}

Job::Delivery* ECS::TaskManagerNode::GetDelivery(int)
{
	return nullptr;
}

void ECS::TaskManagerNode::inspect()
{
}

void ECS::TaskManagerNode::Extend(lua_State* L)
{
}
