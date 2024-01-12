#pragma once
#include "RenderPipeline.h"
#include <string>

struct FrameData
{
	Rectangle frame = Rectangle{ 0, 0, 64, 64 };
	float duration = 0.15f;
	std::string name = "no name";
};
