#pragma once
#include "Node2d.h"
#include <memory>
namespace ECS
{
    struct CameraSettings
    {
        bool alwaysFollow = true;
        bool isfollowing = true;
        bool bounds_enabled = false;
        bool adjust_on_y_set = false;
        bool target_locked = false;
        bool onEnterCamActive = false;
        bool onExitCamActive = false;

        Rectangle bounds = { 0, 0, 0, 0 };
        Vector2 offset = { 0.0f, 0.0f };
        float follow_speed = 3.0f;
        float zoom_adjust_speed = 10.0f;

      
    };
    class CameraController2d : public ECS::Node2d
    {
    public:
        CameraController2d(const char* name);
        ~CameraController2d();

        virtual void Standardize(); //configure rigidbody properties
        virtual void Update(const float& deltaTime);
        virtual void Draw();
        virtual void UIDraw();

        virtual void Action(const float& dt);
        virtual void lerpCam(Camera2D* cam, const float& dt);
        //virtual void SetTarget(std::shared_ptr<ECS::Node2d> target);
        virtual void SetTarget(ECS::Node2d* _target);
        virtual void BeginContact(b2Contact* contact, ECS::Node2d* other) override;
        virtual void EndContact(b2Contact* contact, ECS::Node2d* other) override;
        virtual void inspect();

        static void Extend(lua_State* L);

        bool hasTarget() const { return target != NULL; }
        bool canTrack() const { return hasTarget() && Instance_Id == Locked_Id; }
    protected:
        ECS::Node2d* target;
        CameraSettings camSettings;
        Camera2D onEnter;
        Camera2D onExit;


    private:
        static unsigned int Instances;
        static unsigned int Locked_Id;
        static unsigned int Alive;
        unsigned int Instance_Id;
    };
}


