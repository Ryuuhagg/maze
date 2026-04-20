#include"DxLib.h"
#include"Transition.h"
#include"Constant.h"

void Fade::Update() {
	if (fadeOut) {
		alpha += 10;
		if (alpha >= 255) {
			alpha = 255;
			fadeOut = false;
			switched = true;
		}
	}
	else {
		alpha -= 10;
		if (alpha <= 0) alpha = 0;
	}
}

void Fade::Draw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, WIDTH, HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

TransitionState Fade::GetState() {
	if (switched) {
		switched = false;
		return TransitionState::Switching;
	}
	if (fadeOut) return TransitionState::Enter;
	if (alpha > 0) return TransitionState::Exit;

	return TransitionState::Finished;
}