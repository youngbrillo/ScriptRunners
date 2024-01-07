#pragma once
#include "Scene.h"
#include "Node2d.h"
#include "Components2d.h"
#include <vector>
#include <lua.hpp>
#include <memory>
#include "BoxMouse.h"
#include <box2d/box2d.h>

class Scene2d : public Scene
{
public:
    Scene2d(const char* path);
    virtual ~Scene2d();
    virtual void Initialize() override;
    virtual void Update(const float& deltaTime);
    virtual void FixedUpdate(const float& timeStep);
    virtual void Draw();
    virtual void Debug() override;
    virtual void PollEvents() override;

public:
    static Scene* Create(const char* path) { return new Scene2d("path"); }
    static Scene2d* Instance();
    
public:
    const char* scriptPath;
    lua_State* L;
    ECS::Camera2d camera;
    std::vector<std::shared_ptr<ECS::Node2d>> Nodes;
    bool tryUpdate, tryDraw, tryUIDraw, tryPoll;

    ECS::Node2d* CreateNode2d(const char* name);
    static ECS::Node2d* iCreateNode2d(const char* name);
protected:
    void removeDeadNodes();
    void InitScript(const char* path);
    static bool CallLuaFunction(lua_State* L, const char* funcName);
    static bool CallLuaFunctionf(lua_State* L, const char* funcName, float v);
    static bool CallLuaFunctioni(lua_State* L, const char* funcName, int v);

    static void Extend(lua_State* L);
public:
    b2World* world;
    ECS::Box2dDraw b2drawer;
    Box2dMouse* boxMouse;
    b2Vec2 Gravity;

};

