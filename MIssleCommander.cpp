#include "Scene.h"
#include "App.h"
#include "RenderPipeline.h"

#define MAX_MISSILES                100
#define MAX_INTERCEPTORS            30
#define MAX_EXPLOSIONS              100
#define LAUNCHERS_AMOUNT            3           // Not a variable, should not be changed
#define BUILDINGS_AMOUNT            6           // Not a variable, should not be changed

#define LAUNCHER_SIZE               80
#define BUILDING_SIZE               60
#define EXPLOSION_RADIUS            40

#define MISSILE_SPEED               1
#define MISSILE_LAUNCH_FRAMES       80
#define INTERCEPTOR_SPEED           10
#define EXPLOSION_INCREASE_TIME     90          // In frames
#define EXPLOSION_TOTAL_TIME        210         // In frames

#define EXPLOSION_COLOR             Color{ 125, 125, 125, 125 }

namespace MissleCommander
{
    typedef struct Missile {
        Vector2 origin;
        Vector2 position;
        Vector2 objective;
        Vector2 speed;

        bool active;
    } Missile;

    typedef struct Interceptor {
        Vector2 origin;
        Vector2 position;
        Vector2 objective;
        Vector2 speed;

        bool active;
    } Interceptor;

    typedef struct Explosion {
        Vector2 position;
        float radiusMultiplier;
        int frame;
        bool active;
    } Explosion;

    typedef struct Launcher {
        Vector2 position;
        bool active;
    } Launcher;

    typedef struct Building {
        Vector2 position;
        bool active;
    } Building;
}

class MissleCommanderScene : public Scene
{
public:
	MissleCommanderScene();
	~MissleCommanderScene();

	void Init();
	virtual void Update(const float& dt);
	virtual void FixedUpdate(const float& timeStep);
	virtual void Run(const float& delta);
	virtual void Draw();
	virtual void PollEvents() override;
	static Scene* Create() { return new MissleCommanderScene(); }
    void UpdateOutgoingFire();
    void UpdateIncomingFire();

    virtual void Debug()
    {

    }
public:
    int framesCounter = 0;
    bool gameOver = false;
    int score = 0;
    MissleCommander::Missile missile[MAX_MISSILES] = { 0 };
    MissleCommander::Interceptor interceptor[MAX_INTERCEPTORS] = { 0 };
    MissleCommander::Explosion explosion[MAX_EXPLOSIONS] = { 0 };
    MissleCommander::Launcher launcher[LAUNCHERS_AMOUNT] = { 0 };
    MissleCommander::Building building[BUILDINGS_AMOUNT] = { 0 };
    int explosionIndex = 0;


    bool fire1, fire2, fire3;
};

MissleCommanderScene::MissleCommanderScene()
    :Scene()
    , fire1(false)
    , fire2(false)
    , fire3(false)
{
    Init();
}

MissleCommanderScene::~MissleCommanderScene()
{
}

void MissleCommanderScene::Init()
{

    // Initialize missiles
    for (int i = 0; i < MAX_MISSILES; i++)
    {
        missile[i].origin = Vector2{ 0, 0 };
        missile[i].speed = Vector2{ 0, 0 };
        missile[i].position = Vector2{ 0, 0 };

        missile[i].active = false;
    }

    // Initialize interceptors
    for (int i = 0; i < MAX_INTERCEPTORS; i++)
    {
        interceptor[i].origin = Vector2{ 0, 0 };
        interceptor[i].speed = Vector2{ 0, 0 };
        interceptor[i].position = Vector2{ 0, 0 };

        interceptor[i].active = false;
    }

    // Initialize explosions
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        explosion[i].position = Vector2{ 0, 0 };
        explosion[i].frame = 0;
        explosion[i].active = false;
    }

    // Initialize buildings and launchers
    int sparcing = GetScreenWidth() / (LAUNCHERS_AMOUNT + BUILDINGS_AMOUNT + 1);

    // Buildings and launchers placing
    launcher[0].position = Vector2{ 1.0f * sparcing, (float)GetScreenHeight() - LAUNCHER_SIZE / 2 };
    building[0].position = Vector2{ 2.0f * sparcing, (float)GetScreenHeight() - BUILDING_SIZE / 2 };
    building[1].position = Vector2{ 3.0f * sparcing, (float)GetScreenHeight() - BUILDING_SIZE / 2 };
    building[2].position = Vector2{ 4.0f * sparcing, (float)GetScreenHeight() - BUILDING_SIZE / 2 };
    launcher[1].position = Vector2{ 5.0f * sparcing, (float)GetScreenHeight() - LAUNCHER_SIZE / 2 };
    building[3].position = Vector2{ 6.0f * sparcing, (float)GetScreenHeight() - BUILDING_SIZE / 2 };
    building[4].position = Vector2{ 7.0f * sparcing, (float)GetScreenHeight() - BUILDING_SIZE / 2 };
    building[5].position = Vector2{ 8.0f * sparcing, (float)GetScreenHeight() - BUILDING_SIZE / 2 };
    launcher[2].position = Vector2{ 9.0f * sparcing, (float)GetScreenHeight() - LAUNCHER_SIZE / 2 };

    // Buildings and launchers activation
    for (int i = 0; i < LAUNCHERS_AMOUNT; i++) launcher[i].active = true;
    for (int i = 0; i < BUILDINGS_AMOUNT; i++) building[i].active = true;

    // Initialize game variables
    score = 0;
}

void MissleCommanderScene::Update(const float& dt)
{
}

void MissleCommanderScene::FixedUpdate(const float& timeStep)
{
    Run(timeStep);
}

void MissleCommanderScene::Run(const float& delta)
{
    if (gameOver) return;

    framesCounter++;

    static float distance;

    // Interceptors update
    for (int i = 0; i < MAX_INTERCEPTORS; i++)
    {
        if (interceptor[i].active)
        {
            // Update position
            interceptor[i].position.x += interceptor[i].speed.x;
            interceptor[i].position.y += interceptor[i].speed.y;

            // Distance to objective
            distance = sqrt(pow(interceptor[i].position.x - interceptor[i].objective.x, 2) +
                pow(interceptor[i].position.y - interceptor[i].objective.y, 2));

            if (distance < INTERCEPTOR_SPEED)
            {
                // Interceptor dissapears
                interceptor[i].active = false;

                // Explosion
                explosion[explosionIndex].position = interceptor[i].position;
                explosion[explosionIndex].active = true;
                explosion[explosionIndex].frame = 0;
                explosionIndex++;
                if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;

                break;
            }
        }
    }

    // Missiles update
    for (int i = 0; i < MAX_MISSILES; i++)
    {
        if (missile[i].active)
        {
            // Update position
            missile[i].position.x += missile[i].speed.x;
            missile[i].position.y += missile[i].speed.y;

            // Collision and missile out of bounds
            if (missile[i].position.y > GetScreenHeight()) missile[i].active = false;
            else
            {
                // CHeck collision with launchers
                for (int j = 0; j < LAUNCHERS_AMOUNT; j++)
                {
                    if (launcher[j].active)
                    {
                        if (CheckCollisionPointRec(missile[i].position, Rectangle {
                            launcher[j].position.x - LAUNCHER_SIZE / 2, launcher[j].position.y - LAUNCHER_SIZE / 2,
                                LAUNCHER_SIZE, LAUNCHER_SIZE
                        }))
                        {
                            // Missile dissapears
                            missile[i].active = false;

                            // Explosion and destroy building
                            launcher[j].active = false;

                            explosion[explosionIndex].position = missile[i].position;
                            explosion[explosionIndex].active = true;
                            explosion[explosionIndex].frame = 0;
                            explosionIndex++;
                            if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;

                            break;
                        }
                    }
                }

                // CHeck collision with buildings
                for (int j = 0; j < BUILDINGS_AMOUNT; j++)
                {
                    if (building[j].active)
                    {
                        if (CheckCollisionPointRec(missile[i].position, Rectangle {
                            building[j].position.x - BUILDING_SIZE / 2, building[j].position.y - BUILDING_SIZE / 2,
                                BUILDING_SIZE, BUILDING_SIZE
                        }))
                        {
                            // Missile dissapears
                            missile[i].active = false;

                            // Explosion and destroy building
                            building[j].active = false;

                            explosion[explosionIndex].position = missile[i].position;
                            explosion[explosionIndex].active = true;
                            explosion[explosionIndex].frame = 0;
                            explosionIndex++;
                            if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;

                            break;
                        }
                    }
                }

                // CHeck collision with explosions
                for (int j = 0; j < MAX_EXPLOSIONS; j++)
                {
                    if (explosion[j].active)
                    {
                        if (CheckCollisionPointCircle(missile[i].position, explosion[j].position, EXPLOSION_RADIUS * explosion[j].radiusMultiplier))
                        {
                            // Missile dissapears and we earn 100 points
                            missile[i].active = false;
                            score += 100;

                            explosion[explosionIndex].position = missile[i].position;
                            explosion[explosionIndex].active = true;
                            explosion[explosionIndex].frame = 0;
                            explosionIndex++;
                            if (explosionIndex == MAX_EXPLOSIONS) explosionIndex = 0;

                            break;
                        }
                    }
                }
            }
        }
    }

    // Explosions update
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosion[i].active)
        {
            explosion[i].frame++;

            if (explosion[i].frame <= EXPLOSION_INCREASE_TIME) explosion[i].radiusMultiplier = explosion[i].frame / (float)EXPLOSION_INCREASE_TIME;
            else if (explosion[i].frame <= EXPLOSION_TOTAL_TIME) explosion[i].radiusMultiplier = 1 - (explosion[i].frame - (float)EXPLOSION_INCREASE_TIME) / (float)EXPLOSION_TOTAL_TIME;
            else
            {
                explosion[i].frame = 0;
                explosion[i].active = false;
            }
        }
    }

    // Fire logic
    UpdateOutgoingFire();
    UpdateIncomingFire();

    // Game over logic
    int checker = 0;

    for (int i = 0; i < LAUNCHERS_AMOUNT; i++)
    {
        if (!launcher[i].active) checker++;
        if (checker == LAUNCHERS_AMOUNT) gameOver = true;
    }

    checker = 0;
    for (int i = 0; i < BUILDINGS_AMOUNT; i++)
    {
        if (!building[i].active) checker++;
        if (checker == BUILDINGS_AMOUNT) gameOver = true;
    }
}

void MissleCommanderScene::Draw()
{
    
        if (!gameOver)
        {
            // Draw missiles
            for (int i = 0; i < MAX_MISSILES; i++)
            {
                if (missile[i].active)
                {
                    DrawLine(missile[i].origin.x, missile[i].origin.y, missile[i].position.x, missile[i].position.y, RED);

                    if (framesCounter % 16 < 8) DrawCircle(missile[i].position.x, missile[i].position.y, 3, YELLOW);
                }
            }

            // Draw interceptors
            for (int i = 0; i < MAX_INTERCEPTORS; i++)
            {
                if (interceptor[i].active)
                {
                    DrawLine(interceptor[i].origin.x, interceptor[i].origin.y, interceptor[i].position.x, interceptor[i].position.y, GREEN);

                    if (framesCounter % 16 < 8) DrawCircle(interceptor[i].position.x, interceptor[i].position.y, 3, BLUE);
                }
            }

            // Draw explosions
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosion[i].active) DrawCircle(explosion[i].position.x, explosion[i].position.y, EXPLOSION_RADIUS*explosion[i].radiusMultiplier, EXPLOSION_COLOR);
            }

            // Draw buildings and launchers
            for (int i = 0; i < LAUNCHERS_AMOUNT; i++)
            {
                if (launcher[i].active) DrawRectangle(launcher[i].position.x - LAUNCHER_SIZE/2, launcher[i].position.y - LAUNCHER_SIZE/2, LAUNCHER_SIZE, LAUNCHER_SIZE, GRAY);
            }

            for (int i = 0; i < BUILDINGS_AMOUNT; i++)
            {
                if (building[i].active) DrawRectangle(building[i].position.x - BUILDING_SIZE/2, building[i].position.y - BUILDING_SIZE/2, BUILDING_SIZE, BUILDING_SIZE, LIGHTGRAY);
            }

            // Draw score
            DrawText(TextFormat("SCORE %4i", score), 20, 20, 40, LIGHTGRAY);

            if (App::GetState() != AppState_Play) DrawText("GAME PAUSED", GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2, GetScreenHeight() / 2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

}

void MissleCommanderScene::PollEvents()
{

    if (gameOver && IsKeyPressed(KEY_ENTER))
    {
        Init();
        gameOver = false;
    }


    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){ fire1 = true;}
    if(IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)){ fire1 = true;}
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) { fire1 = true; }
}
void MissleCommanderScene::UpdateOutgoingFire()
{
    static int interceptorNumber = 0;
    int launcherShooting = 0;

    if (fire1) launcherShooting = 1;
    if (fire2) launcherShooting = 2;
    if (fire3) launcherShooting = 3;

    if (launcherShooting > 0 && launcher[launcherShooting - 1].active)
    {
        float module;
        float sideX;
        float sideY;

        // Activate the interceptor
        interceptor[interceptorNumber].active = true;

        // Assign start position
        interceptor[interceptorNumber].origin = launcher[launcherShooting - 1].position;
        interceptor[interceptorNumber].position = interceptor[interceptorNumber].origin;
        interceptor[interceptorNumber].objective = GetMousePosition();

        // Calculate speed
        module = sqrt(pow(interceptor[interceptorNumber].objective.x - interceptor[interceptorNumber].origin.x, 2) +
            pow(interceptor[interceptorNumber].objective.y - interceptor[interceptorNumber].origin.y, 2));

        sideX = (interceptor[interceptorNumber].objective.x - interceptor[interceptorNumber].origin.x) * INTERCEPTOR_SPEED / module;
        sideY = (interceptor[interceptorNumber].objective.y - interceptor[interceptorNumber].origin.y) * INTERCEPTOR_SPEED / module;

        interceptor[interceptorNumber].speed = Vector2{ sideX, sideY };

        // Update
        interceptorNumber++;
        if (interceptorNumber == MAX_INTERCEPTORS) interceptorNumber = 0;


        fire1 = fire2 = fire3 = false;
    }
}
void MissleCommanderScene::UpdateIncomingFire()
{
    static int missileIndex = 0;

    // Launch missile
    if (framesCounter % MISSILE_LAUNCH_FRAMES == 0)
    {
        float module;
        float sideX;
        float sideY;

        // Activate the missile
        missile[missileIndex].active = true;

        // Assign start position
        missile[missileIndex].origin = Vector2{ (float)GetRandomValue(20, GetScreenWidth() - 20), -10 };
        missile[missileIndex].position = missile[missileIndex].origin;
        missile[missileIndex].objective = Vector2{ (float)GetRandomValue(20, GetScreenWidth() - 20),(float)GetScreenHeight() + 10 };

        // Calculate speed
        module = sqrt(pow(missile[missileIndex].objective.x - missile[missileIndex].origin.x, 2) +
            pow(missile[missileIndex].objective.y - missile[missileIndex].origin.y, 2));

        sideX = (missile[missileIndex].objective.x - missile[missileIndex].origin.x) * MISSILE_SPEED / module;
        sideY = (missile[missileIndex].objective.y - missile[missileIndex].origin.y) * MISSILE_SPEED / module;

        missile[missileIndex].speed = Vector2{ sideX, sideY };

        // Update
        missileIndex++;
        if (missileIndex == MAX_MISSILES) missileIndex = 0;
    }
}
static int scene000 = RegisterScene("Game", "Missle Commander", MissleCommanderScene::Create);
