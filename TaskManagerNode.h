#pragma once
#include "Node2d.h"
#include "Task.h"
#include <vector>
#include <map>

namespace ECS
{
    class TaskManagerNode : public Node2d
    {
    public:
        TaskManagerNode(std::string n);
        ~TaskManagerNode();

        virtual void Update(const float& deltaTime) override;
        virtual void Draw() override;
        virtual void UIDraw() override;
        virtual void Poll() override;
        virtual void BeginContact(b2Contact* contact, ECS::Node2d* other) override;
        virtual void EndContact(b2Contact* contact, ECS::Node2d* other) override;

        int CreateDeliveryTask(std::string t, std::string d);
        Job::Delivery* GetDelivery(int);

        virtual void inspect() override;
        static void Extend(lua_State* L);

    public:
        std::map<int, Job::Delivery> Deliveries;
    };
}

