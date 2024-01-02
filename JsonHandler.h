#pragma once
/*
	take in a json object like this:
	{
		"settings": {
		"Background_Color":  {"r":  1.0, "g": 1.0, "b": 0.0, "a": 1.0}
		},
		"objects": {
		"rectangle" :{
			"position": {"x":  0.0, "y" :  0.0, "z":  0.0},
			"size": {"x":  100.0, "y" :  100.0, "z":  10.0},
			"angle": 0.0,
			"color":  {"r":  1.0, "g": 1.0, "b": 0.0, "a": 1.0},
			"origin":  {"x":  0.0, "y": 0.0, "z": 0.0},
			"animations": [30.0, 60.0, 90.0, 180.0, 270.0, 360.0]
		}
		}
	}
	and store it in a data structure like this:
	object.data
		.name = "root"
		.children [
			"settings".data
				.name = "settings"
				.children [
					"background_color".data
						.name = "background_color"
						.children [
							"r".data
								.name = "r"
								.doubles[1.0]
							"g".data
								.name = "g"
								.doubles[1.0]
							"b".data
								.name = "b"
								.doubles[0.0]
							"a".data
								.name = "a"
								.doubles[1.0]
						]
				]
		]
	and present it like this:
		float coin_duration = object.data.getBase<Type>("duration"); [return ref to & object.rawdata.floats[0].first]
	&&
		bool success = object.data.SetObjectValue("frame", "w", value); [return success for object.rawdata.ints[2].first = value ]
*/
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <set>

struct jsonObjects
{
	std::string name = "root";
	bool isObject = false;
	std::vector<int> integers;
	std::vector<double> doubles;
	std::vector<bool> booleans;
	std::vector<std::string> strings;
	std::vector<jsonObjects> children;


	void print(int l = 0) const;
	std::string debugPrint(int l = 0) const;
	jsonObjects& find(const char* name);
	jsonObjects& findOrCreate(const char* name);
	//getters
	jsonObjects& GetNumber(const char* fieldName, float& reference, char dilimeter = '.');
	jsonObjects& GetInteger(const char* fieldName, float& reference, char dilimeter = '.');
	jsonObjects& GetInteger(const char* fieldName, int& reference, char dilimeter = '.');
	jsonObjects& GetString(const char* fieldName, std::string& reference, char dilimeter = '.');
	jsonObjects& Getboolean(const char* fieldName, bool& reference, char dilimeter = '.');
	//setters
	jsonObjects& SetNumberAt(const char* fieldName, float value, char dilimeter = '.');
	//does set SetNumberAt adds the field if it is not found as a child
	jsonObjects& SetNumberAtAndAdd(const char* fieldName, float value, char dilimeter = '.');
	jsonObjects& SetIntegerAt(const char* fieldName, int value, char dilimeter = '.');
	jsonObjects& SetBooleanAt(const char* fieldName, bool value);
	jsonObjects& SetStringAt(const char* fieldName, std::string value);


	//exporters
	//generate json string
	std::string GenerateJSON(int l = 0) const;
	//generate json and save to file at specified path
	bool SaveToFile(const char* file_path);
	void clear();


};
struct jsonStats;

namespace sajson
{
	enum type : uint8_t;
	class value;
}
namespace JSONParse
{
	bool LoadFile(char*& data, int& size, const char* file_path);
	bool ParseJSONFile(jsonObjects& data, const char* file_path, jsonStats* stats = NULL);
	bool ParseString(jsonObjects& data, const char* json_file_string, bool hasRoot = true);
	void Objtraverse(jsonObjects& obj, const sajson::value& node, int level);

}


struct jsonStats {
	size_t null_count = 0;
	size_t false_count = 0;
	size_t true_count = 0;
	size_t number_count = 0;
	size_t object_count = 0;
	size_t array_count = 0;
	size_t string_count = 0;
	size_t total_string_length = 0;
	size_t total_array_length = 0;
	size_t total_object_length = 0;
	double total_number_value = 0;

};