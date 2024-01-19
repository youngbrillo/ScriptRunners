#pragma once
#include "Sprite2d.h"
#include "InteractableNode.h"

namespace ECS
{
    class NPCNode : public ECS::Sprite2d
    {
    public:
        NPCNode(const char* name, const char* alias, const char* icon, const char* script);
        ~NPCNode();

        virtual void Update(const float& dt) override;
        virtual void Draw() override;
        virtual void Poll() override;
        virtual void BeginContact(b2Contact* contact, ECS::Node2d* other) override;
        virtual void EndContact(b2Contact* contact, ECS::Node2d* other) override;
        virtual void inspect() override;

        virtual void doInteract();
        void setIconFrame(float, float, float, float);
        static void Extend(lua_State* L);
    public:
        ECS::Node2d* prompter;
        static ECS::KeyInput interact_key;
        ECS::Icon icon;
        bool approached;
    };

}


