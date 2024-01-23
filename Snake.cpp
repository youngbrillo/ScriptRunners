#include "Scene.h"
#include "RenderPipeline.h"
#include "App.h"

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH   256
#define SQUARE_SIZE     31

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

namespace SNAKE
{
	struct Snake {
		Vector2 position;
		Vector2 size;
		Vector2 speed;
		Color color;
	};

	struct Food {
		Vector2 position;
		Vector2 size;
		bool active;
		Color color;
	};
}
class SnakeScene : public Scene
{
public:
	int framesCounter = 0;
	bool gameOver = true;

	SNAKE::Food fruit = { 0 };
	SNAKE::Snake snake[SNAKE_LENGTH] = { 0 };
	Vector2 snakePosition[SNAKE_LENGTH] = { 0 };
	bool allowMove = false;
	Vector2 offset = { 0 };
	int counterTail = 0;

public:
	SnakeScene();
	~SnakeScene();

	void Init();
	virtual void Update(const float& dt);
    virtual void FixedUpdate(const float& timeStep);
    virtual void Run(const float& delta);
	virtual void Draw();
	virtual void Debug()
	{

	}
	virtual void PollEvents() override;
	static Scene* Create(const char* path) { return new SnakeScene(); }

};



SnakeScene::SnakeScene()
    : Scene()
{
    Init();
}

SnakeScene::~SnakeScene()
{
}

void SnakeScene::Init()
{

	framesCounter = 0;
	gameOver = false;

	counterTail = 1;
	allowMove = false;
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	offset.x = screenWidth % SQUARE_SIZE;
	offset.y = screenHeight % SQUARE_SIZE;

	for (int i = 0; i < SNAKE_LENGTH; i++)
	{
		snake[i].position = Vector2{ offset.x / 2, offset.y / 2 };
		snake[i].size = Vector2{ SQUARE_SIZE, SQUARE_SIZE };
		snake[i].speed = Vector2{ SQUARE_SIZE, 0 };

		if (i == 0) snake[i].color = DARKBLUE;
		else snake[i].color = BLUE;
	}

	for (int i = 0; i < SNAKE_LENGTH; i++)
	{
		snakePosition[i] = Vector2{ 0.0f, 0.0f };
	}

	fruit.size = Vector2{ SQUARE_SIZE, SQUARE_SIZE };
	fruit.color = SKYBLUE;
	fruit.active = false;
}

void SnakeScene::Update(const float& dt)
{

}

void SnakeScene::FixedUpdate(const float& timeStep)
{
    this->Run(timeStep);
}

void SnakeScene::Run(const float& delta)
{
    if (gameOver) return;

    // Snake movement
    for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

    if ((framesCounter % 5) == 0)
    {
        for (int i = 0; i < counterTail; i++)
        {
            if (i == 0)
            {
                snake[0].position.x += snake[0].speed.x;
                snake[0].position.y += snake[0].speed.y;
                allowMove = true;
            }
            else snake[i].position = snakePosition[i - 1];
        }
    }

    // Wall behaviour
    if (((snake[0].position.x) > (GetScreenWidth() - offset.x)) ||
        ((snake[0].position.y) > (GetScreenHeight() - offset.y)) ||
        (snake[0].position.x < 0) || (snake[0].position.y < 0))
    {
        gameOver = true;
    }

    // Collision with yourself
    for (int i = 1; i < counterTail; i++)
    {
        if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) gameOver = true;
    }

    // Fruit position calculation
    if (!fruit.active)
    {
        fruit.active = true;
        fruit.position = Vector2{ GetRandomValue(0, (GetScreenWidth() / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (GetScreenHeight() / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };

        for (int i = 0; i < counterTail; i++)
        {
            while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
            {
                fruit.position = Vector2{ GetRandomValue(0, (GetScreenWidth() / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.x / 2, GetRandomValue(0, (GetScreenHeight() / SQUARE_SIZE) - 1) * SQUARE_SIZE + offset.y / 2 };
                i = 0;
            }
        }
    }

    // Collision
    if ((snake[0].position.x < (fruit.position.x + fruit.size.x) && (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
        (snake[0].position.y < (fruit.position.y + fruit.size.y) && (snake[0].position.y + snake[0].size.y) > fruit.position.y))
    {
        snake[counterTail].position = snakePosition[counterTail - 1];
        counterTail += 1;
        fruit.active = false;
    }

    framesCounter++;
}

void SnakeScene::Draw()
{

    if (!gameOver)
    {
        // Draw grid lines
        for (int i = 0; i < GetScreenWidth() / SQUARE_SIZE + 1; i++)
        {
            DrawLineV(Vector2 { SQUARE_SIZE* i + offset.x / 2, offset.y / 2 }, Vector2 { SQUARE_SIZE* i + offset.x / 2, GetScreenHeight() - offset.y / 2 }, LIGHTGRAY);
        }

        for (int i = 0; i < GetScreenHeight() / SQUARE_SIZE + 1; i++)
        {
            DrawLineV(Vector2 { offset.x / 2, SQUARE_SIZE* i + offset.y / 2 }, Vector2 { GetScreenWidth() - offset.x / 2, SQUARE_SIZE* i + offset.y / 2 }, LIGHTGRAY);
        }

        // Draw snake
        for (int i = 0; i < counterTail; i++) DrawRectangleV(snake[i].position, snake[i].size, snake[i].color);

        // Draw fruit to pick
        DrawRectangleV(fruit.position, fruit.size, fruit.color);

        if (App::GetState() != AppState_Play) DrawText("GAME PAUSED", GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2, GetScreenHeight() / 2 - 40, 40, GRAY);
    }
    else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);

}

void SnakeScene::PollEvents()
{
    if (!gameOver)
    {
        // Player control
        if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
        {
            snake[0].speed = Vector2{ SQUARE_SIZE, 0 };
            allowMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
        {
            snake[0].speed = Vector2{ -SQUARE_SIZE, 0 };
            allowMove = false;
        }
        if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
        {
            snake[0].speed = Vector2{ 0, -SQUARE_SIZE };
            allowMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
        {
            snake[0].speed = Vector2{ 0, SQUARE_SIZE };
            allowMove = false;
        }
    }
    if (gameOver && IsKeyPressed(KEY_ENTER))
    {
        Init();
    }
}


static int scene000 = RegisterScene("Game", "Snake", SnakeScene::Create);
