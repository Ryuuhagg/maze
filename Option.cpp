//Option.cpp
#include"Option.h"
#include"DxLib.h"
#include<fstream>
using namespace std;

void Option::SaveOption(const Config& config) {
	ofstream file("save.dat",ios::binary);
	if(file)
		file.write((char*)&config, sizeof(config));
}

void Option::LoadOption(Config& config) {
	ifstream file("save.dat", ios::binary);
	if(file)
		file.read((char*)&config, sizeof(config));
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
