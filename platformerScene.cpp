/*******************************************************************************************
*
*   raylib [core] example - automation events
*
*   Example originally created with raylib 5.0, last time updated with raylib 5.0
*
*   Example based on 2d_camera_platformer example by arvyy (@arvyy)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include "Scene.h"
#include "RenderPipeline.h"
#include <vector>
#include <imgui.h>

#define GRAVITY 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

#define MAX_ENVIRONMENT_ELEMENTS    5


namespace automation{
    struct Player {
        Vector2 position;
        Vector2 size = { 40, 40 };
        float speed;
        bool canJump;
        Color color = RED;
        bool hitObstacle = false;
        bool crouched = false;
        void Draw()
        {
            DrawRectangleRec(Rectangle{ position.x - size.x *0.5f, position.y - size.x, size.x, size.y }, color);
        }

        void Update(const float& deltaTime)
        {
            // Update player
            //----------------------------------------------------------------------------------
            if (IsKeyDown(KEY_LEFT)) position.x -= PLAYER_HOR_SPD * deltaTime;
            if (IsKeyDown(KEY_RIGHT)) position.x += PLAYER_HOR_SPD * deltaTime;

            if (!hitObstacle)
            {
                position.y += speed * deltaTime;
                speed += GRAVITY * deltaTime;
                canJump = false;
            }
            else canJump = true;
        }
        void Poll()
        {
            if (IsKeyDown(KEY_UP) && canJump)
            {
                speed = -PLAYER_JUMP_SPD;
                canJump = false;
            }

            crouched = IsKeyDown(KEY_DOWN);

        }
        void Debug()
        {
            if (ImGui::TreeNode("Player"))
            {
                ImGui::InputFloat2("Position", &position.x);
                ImGui::InputFloat2("size", &size.x);
                ImGui::InputFloat("speed", &speed, 1.0f, 10.0f);
                ImGui::Checkbox("can Jump", &canJump);
                ImGui::Checkbox("hit Obstacle", &hitObstacle);
                ImGui::Checkbox("crouched", &crouched);

                ImGui::TreePop();
            }
        }
    };

    struct EnvElement {
        Rectangle rect;
        int blocking;
        Color color;
        bool passThrough = true;
    };

}


class AutoMationEventScene: public Scene
{
public:
    AutoMationEventScene();
    ~AutoMationEventScene();
    virtual void Update(const float& deltaTime);
    virtual void FixedUpdate(const float& timeStep);
    virtual void Draw();
    virtual void Debug() override;
    virtual void PollEvents() override;
public:
    automation::Player player;
    std::vector< automation::EnvElement> envElements; // [MAX_ENVIRONMENT_ELEMENTS] ;
    Camera2D camera = { 0 };
    bool camera_lock = true;

    static Scene* Create() {return new AutoMationEventScene(); }
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
AutoMationEventScene::AutoMationEventScene()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const float screenWidth = GetScreenWidth();
    const float screenHeight = GetScreenHeight();


    // Define player
    player = { 0 };
    player.position = Vector2{ screenWidth * 400 / 800, screenHeight * 280 / 450 };
    player.size = Vector2{ 40, 40 };
    player.speed = 0;
    player.canJump = false;

    // Define environment elements (platforms)

    envElements.push_back(automation::EnvElement{ { 0, 0, 1000, 400 }, 0, LIGHTGRAY });
    envElements.push_back(automation::EnvElement{ { 0, 400, 1000, 200 }, 1, GRAY , false});
    envElements.push_back(automation::EnvElement{ { 300, 200, 400, 10 }, 1, DARKGRAY });
    envElements.push_back(automation::EnvElement{ { 250, 300, 100, 10 }, 1, GRAY });
    envElements.push_back(automation::EnvElement{ { 650, 300, 100, 10 }, 1, GRAY });
    envElements.push_back(automation::EnvElement{ { 650, 100, 100, 10 }, 1, DARKGRAY });

    // Define camera
    camera.target = player.position;
    camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}
AutoMationEventScene::~AutoMationEventScene() {

}


void AutoMationEventScene::Update(const float& deltaTime)
{
    player.hitObstacle = false;
    for (auto&& element : envElements)
    {
        Vector2* p = &(player.position);
        if (element.blocking &&
            element.rect.x <= p->x &&
            element.rect.x + element.rect.width >= p->x &&
            element.rect.y >= p->y &&
            element.rect.y <= p->y + player.speed * deltaTime)
        {
            player.hitObstacle = player.crouched && element.passThrough ? false : true;
            if (player.hitObstacle)
            {
                player.speed = 0.0f;
                p->y = element.rect.y;
            }
        }
    }
    player.Update(deltaTime);

    float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;


    for (auto&& element: envElements)
    {
        minX = fminf(element.rect.x, minX);
        maxX = fmaxf(element.rect.x + element.rect.width, maxX);
        minY = fminf(element.rect.y, minY);
        maxY = fmaxf(element.rect.y + element.rect.height, maxY);
    }

    Vector2 max = GetWorldToScreen2D(Vector2 { maxX, maxY }, camera);
    Vector2 min = GetWorldToScreen2D(Vector2 { minX, minY }, camera);

    if (max.x < GetScreenWidth()) camera.offset.x = GetScreenWidth() - (max.x - GetScreenWidth() / 2);
    if (max.y < GetScreenHeight()) camera.offset.y = GetScreenHeight() - (max.y - GetScreenHeight() / 2);
    if (min.x > 0) camera.offset.x = GetScreenWidth() / 2 - min.x;
    if (min.y > 0) camera.offset.y = GetScreenHeight() / 2 - min.y;

}
void AutoMationEventScene::FixedUpdate(const float& timeStep) {

}
void AutoMationEventScene::Draw() {
    // Draw
    //----------------------------------------------------------------------------------
    BeginMode2D(camera);

    // Draw environment elements
    for (auto&&E : envElements)
    {
        DrawRectangleRec(E.rect, E.color);
    }

    // Draw player rectangle
    
    player.Draw();

    EndMode2D();

    // Draw game controls
    DrawRectangle(10, 10+20, 290, 145 + 20, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(10, 10 + 20, 290, 145 + 20, Fade(BLUE, 0.8f));

    DrawText("Controls:", 20, 20 + 20, 10, BLACK);
    DrawText("- RIGHT | LEFT: Player movement", 30, 40 + 20, 10, DARKGRAY);
    DrawText("- SPACE: Player jump", 30, 60 + 20, 10, DARKGRAY);
    DrawText("- R: Reset game state", 30, 80 + 20, 10, DARKGRAY);

}
void AutoMationEventScene::Debug() {
    ImGui::Checkbox("camera lock", &camera_lock);
    player.Debug();
}
void AutoMationEventScene::PollEvents() {
    player.Poll();
    camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.25f) camera.zoom = 0.25f;

    if (IsKeyPressed(KEY_R))
    {
        // Reset game state
        player.position = Vector2{ 400, 280 };
        player.speed = 0;
        player.canJump = false;

        camera.target = player.position;
        camera.offset = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
    }
    //----------------------------------------------------------------------------------

    // Update camera
    //----------------------------------------------------------------------------------
    
    if (camera_lock) camera.target = player.position;
    camera.offset = Vector2{ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
}

static int scene000 = RegisterScene("Raylib", "Platformer", AutoMationEventScene::Create);
