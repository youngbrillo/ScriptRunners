#pragma once
#include <string>
class IOHandler
{
public:
	static std::string LoadFromFile(const char* filename);
	static bool SaveToFile(const char* filename, const char* data);
};

