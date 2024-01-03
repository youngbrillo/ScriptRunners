#include "Scene.h"
#include "RenderPipeline.h"
#include <imgui.h>
#include <vector>

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
namespace Breakout
{
	struct Player {
		Vector2 position;
		Vector2 size;
		int life;
		float speed = 100.0f;

		int direction = 0;
		void Update(const float& dt);
		void PollInputs();
	};

	struct Ball {
		Vector2 position;
		Vector2 speed;
		int radius;
		bool active;
		float launchMagnitude = 100.0f;
		void Update(const float& dt, float& starting_x);
		void PollInputs();
		
	};

	struct Brick {
		Vector2 position;
		Vector2 Size;
		bool active;
		Color color = PINK;
		Rectangle rec() const{return Rectangle{position.x - Size.x / 2,position.y - Size.y / 2,Size.x, Size.y };}
		void Draw();
		void onDestruction(){}
	};
	bool CheckCollision(const Breakout::Ball& ball, const Breakout::Brick& brick, Vector2& direction, const float& dt);

}

class BreakoutScene : public Scene
{
public:
	BreakoutScene();
	~BreakoutScene();

	void Init();
	virtual void Update(const float& dt);
	void ActiveUpdate(const float& dt);
	void handleCollision_ballvWalls();
	void handleCollision_ballvPlayer();
	void handleCollision_ballvBrick(const float& dt);
	void handleGameOver();
	virtual void FixedUpdate(const float& timeStep) {}
	virtual void Draw();
	virtual void Debug();
	virtual void PollEvents() override;

	static Scene* Create() { return new BreakoutScene(); }
	
public:
	int max_lives = 5;
	int LINES_OF_BRICKS = 5, BRICKS_PER_LINE = 20;
	Breakout::Player player = { 0 };
	Breakout::Ball ball = { 0 };
	std::vector<Breakout::Brick> bricks;
	Vector2 brickSize = { 0 };
	bool gameOver = false;
};
