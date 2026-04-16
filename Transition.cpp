#include"DxLib.h"
#include"Transition.h"
#include"Constant.h"

void Fade::Update() {
	if (fadeOut) {
		alpha += 10;
		if (alpha >= 255)fadeOut = false;
	}
	else {
		alpha -= 10;
	}
}

void Fade::Draw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, WIDTH, HEIGHT, GetColor(0, 0, 0), TRUE);
}