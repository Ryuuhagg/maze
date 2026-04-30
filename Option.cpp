//Option.cpp
#include <unordered_map>
#include <algorithm>
#include <string>
#include"Option.h"
#include"DxLib.h"
#include<fstream>
#include"json.hpp"

using namespace std;
using json = nlohmann::json;

string ToUpper(string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

#pragma region Key
unordered_map<string, int> stringToKey = {
	{"SPACE", KEY_INPUT_SPACE},
	{"ENTER", KEY_INPUT_RETURN},
	{"ESC", KEY_INPUT_ESCAPE},
	{"A", KEY_INPUT_A},
	{"D", KEY_INPUT_D},
	{"W", KEY_INPUT_W},
	{"S", KEY_INPUT_S},
	{"UP", KEY_INPUT_UP},
	{"DOWN", KEY_INPUT_DOWN},
	{"LEFT", KEY_INPUT_LEFT},
	{"RIGHT", KEY_INPUT_RIGHT}
};

unordered_map<int, string> keyToString = {
	{KEY_INPUT_SPACE, "SPACE"},
	{KEY_INPUT_RETURN, "ENTER"},
	{KEY_INPUT_ESCAPE, "ESC"},
	{KEY_INPUT_A, "A"},
	{KEY_INPUT_D, "D"},
	{KEY_INPUT_W, "W"},
	{KEY_INPUT_S, "S"},
	{KEY_INPUT_UP, "UP"},
	{KEY_INPUT_DOWN, "DOWN"},
	{KEY_INPUT_LEFT, "LEFT"},
	{KEY_INPUT_RIGHT, "RIGHT"}
};

int StringToKeySafe(string s) {
	s = ToUpper(s);
	return stringToKey.count(s) ? stringToKey[s] : KEY_INPUT_SPACE;
}
#pragma endregion
#pragma region Pad
unordered_map<string, int> stringToPad = {
	{"A", PAD_INPUT_1},
	{"B", PAD_INPUT_2},
	{"X", PAD_INPUT_3},
	{"Y", PAD_INPUT_4},
	{"L", PAD_INPUT_5},
	{"R", PAD_INPUT_6},
	{"LT", PAD_INPUT_7},
	{"RT", PAD_INPUT_8}
};

unordered_map<int, string> PadToString = {
	{PAD_INPUT_1, "A"},
	{PAD_INPUT_2, "B"},
	{PAD_INPUT_3, "X"},
	{PAD_INPUT_4, "Y"},
	{PAD_INPUT_5, "L"},
	{PAD_INPUT_6, "R"},
	{PAD_INPUT_7, "LT"},
	{PAD_INPUT_8, "RT"}
};

int StringToPadSafe(string s) {
	s = ToUpper(s);
	return stringToPad.count(s) ? stringToKey[s] : PAD_INPUT_1;
}
#pragma endregion


void Option::SaveOption(const Config& config) {
	json j;

	j["masterVolume"] = config.masterVolume;
	j["SEVolume"] = config.SEVolume;
	j["BGMVolume"] = config.BGMVolume;

	j["CamelaUpDownFlip"] = config.CamelaUpDownFlip;
	j["CamelaRightLeftFlip"] = config.CamelaRightLeftFlip;

	j["KeyJump"] = keyToString[config.KeyJump];
	j["PadJump"] = PadToString[config.PadJump];

	j["KeyConfirm"] = keyToString[config.KeyConfirm];
	j["PadConfirm"] = PadToString[config.PadConfirm];

	j["KeyCancel"] = keyToString[config.KeyCancel];
	j["PadCancel"] = PadToString[config.PadCancel];

	ofstream file("config.json");
	if (file) {
		file << j.dump(4);
	}
}

void Option::LoadOption(Config& config) {
	ifstream file("config.json");

	if (file) {
		json j;
		file >> j;

		config.masterVolume = j.value("masterVolume", 50);
		config.SEVolume = j.value("SEVolume", 50);
		config.BGMVolume = j.value("BGMVolume", 50);

		config.CamelaUpDownFlip = j.value("CamelaUpDownFlip", false);
		config.CamelaRightLeftFlip = j.value("CamelaRightLeftFlip", false);

		config.KeyJump = StringToKeySafe(j.value("KeyJump", "SPACE"));
		config.PadJump = StringToPadSafe(j.value("PadJump", "X"));

		config.KeyConfirm = StringToKeySafe(j.value("KeyConfirm", "ENTER"));
		config.PadConfirm = StringToPadSafe(j.value("PadConfirm", "B"));

		config.KeyCancel = StringToKeySafe(j.value("KeyCancel", "ESC"));
		config.PadCancel = StringToPadSafe(j.value("PadCancel", "A"));
	}
	else {
		config.masterVolume = 50;
		config.SEVolume = 50;
		config.BGMVolume = 50;

		config.CamelaUpDownFlip = false;
		config.CamelaRightLeftFlip = false;

		config.KeyJump = KEY_INPUT_SPACE;
		config.PadJump = PAD_INPUT_3;

		config.KeyConfirm = KEY_INPUT_RETURN;
		config.PadConfirm = PAD_INPUT_2;

		config.KeyCancel = KEY_INPUT_ESCAPE;
		config.PadCancel = PAD_INPUT_1;

		SaveOption(config);
	}
}
