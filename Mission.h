#pragma once
#include <string>
namespace SR
{
	class Mission
	{
	public:
		Mission(std::string n, float timeLimit = 0.0f)
			: name(n)
			, active(false)
			, completed(false)
			, started(false)
			, assigned(false)
			, timeLimit(timeLimit)
			, elapsedTime(0.0f)
		{
		}
		~Mission()
		{

		}

		virtual void Evaluate(const float& delta) = 0;
		virtual void Draw() = 0;
		virtual void DrawUI() = 0;
	public:
		std::string name;
		bool completed;		// has the mission been completed?
		bool started;		// has the mission been started?
		bool assigned;		// has the mission been given out?
		bool active;		// is the mission currently active ?
		float timeLimit;	// in seconds (a timeLimit <= 0.0 is ignored)
		float elapsedTime;
	};
}

