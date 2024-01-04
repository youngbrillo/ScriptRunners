#include "AppSettings.h"
#include "JsonHandler.h"


void AppSettings::BeginFrame()
{
	if (inStep) { state = AppState_Play; }
}
void AppSettings::EndFrame(const float& dt)
{
	if (inStep && state == AppState_Play && stepProg < stepTime) {
		stepProg += dt;
		if (stepProg >= stepTime)
		{
			stepProg = 0.0f;
			inStep = false;
			state = AppState_FullPause;
		}
	}
}


int AppSettings::windowFlags()
{
	int flags = 0;
	if (win_resizeable) flags = flags | FLAG_WINDOW_RESIZABLE;
	if (win_undecorated) flags = flags | FLAG_WINDOW_UNDECORATED;
	if (win_transparent) flags = flags | FLAG_WINDOW_TRANSPARENT;
	return flags;
}

void AppSettings::Poll()
{
	if (IsKeyReleased(step_key) && state != AppState_Play)
	{
		this->inStep = true;
	}

	if (IsKeyReleased(pause_key))
	{
		if (state == AppState_Play ? AppState_FullPause : AppState_Play);
	}

	if (IsKeyReleased(exitKey))
	{
		quit = true;
	}

	if (IsKeyReleased(KEY_R) && IsKeyDown(ctrl_key))
	{
		restart = true;
	}
}
static const char* state_names[AppState_::AppState_PartialPause + 1] = { "Play",  "Full Pause", "Fixed Pause" };

void AppSettings::Debug()
{
	ImGui::Checkbox("quit", &quit);
	ImGui::Checkbox("restart", &restart);
	ImGui::Checkbox("autoSave", &autoSave);

	//DRAW FRAMERATE
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//MODIFY THE WINDOW COLOR
	Vector4 winColor = ColorNormalize(window_color);
	if (ImGui::ColorEdit4("Window Clear Color", &winColor.x))
	{
		window_color = ColorFromNormalized(winColor);
	}
	//MODIFY WINDOW SETTINGS TODO: MOVE TO A MENU ITEM IN THE INSPECTOR
	if (ImGui::TreeNode("configs"))
	{
		ImGui::Checkbox("Auto Save", &autoSave);
		bool fs0 = ImGui::Checkbox("resizeable", &win_resizeable);
		bool fs1 = ImGui::Checkbox("undecorated", &win_undecorated);
		bool fs2 = ImGui::Checkbox("transparent", &win_transparent);
		if (fs0 || fs1 || fs2)
		{
			ClearWindowState(windowFlags());
			SetWindowState(windowFlags());
		}

		ImGui::TreePop();
	}
	if (ImGui::Button("Save Settings"))
	{
		this->SaveData();
	}

	ImVec2 button_sz = ImVec2(-1, 0);
	ImVec2 button_sz_2 = ImVec2(0, 0);

	ImGui::SliderFloat("step limit", &stepTime, 0.01f, 0.25f);
	int elem = state;

	if (ImGui::SliderInt("State", &elem, AppState_Play, AppState_PartialPause, state_names[state]))
	{
		state = (AppState_)elem;
	}
	bool isPaused = state != AppState_Play;
	const char* pause_status = isPaused ? "Play (P)" : "Pause (P)";
	if (ImGui::Button(pause_status, button_sz_2))
	{
		state = isPaused ? AppState_Play : AppState_FullPause;
	}
	if (isPaused)
	{
		ImGui::SameLine();
		if (ImGui::Button("Step (])", button_sz_2))
		{
			inStep = true;
		}
	}
	if (ImGui::Button("Restart (LCTRL + R)", button_sz))
	{
		restart = true;
	}

	if (ImGui::Button("Quit (Q)", button_sz))
	{
		quit = true;
	}



}

void AppSettings::LoadData()
{
	jsonObjects obj;
	JSONParse::ParseJSONFile(obj, configPath.c_str());
	Vector4 bgColor = ColorNormalize(window_color);
	obj.Getboolean("autoSave", autoSave)
		.GetString("title", wintitle)
		.GetInteger("scene_index", lastScene_id)
		.GetString("scene_name", last_scene_name)
		.GetString("scene_category", last_scene_cat)
		.GetInteger("width", window_width)
		.GetInteger("height", window_height)
		.Getboolean("resizeable", win_resizeable)
		.Getboolean("undecorated", win_undecorated)
		.Getboolean("transparent", win_transparent)
		.find("bgColor")
			.GetNumber("r", bgColor.x)
			.GetNumber("g", bgColor.y)
			.GetNumber("b", bgColor.z)
			.GetNumber("a", bgColor.w);

	window_color = ColorFromNormalized(bgColor);
}

void AppSettings::SaveData()
{
	jsonObjects obj;

	Vector4 bgColor = ColorNormalize(window_color);
	obj
		.SetBooleanAt("autoSave", autoSave)
		//.SetStringAt("title", wintitle)
		.SetIntegerAt("scene_index", lastScene_id)
		.SetStringAt("scene_name", last_scene_name)
		.SetStringAt("scene_category", last_scene_cat)
		.SetIntegerAt("width", window_width)
		.SetIntegerAt("height", window_height)
		.SetBooleanAt("resizeable", win_resizeable)
		.SetBooleanAt("undecorated", win_undecorated)
		.SetBooleanAt("transparent", win_transparent)
		.findOrCreate("bgColor")
		.SetNumberAtAndAdd("r", bgColor.x)
		.SetNumberAtAndAdd("g", bgColor.y)
		.SetNumberAtAndAdd("b", bgColor.z)
		.SetNumberAtAndAdd("a", bgColor.w);

	obj.SaveToFile(configPath.c_str());

}

void AppSettings::AutoSave()
{
}

