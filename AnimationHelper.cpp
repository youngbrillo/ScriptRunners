#include "AnimationHelper.h"
#include "AnimationStateManager.h"
#include <imgui_string_wrap_extension.h>
#include "TextureManager.h"

//Animation State machine //////////////////////////////////////////////////////////////////////////////////////////////////

void Animation::StateMachine::Populate(unsigned int texture, std::string path)
{
	auto& sprite = TextureManager::Instance()->getSprite(texture);

	int i = 0;
	for (std::pair<std::string, std::vector<int>>&& pair : sprite.animations)
	{
		this->stateLookup[pair.first] = i;
		this->reverse_stateLookup[i] = pair.first;
		this->states[i] = sprite.getAnimationFrames(pair.first);
		i++;
	}

	transition_file_path = path + "_animations.json";
	JSONParse::ParseJSONFile(transition_data, transition_file_path.c_str());

	auto& kChild = transition_data.find("keys");

	int keyIndex = 0;
	for (auto& child : kChild.children)
	{
		std::string type = "", key = "";
		child.GetString("name", key).GetString("type", type);
		Animation::Event e;
		Animation::Value cond;
		if (strcmp(type.c_str(), "number") == 0)
		{
			e = Animation::Event::NUMBER;
			child.GetNumber("default", cond.numericValue);
		}
		else if (strcmp(type.c_str(), "boolean") == 0)
		{
			e = Animation::Event::BOOL;
			child.Getboolean("default", cond.boolvalue);
		}
		else if (strcmp(type.c_str(), "string") == 0)
		{
			e = Animation::Event::STRING;
			child.GetString("default", cond.stringValue);
		}

		Animation::Condition kp = 
			Animation::Condition{ e, Animation::Operator::EQUAL, key, cond };

		this->keys.push_back(kp);
		this->keyLookup[key] = keyIndex++;
	}
	//2. get the actual transition data from the 'transitions' field
	auto& tChild = transition_data.find("transitions");
	std::vector<Animation::Transition> allTransitions;

	for (auto& child : tChild.children)
	{
		Animation::Transition nTran;
		std::string from, to, key, opprand;
		Animation::Condition cond; //only one condition for now
		child.GetString("from", from).GetString("to", to).GetString("key", key).GetString("operator", opprand)
			.GetString("value", cond.target.stringValue)
			.GetNumber("value", cond.target.numericValue)
			.Getboolean("value", cond.target.boolvalue);
		cond.key = key;
		cond.type = this->keys[keyLookup[key]].type; //type should match the key
		cond.mOperator = Animation::ConvertOpperatorString(opprand.c_str());
		//add condition to new transitions' condition list
		nTran.conditions.push_back(cond);
		nTran.origin = this->stateLookup[from];
		nTran.destination = this->stateLookup[to];
		nTran.name = child.name;
		//add new transition
		allTransitions.push_back(nTran);
	}
	//3 take the transitions we have generated, and dole them out to the AnimationResource's stateTrnasitions map
	for (auto& transition : allTransitions)
	{
		stateTransitions[transition.origin].push_back(transition);
	}

	std::string defaultStateName = "";
	transition_data.GetString("defaultState", defaultStateName);
	if(stateLookup.find(defaultStateName) != stateLookup.end())
		this->defaultState = stateLookup[defaultStateName];
}
		//GETTERS ////////////////////////////////////////////////////////////////////////////
	   //////////////////////////////////////////////////////////////////////////////////////
std::vector<Animation::Transition> Animation::StateMachine::GetTransitions(int state) const
{
	std::vector<Animation::Transition> val = std::vector<Animation::Transition>();
	if (this->stateTransitions.find(state) != stateTransitions.end())
	{
		val = stateTransitions.at(state);
	}
	return val;
}

int Animation::StateMachine::GetState(std::string n) const
{
	int r = -1;
	if (stateLookup.find(n) != stateLookup.end())
	{
		r = stateLookup.at(n);
	}
	return r;
}
std::string Animation::StateMachine::GetState(int i) const
{
	std::string r = "No State";
	if (reverse_stateLookup.find(i) != reverse_stateLookup.end())
	{
		r = reverse_stateLookup.at(i);
	}
	return r;
}
std::vector<FrameData> Animation::StateMachine::GetFrame(int frame_index) const
{
	return std::vector<FrameData>();
}
const int Animation::StateMachine::getStateIndex(std::string key)
{
	return 0;
}
//RESOURCE //////////////////////////////////////////////////////////////////////////////////////////////////


const bool& Animation::Resource::onStateModified(const std::vector<Animation::Transition>& transitions)
{
	bool mod = false;
	for (auto& k : transitions)
	{
		mod = k.Evaluate(this->currentState, this->keys);
		if (mod) break;
	}
	return mod;
}

void Animation::Resource::SetState(int new_state)
{
	if (new_state == this->currentState) return;
	auto& a = AnimationStateManager::Instance()->GetAnimator(this->texId);

	if (a.states.find(new_state) != a.states.end())
	{
		this->currentState = new_state;
		this->onStateModified(a.stateTransitions[new_state]);
	}
}

void Animation::Resource::SetFieldf(std::string key, float v)
{
	auto& a = AnimationStateManager::Instance()->GetAnimator(this->texId);
	auto key_pair = a.keyLookup.find(key);
	if (key_pair != a.keyLookup.end())
	{
		this->keys[key_pair->second].target.numericValue = v;
		this->onStateModified(a.stateTransitions[this->currentState]);
	}
}

void Animation::Resource::SetFieldb(std::string key, bool v)
{
	auto& a = AnimationStateManager::Instance()->GetAnimator(this->texId);
	auto key_pair = a.keyLookup.find(key);
	if (key_pair != a.keyLookup.end())
	{
		this->keys[key_pair->second].target.boolvalue = v;
		this->onStateModified(a.stateTransitions[this->currentState]);
	}
}

void Animation::Resource::SetFields(std::string key, std::string v)
{
	auto& a = AnimationStateManager::Instance()->GetAnimator(this->texId);
	auto key_pair = a.keyLookup.find(key);
	if (key_pair != a.keyLookup.end())
	{
		this->keys[key_pair->second].target.stringValue = v;
		this->onStateModified(a.stateTransitions[this->currentState]);
	}
}

const int Animation::Resource::getStateIndex(std::string key)
{
	auto& a = AnimationStateManager::Instance()->GetAnimator(this->texId);
	if (a.stateLookup.find(key) != a.stateLookup.end())
	{
		return a.stateLookup[key];
	}
	return -1;
}

std::vector<FrameData> Animation::Resource::GetFrameCurrent()
{
	return this->GetFrame(this->currentState);
}

std::vector<FrameData> Animation::Resource::GetFrame(int frame_index)
{
	auto& a = AnimationStateManager::Instance()->GetAnimator(this->texId);
	return a.GetFrame(frame_index);
}



//Transition  //////////////////////////////////////////////////////////////////////////////////////////////////
bool Animation::Transition::Evaluate(int& currentState, const std::vector<Animation::Condition>& keys) const
{
	int fullfillmentCount = 0;
	for (auto&& con : this->conditions)
	{
		std::string _key = "";
		auto it = find_if(keys.begin(), keys.end(), [&con](const Animation::Condition& obj) {return strcmp(obj.key.c_str(), con.key.c_str()) == 0;});
		bool r = false;
		if (it != keys.end())
		{
			switch (con.mOperator)
			{
			case Animation::EQUAL:
				r = con == (*it);
				break;
			case Animation::GREATER_THAN:
				r = (*it) > con;
				break;
			case Animation::LESS_THAN:
				r = (*it) < con;
				break;
			case Animation::GREATER_THAN_EQUAL_TO:
				r = (*it) >= con;
				break;
			case Animation::LESS_THAN_EQUAL_TO:
				r = (*it) <= con;
				break;
			case Animation::NOTEQUAL:
				r = con != (*it);
				break;
			default:
				break;
			}
		}
		if (r) fullfillmentCount++;
	}
	bool f = fullfillmentCount == this->conditions.size();
	if (f)
	{
		currentState = this->destination;
	}
	return f;
}



//INSPECTIONS  //////////////////////////////////////////////////////////////////////////////////////////////////

static const char* anim_event_list[] = { "NUMBER", "STRING", "BOOL" };
static const char* anim_operator_list[] = { "EQUAL", "GREATER_THAN", "LESS_THAN" , "GREATER_THAN_EQUAL_TO", "LESS_THAN_EQUAL_TO" , "NOTEQUAL" };

bool Animation::Condition::Inspect(const char* title, bool key_in_list)
{

	bool change = false;
	if (ImGui::TreeNode(title))
	{
		//debug the type , dont allow type changes on non keys
		if (key_in_list)
		{

			const char* typeName = anim_event_list[this->type];

			if (ImGui::BeginCombo("Type", typeName))
			{
				for (int n = 0; n <= (int)Animation::Event::BOOL; n++)
				{
					const bool is_selected = n == (int)this->type;
					if (ImGui::Selectable(anim_event_list[n], is_selected))
					{
						this->type = (Animation::Event)n;
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		//and debug the opperator, allow  opperator changes for no keys, don't allow for keys, b/c their opperator is irrelevant
		if (!key_in_list)
		{
			const char* oppName = anim_operator_list[this->mOperator];
			if (ImGui::BeginCombo("Opperator", oppName))
			{
				for (int n = 0; n <= (int)Animation::Operator::NOTEQUAL; n++)
				{
					const bool is_selected = n == (int)this->type;
					if (ImGui::Selectable(anim_operator_list[n], is_selected))
					{
						this->mOperator = (Animation::Operator)n;
					}
					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		//now debug the value
		ImGui::PushItemWidth(150.0f);
		switch (this->type)
		{
		case Animation::NUMBER:
			change = ImGui::InputFloat("value", &this->target.numericValue, 0.5f, 0.1f);
			break;
		case Animation::STRING:
			change = ImGui::InputText("value", &this->target.stringValue);
			break;
		case Animation::BOOL:
			change = ImGui::Checkbox("value", &this->target.boolvalue);
			break;
		default:
			break;
		}

		//if the value has changed and the value is a key value: DO A STATE CHANGE!

		ImGui::TreePop();
	}

	return change && key_in_list;
}
void Animation::Transition::Inspect(const char* title, const std::vector<Animation::Condition>& keys)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::SliderInt("Destination", &destination, 0, 0);

		if (ImGui::TreeNode("conditions"))
		{
			int i = 0;
			for (auto& condition : conditions)
			{
				const char* n = TextFormat("%d - %s", i, condition.key.c_str());
				condition.Inspect(n, false);
				i++;
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void Animation::StateMachine::Inspect(const char* title)
{
	if (ImGui::TreeNode(title))
	{
		if (ImGui::TreeNode(TextFormat("States: %d", states.size())))
		{

			for (auto&& pair : states)
			{
				if (ImGui::TreeNode(reverse_stateLookup[pair.first].c_str()))
				{
					ImGui::Text("Frames: %d", pair.second.size());
					for (auto& transition : stateTransitions[pair.first])
					{
						transition.Inspect(transition.name.c_str(), this->keys);
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		//ImGui::Text("Transitions: %d", r.stateTransitions.size());
		if (ImGui::TreeNode(TextFormat("Keys: %d", keys.size())))
		{
			for (auto& key : keys)
			{
				key.Inspect(key.key.c_str(), true);
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
void Animation::Resource::Inspect(const char* title, const Animation::StateMachine& map)
{
	if (ImGui::TreeNode(title))
	{
		ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "Current State: %s", map.GetState(this->currentState).c_str());
		if (ImGui::TreeNode(TextFormat("Keys: %d", this->keys.size())))
		{
			for (auto& key : this->keys)
			{
				if (key.Inspect(key.key.c_str(), true))
				{
					this->onStateModified(map.GetTransitions(this->currentState));
				}
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}
#define _strcmp(a, b) strcmp(a, b ) == 0

Animation::Operator Animation::ConvertOpperatorString(const char* opp_string)
{
	Animation::Operator k = Animation::Operator::EQUAL;

	if (_strcmp(opp_string, "==")) k = Animation::Operator::EQUAL;
	else if (_strcmp(opp_string, "!=")) k = Animation::Operator::NOTEQUAL;
	else if (_strcmp(opp_string, ">")) k = Animation::Operator::GREATER_THAN;
	else if (_strcmp(opp_string, ">=")) k = Animation::Operator::GREATER_THAN_EQUAL_TO;
	else if (_strcmp(opp_string, "<")) k = Animation::Operator::LESS_THAN;
	else if (_strcmp(opp_string, "<=")) k = Animation::Operator::LESS_THAN_EQUAL_TO;

	return k;
}
