#pragma once

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
};

class Option {
	Config config;
public:
	void SaveOption(const Config& config);
	void LoadOption(Config& config);
};