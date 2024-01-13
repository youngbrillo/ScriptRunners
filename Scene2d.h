#pragma once
#include "Scene.h"
#include "Node2d.h"
#include "Components2d.h"
#include <vector>
#include <lua.hpp>
#include <memory>
#include "BoxMouse.h"
#include <box2d/box2d.h>
#include "Script2d.h"

class Scene2d : public Scene, public b2ContactListener
{
public:
    Scene2d(const char* path);
    virtual ~Scene2d();
    virtual void Initialize() override;
    virtual void Update(const float& deltaTime);
    virtual void FixedUpdate(const float& timeStep);
    virtual void Draw();
    virtual void Debug() override;
    void DebugNode(ECS::Node2d* Node, const char* title);
    virtual void PollEvents() override;
protected:
    bool validateContact(b2Contact* contact, std::shared_ptr<ECS::Node2d>& A, std::shared_ptr<ECS::Node2d>& B);
    //bool validateContact(b2Contact* contact, ECS::Node2d* A, ECS::Node2d* B);
public:
    virtual void BeginContact(b2Contact* contact) override;
    virtual void EndContact(b2Contact* contact) override;
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

public:
    static Scene* Create(const char* path) { return new Scene2d(path); }
    static Scene2d* Instance();
    
public:
    const char* scriptPath;
    ECS::Camera2d camera;
    std::vector<std::shared_ptr<ECS::Node2d>> Nodes;
    int NodeA = -1, NodeB = -1;
    bool tryUpdate, tryDraw, tryUIDraw, tryPoll;

    b2World* world;
    ECS::Box2dDraw b2drawer;
    Box2dMouse* boxMouse;
    b2Vec2 Gravity;
    ECS::Script2d script;
protected:
    ECS::Node2d* CreateNode2d(const char* name);
    static ECS::Node2d* iCreateNode2d(const char* name);

    static b2World* GetWorld() { return Instance()->world; }
    void removeDeadNodes();
    void InitScript(const char* path);
    static void Extend(lua_State* L);

};

