#pragma once
#include <map>
#include <vector>
#include <string>
#include "RenderPipeline.h"
#include "JsonHandler.h"
namespace Animation
{
	struct FrameData
	{
		Rectangle frame = Rectangle{ 0, 0, 64, 64 };
		float duration = 0.15f;
	};
	enum Event { NUMBER = 0, STRING, BOOL };
	enum Operator { EQUAL = 0, GREATER_THAN, LESS_THAN, GREATER_THAN_EQUAL_TO, LESS_THAN_EQUAL_TO, NOTEQUAL };
	struct Value
	{
		std::string stringValue = "";
		float		numericValue = 0.0;
		bool		boolvalue = false;
	};
	struct Condition
	{
		Event type = NUMBER;
		Operator mOperator = EQUAL;
		std::string key;
		Value target;

		friend bool operator==(const Animation::Condition& a, const Animation::Condition& b)
		{
			bool result = false;
			switch (a.type)
			{
			case Animation::NUMBER:
				result = a.target.numericValue == b.target.numericValue;
				break;
			case Animation::STRING:
				result = strcmp(a.target.stringValue.c_str(), b.target.stringValue.c_str()) == 0;
				break;
			case Animation::BOOL:
				result = a.target.boolvalue == b.target.boolvalue;
				break;
			default:
				break;
			}
			return result;
		}
		friend bool operator!=(const Animation::Condition& a, const Animation::Condition& b)
		{
			bool r = a == b;
			return r == false;
		}
		friend bool operator<(const Animation::Condition& a, const Animation::Condition& b)
		{
			bool result = false;
			switch (a.type)
			{
			case Animation::NUMBER:
				result = a.target.numericValue < b.target.numericValue;
				break;
			default:
				break;
			}
			return result;
		}
		friend bool operator<=(const Animation::Condition& a, const Animation::Condition& b)
		{
			bool result = false;
			switch (a.type)
			{
			case Animation::NUMBER:
				result = a.target.numericValue <= b.target.numericValue;
				break;
			default:
				break;
			}
			return result;
		}
		friend bool operator>(const Animation::Condition& a, const Animation::Condition& b)
		{
			bool result = false;
			switch (a.type)
			{
			case Animation::NUMBER:
				result = a.target.numericValue > b.target.numericValue;
				break;
			default:
				break;
			}
			return result;
		}
		friend bool operator>=(const Animation::Condition& a, const Animation::Condition& b)
		{
			bool result = false;
			switch (a.type)
			{
			case Animation::NUMBER:
				result = a.target.numericValue >= b.target.numericValue;
				break;
			default:
				break;
			}
			return result;
		}

		bool Inspect(const char* title, bool key_in_list);
	};
	struct Transition
	{
		std::string name = "transition";
		int origin = -1;
		int destination = -1;
		std::vector<Animation::Condition> conditions = {};
		bool fullfilled = false;

		bool Evaluate(int& currentState, const std::vector<Animation::Condition>& keys) const;
		void Inspect(const char* title, const std::vector<Animation::Condition>& keys);
	};

	/*
	*/
	struct StateMachine
	{
		int defaultState = 0;
		std::map<int, std::vector<Animation::FrameData>> states = {};
		std::map<int, std::vector<Animation::Transition>> stateTransitions = {};
		std::map<std::string, int> stateLookup = {};
		std::map< int, std::string> reverse_stateLookup = {};

		std::vector<Animation::Condition> keys;
		std::map<std::string, int> keyLookup;

		std::string transition_file_path;
		jsonObjects transition_data;

		std::vector<Animation::Condition> GetKeys() const { return keys; }

		std::vector<Animation::Transition> GetTransitions(int state) const;

		int GetState(std::string n) const;
		std::string GetState(int i) const;
		std::vector<Animation::FrameData> GetFrame(int frame_index) const;
		const int getStateIndex(std::string key);
		void Inspect(const char* title);
	};

	struct Resource
	{
		int currentState = 0;
		unsigned int texId = 0;
		std::vector<Animation::Condition> keys;

		const bool& onStateModified(const std::vector<Animation::Transition>& transitions);
		void SetState(int new_state);
		void SetFieldf(std::string key, float v);
		void SetFieldb(std::string key, bool v);
		void SetFields(std::string key, std::string v);
		const int getStateIndex(std::string key);

		std::vector<Animation::FrameData> GetFrameCurrent();
		std::vector<Animation::FrameData> GetFrame(int frame_index);

		void Inspect(const char* title, const Animation::StateMachine& map);
	};
}

