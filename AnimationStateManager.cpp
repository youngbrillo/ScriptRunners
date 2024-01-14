#include "AnimationStateManager.h"
#include "GlobalManager.h"
#include "TextureManager.h"

AnimationStateManager::AnimationStateManager()
	: Manager(
		"Animation Manager",
		"Configs/AnimationStateManager.json",
		"Scripts/Manager/AnimationStateManager.lua"
	)
{
}

AnimationStateManager::~AnimationStateManager()
{
}

void AnimationStateManager::init()
{
	Manager::init();
}

void AnimationStateManager::clear()
{
}

void AnimationStateManager::Update(const float& dt)
{
	if (!active) return;
}

void AnimationStateManager::CameraDraw(Camera2D& camera)
{
}

void AnimationStateManager::CanvasDraw()
{
	if (!active) return;
}

void AnimationStateManager::innerdebug()
{
	if (ImGui::TreeNode("State Machines"))
	{
		for (auto&& pair : animatorLookup)
		{
			stateMachines[pair.second].Inspect(pair.first.c_str());
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Resources"))
	{
		for (auto&& pair : animatorLookup)
		{
			resources[pair.second].Inspect(pair.first.c_str(), stateMachines[pair.second]);
		}
		ImGui::TreePop();
	}
}

void AnimationStateManager::Populate(unsigned int textureId, std::string textureName, std::string path)
{
	std::string ppath = path + "_animations.json";
	jsonObjects testonme;
	if (!JSONParse::ParseJSONFile(testonme, ppath.c_str())) return;


	this->animatorLookup[textureName] = textureId;
	this->reverse_animatorLookup[textureId] = textureName;
	this->stateMachines[textureId].Populate(textureId, path);
	aliasDB.insert(textureName);

	this->resources[textureId] = this->GenerateResource(textureId);
}

Animation::Resource AnimationStateManager::GenerateResource(unsigned int texureId)
{
	Animation::Resource k;
	k.keys = this->stateMachines[texureId].keys;
	k.texId = texureId; 
	k.currentState = this->stateMachines[texureId].defaultState;
	k.lastState = k.currentState;
	return k;
}

Animation::StateMachine& AnimationStateManager::GetAnimator(unsigned int texture_id)
{
	//TODO: Add Guards to avoid adding additional state machines
	return stateMachines[texture_id];
}

std::shared_ptr<AnimationStateManager> AnimationStateManager::Instance()
{
	return GlobalManager::Get_Manager<AnimationStateManager>();
}

Manager AnimationStateManager::Register()
{
	GlobalManager::Register_Manager<AnimationStateManager>();
	return AnimationStateManager();
}
static int GMInstance01 = RegisterManager("Animation Manager", "", "", AnimationStateManager::Register);