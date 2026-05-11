//Option.h
#pragma once
#include <string>
struct Config {
	int masterVolume;
	int SEVolume;
	int BGMVolume;

	bool CamelaUpDownFlip;
	bool CamelaRightLeftFlip;

	int KeyJump;
	int PadJump;

	int KeyConfirm;
	int PadConfirm;

	int KeyCancel;
	int PadCancel;

	int KeyDash;
	int PadDash;

	int KeyFront;
	int KeyBack;
	int KeyRight;
	int KeyLeft;
};

class Option {
	Config config;
public:
	void SaveOption(const Config& config);
	void LoadOption(Config& config);
};

int StringToKeySafe(const std::string& s);
std::string KeyToStringSafe(int key);