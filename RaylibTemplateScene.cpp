#pragma once
#include "Scene.h"
#include <raylib/raylib.h>
#include "RenderPipeline.h"

class Raylib_intro_scene : public Scene
{
private:
	int logoPositionX;
	int logoPositionY;

	float framesCounter = 0.0f;
	float state0Speed = 0.5f;
	float state2Speed = 0.16f;
	float state4Speed = 0.5f;
	bool state4autoReset = false;
	int lettersCount = 0;

	int topSideRecWidth = 16;
	int leftSideRecHeight = 16;

	int bottomSideRecWidth = 16;
	int rightSideRecHeight = 16;

	int state = 0;                  // Tracking animation states (State Machine)
	float alpha = 1.0f;             // Useful for fading
public:
	Raylib_intro_scene()
	{
		logoPositionX = GetScreenWidth() / 2 - 128;
		logoPositionY = GetScreenHeight() / 2 - 128;
	}
	~Raylib_intro_scene()
	{

	}


	virtual void Update(const float& dt)
	{
		if (state == 0)                 // State 0: Small box blinking
		{
			framesCounter += dt;

			if (framesCounter >= state0Speed)
			{
				state = 1;
				framesCounter = 0.0f;      // Reset counter... will be used later...
			}
		}
		else if (state == 1)            // State 1: Top and left bars growing
		{
			topSideRecWidth += 4;
			leftSideRecHeight += 4;

			if (topSideRecWidth == 256) state = 2;
		}
		else if (state == 2)            // State 2: Bottom and right bars growing
		{
			bottomSideRecWidth += 4;
			rightSideRecHeight += 4;

			if (bottomSideRecWidth == 256) state = 3;
		}
		else if (state == 3)            // State 3: Letters appearing (one by one)
		{
			framesCounter+=dt;

			if (framesCounter >= state2Speed)       // Every 12 frames, one more letter!
			{
				lettersCount++;
				framesCounter = 0.0f;
			}

			if (lettersCount >= 10)     // When all letters have appeared, just fade out everything
			{
				alpha -= 0.02f;

				if (alpha <= 0.0f)
				{
					alpha = 0.0f;
					state = 4;
				}
			}
		}
		else if (state == 4)            // State 4: Reset and Replay
		{
			framesCounter += dt;
			if (IsKeyPressed(KEY_R) || (framesCounter >= state4Speed&& state4autoReset))
			{
				framesCounter = 0.0f;
				lettersCount = 0;

				topSideRecWidth = 16;
				leftSideRecHeight = 16;

				bottomSideRecWidth = 16;
				rightSideRecHeight = 16;

				alpha = 1.0f;
				state = 0;          // Return to State 0
			}
		}
	}

	virtual void FixedUpdate(const float& timeStep) {}
	virtual void Draw()
	{

		if (state == 0)
		{
			DrawRectangle(logoPositionX, logoPositionY, 16, 16, BLACK);
		}
		else if (state == 1)
		{
			DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
			DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);
		}
		else if (state == 2)
		{
			DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
			DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);

			DrawRectangle(logoPositionX + 240, logoPositionY, 16, rightSideRecHeight, BLACK);
			DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, BLACK);
		}
		else if (state == 3)
		{
			DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, Fade(BLACK, alpha));
			DrawRectangle(logoPositionX, logoPositionY + 16, 16, leftSideRecHeight - 32, Fade(BLACK, alpha));

			DrawRectangle(logoPositionX + 240, logoPositionY + 16, 16, rightSideRecHeight - 32, Fade(BLACK, alpha));
			DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, Fade(BLACK, alpha));

			DrawRectangle(GetScreenWidth() / 2 - 112, GetScreenHeight() / 2 - 112, 224, 224, Fade(RAYWHITE, alpha));

			DrawText(TextSubtext("raylib", 0, lettersCount), GetScreenWidth() / 2 - 44, GetScreenHeight() / 2 + 48, 50, Fade(BLACK, alpha));
		}
		else if (state == 4)
		{
			//DrawText("[R] REPLAY", 340, 200, 20, GRAY);
		}
	}
	virtual void Debug() {
	
	};

	static Scene* Register(const char* path) { return new Raylib_intro_scene(); }

};


class Raylib_custom_frameControl : public Scene
{
private:
	// Custom timming variables
	double previousTime = 0.0f;   // Previous time measure
	double currentTime = 0.0;           // Current time measure
	double updateDrawTime = 0.0;        // Update + Draw time
	double waitTime = 0.0;              // Wait time (if target fps required)
	float deltaTime = 0.0f;             // Frame time (Update + Draw + Wait time)

	float timeCounter = 0.0f;           // Accumulative time counter (seconds)
	float position = 0.0f;              // Circle position
	bool pause = false;                 // Pause control flag

	int targetFPS = 60;                 // Our initial target fps
	//--------------------------------------------------------------------------------------

public:
	Raylib_custom_frameControl()
	{
		previousTime = GetTime();
	}
	~Raylib_custom_frameControl()
	{

	}

	virtual void Draw()
	{
#ifndef USE_RAYLIB
		BeginDrawing();            
		ClearBackground(RAYWHITE);
#endif 

		for (int i = 0; i < GetScreenWidth() / 200; i++) DrawRectangle(200 * i, 0, 1, GetScreenHeight(), SKYBLUE);

		DrawCircle((int)position, GetScreenHeight() / 2 - 25, 50, RED);

		DrawText(TextFormat("%03.0f ms", timeCounter * 1000.0f), (int)position - 40, GetScreenHeight() / 2 - 100, 20, MAROON);
		DrawText(TextFormat("PosX: %03.0f", position), (int)position - 50, GetScreenHeight() / 2 + 40, 20, BLACK);

		DrawText("Circle is moving at a constant 200 pixels/sec,\nindependently of the frame rate.", 10, 25, 20, DARKGRAY);
		DrawText("PRESS SPACE to PAUSE MOVEMENT", 10, GetScreenHeight() - 60, 20, GRAY);
		DrawText("PRESS UP | DOWN to CHANGE TARGET FPS", 10, GetScreenHeight() - 30, 20, GRAY);
		DrawText(TextFormat("TARGET FPS: %i", targetFPS), GetScreenWidth() - 220, 25, 20, LIME);
		DrawText(TextFormat("CURRENT FPS: %i", (int)(1.0f / deltaTime)), GetScreenWidth() - 220, 25+30, 20, GREEN);
		//SwapScreenBuffer();         // Flip the back buffer to screen (front buffer)

#ifndef USE_RAYLIB
		EndDrawing();
#endif 
	}
	virtual void Update(const float& dt)
	{
		// Update
		//----------------------------------------------------------------------------------
		//PollInputEvents();              // Poll input events (SUPPORT_CUSTOM_FRAME_CONTROL)

		if (IsKeyPressed(KEY_SPACE)) pause = !pause;

		if (IsKeyPressed(KEY_UP)) targetFPS += 20;
		else if (IsKeyPressed(KEY_DOWN)) targetFPS -= 20;

		if (targetFPS < 0) targetFPS = 0;

		if (!pause)
		{
			position += 200 * deltaTime;  // We move at 200 pixels per second
			if (position >= GetScreenWidth()) position = 0;
			timeCounter += deltaTime;   // We count time (seconds)
		}


		currentTime = GetTime();
		updateDrawTime = currentTime - previousTime;

		if (targetFPS > 0)          // We want a fixed frame rate
		{
			waitTime = (1.0f / (float)targetFPS) - updateDrawTime;
			if (waitTime > 0.0)
			{
				WaitTime((float)waitTime);
				currentTime = GetTime();
				deltaTime = (float)(currentTime - previousTime);
			}
		}
		else deltaTime = (float)updateDrawTime;    // Framerate could be variable

		previousTime = currentTime;
	}
	virtual void FixedUpdate(const float& timeStep)
	{

	}
	virtual void Debug() override
	{

	}

	static Scene* Register(const char* path) { return new Raylib_custom_frameControl(); }
};






class Raylib_core_world_scene : public Scene
{
private:
	Camera camera = { 0 };        // Camera projection type

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };
	Vector2 cubeScreenPosition = { 0.0f, 0.0f };
public:
	Raylib_core_world_scene()
	{

		camera.position = Vector3{ 10.0f, 10.0f, 10.0f }; // Camera position
		camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
		camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
		camera.fovy = 45.0f;                                // Camera field-of-view Y
		camera.projection = CAMERA_PERSPECTIVE;
		DisableCursor();                    // Limit cursor to relative movement inside the window

	}
	~Raylib_core_world_scene()
	{
		EnableCursor();
	}

	
	virtual void Update(const float& dt)
	{
		UpdateCamera(&camera, CAMERA_THIRD_PERSON);
		//RAYLIB::UpdateCameraWithTimeDelta(&camera, CAMERA_THIRD_PERSON, dt);
		// Calculate cube screen space position (with a little offset to be in top)
		cubeScreenPosition = GetWorldToScreen(Vector3 { cubePosition.x, cubePosition.y + 2.5f, cubePosition.z }, camera);

	}

	virtual void FixedUpdate(const float& timeStep)
	{

	}

	virtual void Draw()
	{

#ifndef USE_RAYLIB
		BeginDrawing();
		ClearBackground(RAYWHITE);
#endif 

		BeginMode3D(camera);

		DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
		DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

		DrawGrid(10, 1.0f);

		EndMode3D();

		DrawText("Enemy: 100 / 100", (int)cubeScreenPosition.x - MeasureText("Enemy: 100/100", 20) / 2, (int)cubeScreenPosition.y, 20, BLACK);

		DrawText(TextFormat("Cube position in screen space coordinates: [%i, %i]", (int)cubeScreenPosition.x, (int)cubeScreenPosition.y), 30, 25, 20, LIME);
		DrawText("Text 2d should be always on top of the cube", 40, 50, 20, GRAY);
		DrawText("Press [~] to lock/unlock cursor", 40, 80, 20, GRAY);



#ifndef USE_RAYLIB
		EndDrawing();
#endif 
	}
	virtual void Debug() override
	{

	}

	virtual void PollEvents() override
	{
		if (IsKeyPressed( KEY_GRAVE)) 
		{
			IsCursorHidden() ? EnableCursor() : DisableCursor();
		}
	}
	static Scene* Register(const char* path) { return new Raylib_core_world_scene(); }

};







static int scene000 = RegisterScene("Template", "Raylib: Intro Scene", Raylib_intro_scene::Register);
static int scene001 = RegisterScene("Template", "Raylib: Custom Frame Control", Raylib_custom_frameControl::Register);
static int scene002 = RegisterScene("Template", "Raylib: Core World Scene", Raylib_core_world_scene::Register);



