// GameDevTemplate.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "App.h"
int main()
{
    App::Start("Template Project", "Configs/window.json", NULL);
    App::Run();
    App::End();
}

