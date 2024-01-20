#pragma once
#include "RenderPipeline.h"
#include <string>


namespace ECS
{
	void DrawWrappedText(Font font, std::string text, Rectangle rec, float fontSize, float spacing, Color fontColor, Color backgroundColor);
}