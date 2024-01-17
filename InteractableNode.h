#pragma once
#include "Node2d.h"
namespace ECS
{
    struct Icon
    {
        Texture2D texture = { 1, 1, 1 };
        Rectangle frame = {0, 0, 1, 1};
        Rectangle destination = Rectangle{ 0, -1.0f, 1.f, 1.f };
        Vector2 origin = Vector2{0.5f, 0.5f};
        float rotation = 0.0f;
        Color color = RAYWHITE;

        void Inspect(const char* title = "Icon");
    };

    class InteractableNode : public ECS::Node2d
    {
    public:
        InteractableNode(std::string name);
        ~InteractableNode();
        virtual void Draw() override;
        virtual void Update(const float& deltaTime) override;
        virtual void BeginContact(b2Contact* contact, ECS::Node2d* other) override;
        virtual void EndContact(b2Contact* contact, ECS::Node2d* other) override;
        virtual void inspect() override;

    public:
        void setObserver(ECS::Node2d* t);
        void setIconTexture(const char* alias);
        void setIconDestination(float x, float y, float w, float h);
        void setIconFrame(float x, float y, float w, float h);
        static void Extend(lua_State* L);

    protected:
        Rectangle getIconTrueDestination() const;
    public:

        ECS::Node2d* observer;
        bool isInteractive = true;
        Icon icon;

    };
}

