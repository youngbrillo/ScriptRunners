#pragma once
#include <string>
#include <lua.hpp>
//Game File class
namespace SR
{
	/// <summary>
	/// Class to save/load game file settings
	/// data persists accross scenes
	/// should be updated in a settings file, and saved to the same at regular intervals
	/// </summary>
	class GFile
	{
	public:
		class GFile() {}

	public:
		int reputation = 0;
		int money = 0;

	};

}