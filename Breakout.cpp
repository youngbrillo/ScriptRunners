#include "Breakout.h"

//player ///////////////////////////////////////////////////////////////////////////
//       ///////////////////////////////////////////////////////////////////////////
//       ///////////////////////////////////////////////////////////////////////////
void Breakout::Player::Update(const float& dt)
{
    position.x += direction * speed * dt;
    if ((position.x - size.x / 2) <= 0)
        position.x = size.x / 2;
    else if ((position.x + size.x / 2) >= GetScreenWidth())
        position.x = GetScreenWidth() - size.x / 2;

}

void Breakout::Player::PollInputs()
{
    direction = 0;
    if (IsKeyDown(KEY_LEFT)) direction--;
    if (IsKeyDown(KEY_RIGHT)) direction++;
}

// ball ///////////////////////////////////////////////////////////////////////////
//      ///////////////////////////////////////////////////////////////////////////
//      ///////////////////////////////////////////////////////////////////////////
void Breakout::Ball::Update(const float& dt, float& starting_x)
{
    if (active)
    {
        position.x += speed.x * dt;
        position.y += speed.y * dt;
    }
    else
    {
        position = Vector2{ starting_x, (float)GetScreenHeight() * 7 / 8 - 30 };
    }

}
void Breakout::Ball::PollInputs()
{
    // Ball launching logic
    if (!active)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            active = true;
            speed = Vector2{ 0, launchMagnitude };
        }
    }
}
// BRICK ///////////////////////////////////////////////////////////////////////////
//       ///////////////////////////////////////////////////////////////////////////
//       ///////////////////////////////////////////////////////////////////////////
void Breakout::Brick::Draw()
{
    DrawRectangleRec(rec(), color);
}
// BREAK OUT ///////////////////////////////////////////////////////////////////////////
//           ///////////////////////////////////////////////////////////////////////////
//           ///////////////////////////////////////////////////////////////////////////
BreakoutScene::BreakoutScene() 
    : Scene()
    , max_lives(5)
    , player()
    , ball()
    , brickSize({ 0.0f, 0.0f })
    , gameOver(false)
    , LINES_OF_BRICKS(5)
    , BRICKS_PER_LINE(20)
{
    player.speed = GetScreenWidth() * 0.75;
    player.size = Vector2{ (float)GetScreenWidth() / 10, 20 };

    ball.launchMagnitude = GetScreenWidth() * 0.75;
    Init();
}

BreakoutScene::~BreakoutScene()
{
}

void BreakoutScene::Init()
{
    bricks.clear();
    brickSize = Vector2{ (float)GetScreenWidth() / BRICKS_PER_LINE, 40 };
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();
    // Initialize player
    player.position = Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() * 7 / 8 };
    player.size.x = player.size.x < (float)GetScreenWidth() / 10 ? (float)GetScreenWidth() / 10 : player.size.x;

    player.life = max_lives;

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
            Breakout::Brick brick;
            brick.position = Vector2{ j * brickSize.x + brickSize.x / 2, i * brickSize.y + initialDownPosition };
            brick.active = true;
            brick.Size = brickSize;
            if ((i + j) % 2 == 0)
                brick.color = LIGHTGRAY;
            else
                brick.color = DARKGRAY;
            bricks.push_back(brick);
        }
    }
}


void BreakoutScene::Update(const float& dt)
{
    ActiveUpdate(dt);
}

void BreakoutScene::ActiveUpdate(const float& dt)
{
    if (gameOver) return;
    player.Update(dt);
    ball.Update(dt, player.position.x);
    handleCollision_ballvWalls();
    handleCollision_ballvPlayer();
    handleCollision_ballvBrick(dt);
    handleGameOver();
}

void BreakoutScene::handleCollision_ballvWalls()
{
    if (((ball.position.x + ball.radius) >= GetScreenWidth()) || ((ball.position.x - ball.radius) <= 0)) ball.speed.x *= -1;
    if ((ball.position.y - ball.radius) <= 0) ball.speed.y *= -1;
    if ((ball.position.y + ball.radius) >= GetScreenHeight())
    {
        ball.speed = Vector2{ 0, 0 };
        ball.active = false;

        player.life--;
    }
}
void BreakoutScene::handleCollision_ballvPlayer()
{
    // Collision logic: ball vs player
    if (CheckCollisionCircleRec(ball.position, ball.radius,
        Rectangle{
        player.position.x - player.size.x / 2, player.position.y - player.size.y / 2, player.size.x, player.size.y
        }))
    {
        if (ball.speed.y > 0)
        {
            ball.speed.y *= -1;
            ball.speed.x = (ball.position.x - player.position.x) / (player.size.x / 2) * ball.launchMagnitude;
        }
    }
}

void BreakoutScene::handleCollision_ballvBrick(const float& dt)
{
    // Collision logic: ball vs bricks
    for (auto&& brick : bricks)
    {
        if (brick.active)
        {
            if (Breakout::CheckCollision(ball, brick, ball.speed, dt))
            {
                brick.active = false;
                break;
            }
        }
    }
}

void BreakoutScene::handleGameOver()
{
    // Game over logic
    if (player.life <= 0) gameOver = true;
    else
    {
        gameOver = true;
        for (auto&& brick : bricks)
        {
            if (brick.active) gameOver = false;
        }
    }
}


void BreakoutScene::Draw()
{
    if (!gameOver)
    {
        // Draw player bar
        DrawRectangle(player.position.x - player.size.x / 2, player.position.y - player.size.y / 2, player.size.x, player.size.y, BLACK);

        // Draw player lives
        for (int i = 0; i < player.life; i++) DrawRectangle(20 + 40 * i, GetScreenHeight() - 30, 35, 10, LIGHTGRAY);

        // Draw ball
        DrawCircleV(ball.position, ball.radius, MAROON);

        // Draw bricks
        int index = 0;
        for (auto&& brick : bricks)
        {
            if (brick.active)
            {
                brick.Draw();
            }
            index++;
        }

        if (false) DrawText("GAME PAUSED", GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2, GetScreenHeight() / 2 - 40, 40, GRAY);
    }
    else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, GetScreenHeight() / 2 - 50, 20, GRAY);

}

void BreakoutScene::Debug()
{
    ImGui::SliderFloat("player speed", &player.speed, 0, GetScreenWidth());
    ImGui::SliderFloat("player size x", &player.size.x, 0, GetScreenWidth());
    ImGui::SliderFloat("player size y", &player.size.y, 0, GetScreenHeight()/3.0f);
    ImGui::SliderFloat("ball speed", &ball.launchMagnitude, 0, GetScreenHeight());
}


//get inputs 
void BreakoutScene::PollEvents()
{
    player.PollInputs();
    ball.PollInputs();

    if (gameOver)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            Init();
            gameOver = false;
        }
    }
}


static int scene000 = RegisterScene("Game", "Breakout", BreakoutScene::Create);

bool Breakout::CheckCollision(const Breakout::Ball& ball, const Breakout::Brick& brick, Vector2& direction, const float& deltaTime)
{
    
    bool result = false;

    bool contact = CheckCollisionCircleRec(ball.position, ball.radius, brick.rec());
    if (!contact) return result;

    float ballTop   = ball.position.y - ball.radius;
    float ballBottom= ball.position.y + ball.radius;
    float ballLeft  = ball.position.x - ball.radius;
    float ballRight = ball.position.x + ball.radius;

    float brickTop      = brick.position.y - brick.Size.y / 2;
    float brickBottom   = brick.position.y + brick.Size.y / 2;
    float brickLeft     = brick.position.x - brick.Size.x / 2;
    float brickRight    = brick.position.x + brick.Size.x / 2;


    bool hit_below = ((ballTop <= brickBottom) && (ballTop > brickBottom + ball.speed.y * deltaTime));
    bool hit_above = ballBottom <= brickTop && (ballBottom < (brickTop + ball.speed.y * deltaTime));
    bool hit_left  = ballRight >= brickLeft && (ballRight < brickLeft + ball.speed.x * deltaTime);
    bool hit_right = ballLeft <= ballRight && (ballLeft > brickLeft + ball.speed.x * deltaTime);

    bool isFalling = ball.speed.y > 0;
    bool isTravelingRight = ball.speed.x > 0;

    result = contact;
    if (hit_below && !isFalling)
        direction.y *= -1;
    else if (hit_above && isFalling)
        direction.y *= -1;
    else if (hit_left && isTravelingRight)
        direction.x *= -1;
    else if (hit_right && !isTravelingRight)
        direction.x *= -1;

    return result;
}
