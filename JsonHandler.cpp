#define _CRT_SECURE_NO_WARNINGS

#include "JsonHandler.h"
#include "sajson.h"
#include <fstream>
#include <iostream>

bool JSONParse::LoadFile(char*& data, int& size, const char* file_path)
{
	FILE* file = fopen(file_path, "rb");
	if (file == nullptr)
	{
		printf("%s (%d)\t cannot open file '%s'", __FILE__, __LINE__, file_path);
		return false;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (size == 0)
	{
		printf("%s (%d)\t file '%s': has no data", __FILE__, __LINE__, file_path);
		fclose(file);
		return false;
	}

	data = (char*)malloc(size + 1);
	fread(data, size, 1, file);
	fclose(file);
	data[size] = 0;
	return true;
}

bool JSONParse::ParseJSONFile(jsonObjects& data, const char* file_path, jsonStats* stats)
{
	int dataSize;
	char* rawdata;
	if (!LoadFile(rawdata, dataSize, file_path)) { printf("\n\tcould not read file and could not initialize data.\n"); return false; }

	const sajson::document& document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(dataSize, rawdata));
	if (!document.is_valid())
	{
		int line = document.get_error_line();
		int column = document.get_error_column();
		const char* message = document.get_error_message_as_cstring();
		printf("%s\t\tFile '%s' is invalid\n\t[Line %d, column %d]:\t%s\n", __FILE__, file_path, line, column, message);

		return false;
	}
	data.clear();
	sajson::value root = document.get_root();
	Objtraverse(data, root, 0);

	if (stats)
	{
		//traverse(*stats, root, 0);
	}

	return true;
}

bool JSONParse::ParseString(jsonObjects& data, const char* json_file_string, bool hasRoot)
{
	int dataSize;

	dataSize = strlen(json_file_string);
	const sajson::document& document = sajson::parse(sajson::dynamic_allocation(), sajson::mutable_string_view(dataSize, (char*)json_file_string));
	if (!document.is_valid())
	{
		int line = document.get_error_line();
		int column = document.get_error_column();
		const char* message = document.get_error_message_as_cstring();
		printf("%s\t\tstring '%s' is invalid\n\t[Line %d, column %d]:\t%s\n", __FILE__, json_file_string, line, column, message);

		return false;
	}
	data.clear();
	sajson::value root = document.get_root();

	jsonObjects newData;
	Objtraverse(newData, root, 0);

	if (hasRoot == false) //disregard the root  and grab the first child and assign that to the data
	{
		if (newData.children.size() > 0)
			data = newData.children[newData.children.size() - 1];
	}
	return true;
}



void ptab(int t)
{
	printf("\n");
	for (int i = 0; i < t; ++i)
	{
		printf("\t");
	}
}

void JSONParse::Objtraverse(jsonObjects& obj, const sajson::value& node, int level)
{
	switch (node.get_type())
	{
	case sajson::TYPE_INTEGER:
		obj.integers.push_back(node.get_integer_value());
		obj.isObject = false;
		break;
	case sajson::TYPE_DOUBLE:
		obj.doubles.push_back(node.get_number_value());
		obj.isObject = false;
		break;
	case sajson::TYPE_NULL:
		obj.isObject = false;
		break;
	case sajson::TYPE_FALSE:
		obj.booleans.push_back(false);
		obj.isObject = false;
		break;
	case sajson::TYPE_TRUE:
		obj.booleans.push_back(true);
		obj.isObject = false;
		break;
	case sajson::TYPE_STRING:
		obj.isObject = false;
		obj.strings.push_back(node.as_string());
		break;
	case sajson::TYPE_ARRAY: {
		obj.isObject = false;
		auto length = node.get_length();
		for (size_t i = 0; i < length; ++i) {
			//lets look forward in time real quick, see if the array element is an object
			if (node.get_array_element(i).get_type() == sajson::TYPE_OBJECT)
			{
				//printf("\nFor the child of '%s' I should do something different here...\n", obj.name.c_str());

				jsonObjects newChild;
				newChild.name = obj.name + ": " + std::to_string(i);
				newChild.isObject = true;
				Objtraverse(newChild, node.get_array_element(i), level + 1);
				obj.children.push_back(newChild);
			}
			else
			{
				Objtraverse(obj, node.get_array_element(i), level + 1);
			}

		}

		break;
	}
	case sajson::TYPE_OBJECT: {
		obj.isObject = true;
		auto length = node.get_length();
		for (auto i = 0u; i < length; ++i) {
			jsonObjects newChild;
			newChild.name = node.get_object_key(i).as_string();
			newChild.isObject = true;
			Objtraverse(newChild, node.get_object_value(i), level + 1);
			obj.children.push_back(newChild);
		}
		break;
	}

	default:
		break;
	}
}

void jsonObjects::print(int l) const
{
	printf("\n%s\n", this->GenerateJSON().c_str());
}


static jsonObjects* recursive_find(jsonObjects* node, bool& found, const char* fieldName)
{

	found = strcmp(fieldName, node->name.c_str()) == 0;
	if (found) return node;
	for (auto& child : node->children) {
		jsonObjects* mNode = recursive_find(&child, found, fieldName);
		if (found)
		{
			return mNode;
		}
	}

	return node;


}


jsonObjects& jsonObjects::find(const char* name)
{
	bool found = false;
	return *recursive_find(this, found, name);
}

jsonObjects& jsonObjects::findOrCreate(const char* name)
{

	bool found = false;
	jsonObjects* object = recursive_find(this, found, name);

	//if the object is the 'root' create a new object w/ a name of 'name'
	if (object == this)
	{
		jsonObjects new_obj;

		new_obj.name = name;
		new_obj.isObject = true;
		this->children.push_back(new_obj);
		return this->children[this->children.size() - 1];
	}
	else
	{
		return *object;
	}

}

jsonObjects& jsonObjects::GetNumber(const char* name, float& reference, char dilimeter)
{
	const jsonObjects& nNode = find(name);

	bool numberExists = nNode.doubles.size() > 0;
	if (numberExists)
	{
		reference = (float)nNode.doubles[0];
	}

	return  *this;
}
jsonObjects& jsonObjects::GetInteger(const char* fieldName, float& reference, char dilimeter)
{
	const jsonObjects& nNode = find(fieldName);

	bool numberExists = nNode.integers.size() > 0;
	if (numberExists)
	{
		reference = nNode.integers[0];
	}

	return  *this;
}
jsonObjects& jsonObjects::GetInteger(const char* fieldName, int& reference, char dilimeter)
{
	const jsonObjects& nNode = find(fieldName);

	bool numberExists = nNode.integers.size() > 0;
	if (numberExists)
	{
		reference = nNode.integers[0];
	}

	return  *this;
}
jsonObjects& jsonObjects::GetString(const char* fieldName, std::string& reference, char dilimeter)
{
	const jsonObjects& nNode = find(fieldName);

	bool numberExists = nNode.strings.size() > 0;
	if (numberExists)
	{
		reference = nNode.strings[0];
	}

	return  *this;
}
jsonObjects& jsonObjects::Getboolean(const char* fieldName, bool& reference, char dilimeter)
{
	// TODO: insert return statement here
	const jsonObjects& nNode = find(fieldName);

	bool numberExists = nNode.booleans.size() > 0;
	if (numberExists)
	{
		reference = nNode.booleans[0];
	}

	return  *this;
}
jsonObjects& jsonObjects::SetNumberAt(const char* fieldName, float value, char dilimeter)
{
	jsonObjects& nNode = find(fieldName);
	bool valid = strcmp(nNode.name.c_str(), fieldName) == 0;
	if (valid)
		nNode.doubles[0] = value;

	return  *this;
}

jsonObjects& jsonObjects::SetNumberAtAndAdd(const char* fieldName, float value, char dilimeter)
{
	// // O: insert return statement here
	jsonObjects& nNode = find(fieldName);
	bool found = strcmp(nNode.name.c_str(), fieldName) == 0;
	if (found)
	{
		nNode.doubles[0] = value;
	}
	else
	{
		jsonObjects nChild;
		nChild.name = fieldName;
		nChild.doubles.push_back(value);
		nNode.children.push_back(nChild);
	}

	return  *this;
}

jsonObjects& jsonObjects::SetIntegerAt(const char* fieldName, int value, char dilimeter)
{
	jsonObjects& nNode = find(fieldName);
	bool valid = strcmp(nNode.name.c_str(), fieldName) == 0;
	if (valid)
		nNode.integers[0] = value;
	else
	{
		jsonObjects nChild;
		nChild.name = fieldName;
		nChild.integers.push_back(value);
		nNode.children.push_back(nChild);
	}

	return  *this;
}

jsonObjects& jsonObjects::SetBooleanAt(const char* fieldName, bool value)
{
	// // O: insert return statement here
	jsonObjects& nNode = find(fieldName);
	bool found = strcmp(nNode.name.c_str(), fieldName) == 0;
	if (found)
	{
		nNode.booleans[0] = value;
	}
	else
	{
		jsonObjects nChild;
		nChild.name = fieldName;
		nChild.booleans.push_back(value);
		nNode.children.push_back(nChild);
	}

	return  *this;
}

jsonObjects& jsonObjects::SetStringAt(const char* fieldName, std::string value)
{
	// TODO: insert return statement here
	jsonObjects& nNode = find(fieldName);
	bool found = strcmp(nNode.name.c_str(), fieldName) == 0;
	if (found)
	{
		nNode.strings[0] = value;
	}
	else
	{
		jsonObjects nChild;
		nChild.name = fieldName;
		nChild.strings.push_back(value);
		nNode.children.push_back(nChild);
	}

	return  *this;
}




void atab(std::string& r, int t)
{
	for (int i = 0; i < t; ++i) r += "\t";
}

std::string jsonObjects::GenerateJSON(int level) const
{

	std::string result = "\n";
	bool isRoot = (level == 0);
	if (isRoot || this->isObject) {
		atab(result, level);
		if (this->isObject && !isRoot) result += "\"" + this->name + "\": ";

		result += "{";
	}
	else
	{
		atab(result, level);
		result += "\"" + this->name + "\": ";
	}

	//loop for values
	if (integers.size() > 1) result += "[";
	for (int i = 0; i < integers.size(); i++)
	{
		result += std::to_string(integers[i]);
		if (i < integers.size() - 1) result += ", ";
	}
	if (integers.size() > 1) result += "]";


	if (doubles.size() > 1) result += "[";
	for (int i = 0; i < doubles.size(); i++)
	{
		result += std::to_string(doubles[i]);
		if (i < doubles.size() - 1) result += ", ";
	}
	if (doubles.size() > 1) result += "]";

	if (booleans.size() > 1) result += "[";
	for (int i = 0; i < booleans.size(); i++)
	{
		result += booleans[i] ? "true" : "false";
		if (i < booleans.size() - 1) result += ", ";
	}
	if (booleans.size() > 1) result += "]";

	if (strings.size() > 1) result += "[";
	for (int i = 0; i < strings.size(); i++)
	{
		result += "\"" + strings[i] + "\"";
		if (i < strings.size() - 1) result += ", ";
	}
	if (strings.size() > 1) result += "]";

	for (int i = 0; i < children.size(); i++)
	{
		result += children[i].GenerateJSON(level + 1);
		if (i < children.size() - 1)  result += ", ";
	}
	if (isRoot || this->isObject) { result += "\n"; atab(result, level); result += "}"; }

	return result;
}
//#include "SystemLog.h"
bool jsonObjects::SaveToFile(const char* file_path)
{
#if false
	FILE* file = fopen(file_path, "w");

	fprintf(file, "%s", this->GenerateJSON().c_str());

	fclose(file);
#else
	std::ofstream file;
	file.open(file_path);
	file.clear();
	file << this->GenerateJSON().c_str();
	file.close();
#endif
	printf("[JsonParse.cpp]\t\tSuccessfully Saved File '%s':\n", file_path);

	return true;
}
void jsonObjects::clear()
{
	name = "root";
	integers.clear();
	doubles.clear();
	booleans.clear();
	strings.clear();
	children.clear();
}



std::string jsonObjects::debugPrint(int level) const
{

	std::string result = "\n";
	bool isRoot = level == 0;
	if (isRoot)
	{
		result += "{";
	}
	else
	{
		atab(result, level);
		result += "\"" + this->name + "\": ";

		if (isObject)
		{
			result += "\n";
			atab(result, level);
			result += "{";
			atab(result, level);
		}
	}
	if (integers.size() > 1) result += "[";
	for (int i = 0; i < integers.size(); i++)
	{
		result += std::to_string(integers[i]);
		if (i < integers.size() - 1) result += ", ";
	}
	if (integers.size() > 1) result += "]";


	if (doubles.size() > 1) result += "[";
	for (int i = 0; i < doubles.size(); i++)
	{
		result += std::to_string(doubles[i]);
		if (i < doubles.size() - 1) result += ", ";
	}
	if (doubles.size() > 1) result += "]";

	if (booleans.size() > 1) result += "[";
	for (int i = 0; i < booleans.size(); i++)
	{
		result += booleans[i] ? "true" : "false";
		if (i < booleans.size() - 1) result += ", ";
	}
	if (booleans.size() > 1) result += "]";

	if (strings.size() > 1) result += "[";
	for (int i = 0; i < strings.size(); i++)
	{
		result += "\"" + strings[i] + "\"";
		if (i < strings.size() - 1) result += ", ";
	}
	if (strings.size() > 1) result += "]";

	//for (auto& i : this->children)
	for (int i = 0; i < children.size(); i++)
	{
		result += children[i].debugPrint(level + 1);
		if (i < children.size() - 1)  result += ", ";
	}
	if (isObject)
	{
		result += "\n";
		atab(result, level);
		result += "}";
	}
	//if (isRoot) { result += "\n}"; } root is already an object, so we do not need to add an additional brace

	printf("%d:: '%s' \n", level, result.c_str());
	return result;
}
