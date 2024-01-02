#include "Scene.h"
#include "RenderPipeline.h"
#include <imgui.h>

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
	Vector2 position;
	Vector2 size;
	int life;
} Player;

typedef struct Ball {
	Vector2 position;
	Vector2 speed;
	int radius;
	bool active;
} Ball;

typedef struct Brick {
	Vector2 position;
	bool active;
} Brick;

#define LINES_OF_BRICKS         5
#define BRICKS_PER_LINE        20
class PongGameScene : public Scene
{
public:
	int PLAYER_MAX_LIFE = 5;

	Player player = { 0 };
	Ball ball = { 0 };
	Brick brick[LINES_OF_BRICKS][BRICKS_PER_LINE] = { 0 };
	Vector2 brickSize = { 0 };
	bool gameOver = false;
    float playerSpeed = 20.0f;
    float ballSpeed = 10.0f;
public:
    PongGameScene() 
    {
        playerSpeed = GetScreenWidth() * 0.75;
        ballSpeed = GetScreenHeight() * 0.75;
        InitGame();
    }

    ~PongGameScene() {

    }

	void InitGame()
	{

		brickSize = Vector2{ (float)GetScreenWidth() / BRICKS_PER_LINE, 40 };

		float screenWidth = (float)GetScreenWidth();
		float screenHeight = (float)GetScreenHeight();
		// Initialize player
		player.position = Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() * 7 / 8 };
		player.size = Vector2{ (float)GetScreenWidth() / 10, 20 };
		player.life = PLAYER_MAX_LIFE;

		// Initialize ball
		ball.position = Vector2{ screenWidth / 2, screenHeight * 7 / 8 - 30 };
		ball.speed = Vector2{ 0, 0 };
		ball.radius = 7;
		ball.active = false;

		// Initialize bricks
		int initialDownPosition = 50;

		for (int i = 0; i < LINES_OF_BRICKS; i++)
		{
			for (int j = 0; j < BRICKS_PER_LINE; j++)
			{
				brick[i][j].position = Vector2{ j * brickSize.x + brickSize.x / 2, i * brickSize.y + initialDownPosition };
				brick[i][j].active = true;
			}
		}
	}
	virtual void Update(const float& dt)
	{

        if (!gameOver)
        {
            //if (IsKeyPressed('P')) pause = !pause;

            //use application pause here
            if (true)
            {
                // Player movement logic
                if (IsKeyDown(KEY_LEFT)) player.position.x -= playerSpeed * dt;
                if ((player.position.x - player.size.x / 2) <= 0) player.position.x = player.size.x / 2;
                if (IsKeyDown(KEY_RIGHT)) player.position.x += playerSpeed * dt;
                if ((player.position.x + player.size.x / 2) >= GetScreenWidth()) player.position.x = GetScreenWidth() - player.size.x / 2;

                // Ball launching logic
                if (!ball.active)
                {
                    if (IsKeyPressed(KEY_SPACE))
                    {
                        ball.active = true;
                        ball.speed = Vector2{ 0, ballSpeed };
                    }
                }

                // Ball movement logic
                if (ball.active)
                {
                    ball.position.x += ball.speed.x * dt;
                    ball.position.y += ball.speed.y * dt;
                }
                else
                {
                    ball.position = Vector2{ player.position.x,(float) GetScreenHeight() * 7 / 8 - 30 };
                }

                // Collision logic: ball vs walls
                if (((ball.position.x + ball.radius) >= GetScreenWidth()) || ((ball.position.x - ball.radius) <= 0)) ball.speed.x *= -1;
                if ((ball.position.y - ball.radius) <= 0) ball.speed.y *= -1;
                if ((ball.position.y + ball.radius) >= GetScreenHeight())
                {
                    ball.speed = Vector2{ 0, 0 };
                    ball.active = false;

                    player.life--;
                }

                // Collision logic: ball vs player
                if (CheckCollisionCircleRec(ball.position, ball.radius,
                    Rectangle {
                    player.position.x - player.size.x / 2, player.position.y - player.size.y / 2, player.size.x, player.size.y
                }))
                {
                    if (ball.speed.y > 0)
                    {
                        ball.speed.y *= -1;
                        ball.speed.x = (ball.position.x - player.position.x) / (player.size.x / 2) * ballSpeed;
                    }
                }

                    // Collision logic: ball vs bricks
                    for (int i = 0; i < LINES_OF_BRICKS; i++)
                    {
                        for (int j = 0; j < BRICKS_PER_LINE; j++)
                        {
                            if (brick[i][j].active)
                            {
                                // Hit below
                                if (((ball.position.y - ball.radius) <= (brick[i][j].position.y + brickSize.y / 2)) &&
                                    ((ball.position.y - ball.radius) > (brick[i][j].position.y + brickSize.y / 2 + ball.speed.y)) &&
                                    ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) && (ball.speed.y < 0))
                                {
                                    brick[i][j].active = false;
                                    ball.speed.y *= -1;
                                }
                                // Hit above
                                else if (((ball.position.y + ball.radius) >= (brick[i][j].position.y - brickSize.y / 2)) &&
                                    ((ball.position.y + ball.radius) < (brick[i][j].position.y - brickSize.y / 2 + ball.speed.y)) &&
                                    ((fabs(ball.position.x - brick[i][j].position.x)) < (brickSize.x / 2 + ball.radius * 2 / 3)) && (ball.speed.y > 0))
                                {
                                    brick[i][j].active = false;
                                    ball.speed.y *= -1;
                                }
                                // Hit left
                                else if (((ball.position.x + ball.radius) >= (brick[i][j].position.x - brickSize.x / 2)) &&
                                    ((ball.position.x + ball.radius) < (brick[i][j].position.x - brickSize.x / 2 + ball.speed.x)) &&
                                    ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) && (ball.speed.x > 0))
                                {
                                    brick[i][j].active = false;
                                    ball.speed.x *= -1;
                                }
                                // Hit right
                                else if (((ball.position.x - ball.radius) <= (brick[i][j].position.x + brickSize.x / 2)) &&
                                    ((ball.position.x - ball.radius) > (brick[i][j].position.x + brickSize.x / 2 + ball.speed.x)) &&
                                    ((fabs(ball.position.y - brick[i][j].position.y)) < (brickSize.y / 2 + ball.radius * 2 / 3)) && (ball.speed.x < 0))
                                {
                                    brick[i][j].active = false;
                                    ball.speed.x *= -1;
                                }
                            }
                        }
                    }

                    // Game over logic
                    if (player.life <= 0) gameOver = true;
                    else
                    {
                        gameOver = true;

                        for (int i = 0; i < LINES_OF_BRICKS; i++)
                        {
                            for (int j = 0; j < BRICKS_PER_LINE; j++)
                            {
                                if (brick[i][j].active) gameOver = false;
                            }
                        }
                    }
            }
        }
        else
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                InitGame();
                gameOver = false;
            }
        }
	}
    virtual void FixedUpdate(const float& timeStep) {}
	virtual void Draw() {

        if (!gameOver)
        {
            // Draw player bar
            DrawRectangle(player.position.x - player.size.x / 2, player.position.y - player.size.y / 2, player.size.x, player.size.y, BLACK);

            // Draw player lives
            for (int i = 0; i < player.life; i++) DrawRectangle(20 + 40 * i, GetScreenHeight() - 30, 35, 10, LIGHTGRAY);

            // Draw ball
            DrawCircleV(ball.position, ball.radius, MAROON);

            // Draw bricks
            for (int i = 0; i < LINES_OF_BRICKS; i++)
            {
                for (int j = 0; j < BRICKS_PER_LINE; j++)
                {
                    if (brick[i][j].active)
                    {
                        if ((i + j) % 2 == 0) DrawRectangle(brick[i][j].position.x - brickSize.x / 2, brick[i][j].position.y - brickSize.y / 2, brickSize.x, brickSize.y, GRAY);
                        else DrawRectangle(brick[i][j].position.x - brickSize.x / 2, brick[i][j].position.y - brickSize.y / 2, brickSize.x, brickSize.y, DARKGRAY);
                    }
                }
            }

            if (false) DrawText("GAME PAUSED", GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2, GetScreenHeight() / 2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);

	}
	virtual void Debug() {
        ImGui::SliderFloat("player speed", &playerSpeed, 0, GetScreenWidth());
        ImGui::SliderFloat("ball speed", &ballSpeed, 0, GetScreenHeight());
	};
	virtual void PollEvents() {
	
	};

    static Scene* Create() { return new PongGameScene(); }
};

static int scene000 = RegisterScene("Game", "Pong", PongGameScene::Create);
