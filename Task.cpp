#include "Task.h"
#include <LuaBridge/LuaBridge.h>
#include "Extentions2d.h"
#include <imgui.h>

Job::Task::Task()
	: title("Untitled"), description("Undescribed")
	, subject(NULL), destinationBody(NULL), originBody(NULL)
{
}

Job::Task::Task(std::string title, std::string desc)
	: title(title), description(desc)
	, subject(NULL), destinationBody(NULL), originBody(NULL)
{
}

Job::Task::~Task()
{
	subject = NULL;
	if (destinationBody != NULL)
		destinationBody->GetWorld()->DestroyBody(destinationBody); 
	if(originBody != NULL)
		originBody->GetWorld()->DestroyBody(originBody); 
	originBody = NULL;
	destinationBody = NULL;
}

int Job::Task::CreateOriginBody(lua_State* L)
{
	return 0;
}

int Job::Task::CreateDestinationBody(lua_State* L)
{
	return 0;
}

void Job::Task::DestroyBody(b2Body* b)
{
}

void Job::Task::Inspect()
{
	if (ImGui::TreeNode(TextFormat("Task: %s", this->title.c_str())))
	{
		this->inspect();
		ImGui::TreePop();
	}
}

void Job::Task::inspect()
{
	//ImGui::LabelText("Description", this->description.c_str());
	ImGui::Text("Description:\n\t%s", this->description.c_str());
	ImGui::Checkbox("started", &this->started);
	ImGui::Checkbox("succeded", &this->succeded);
	ImGui::Checkbox("failed", &this->failed);
}

void Job::Task::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Job")
		.beginClass<Job::Task>("Task")
		.addFunction("CreateOriginBody", &Job::Task::CreateOriginBody)
		.addFunction("CreateDestinationBody", &Job::Task::CreateDestinationBody)
		.addFunction("DestroyBody", &Job::Task::DestroyBody)
		.addData("title", &Job::Task::title)
		.addData("description", &Job::Task::description)
		.addData("subject", &Job::Task::subject)
		.addData("destinationBody", &Job::Task::destinationBody)
		.addData("originBody", &Job::Task::originBody)
		.endClass()
		.endNamespace();
}

Job::Delivery::Delivery() : Job::Task()
{
}

Job::Delivery::Delivery(std::string title, std::string desc)
	: Job::Task(title, desc)
{

}

Job::Delivery::~Delivery()
{
}

void Job::Delivery::BeginDelivery(float destination_x, float destination_y)
{
	
}

void Job::Delivery::Update(const float& dt)
{
}

void Job::Delivery::BeginContact(b2Contact* contact, ECS::Node2d* other)
{
}

void Job::Delivery::EndContact(b2Contact* contact, ECS::Node2d* other)
{
}

void Job::Delivery::Extend(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("Job")
			.deriveClass<Job::Delivery, Job::Task>("Delivery")
				.addFunction("BeginDelivery", &Job::Delivery::BeginDelivery)
				.addData("timeLimit", &Job::Delivery::timeLimit)
				.addData("startTime", &Job::Delivery::startTime)
				.addData("finishTime", &Job::Delivery::finishTime)
				.addData("hasTimeLimit", &Job::Delivery::hasTimeLimit)
			.endClass()
		.endNamespace();
}




