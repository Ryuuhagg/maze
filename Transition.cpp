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

TransitionState Fade::GetState() {
	if (alpha < 255 && fadeOut) return TransitionState::Enter;
	if (alpha >= 255 && fadeOut) return TransitionState::Switching;
	if (!fadeOut && alpha > 0) return TransitionState::Exit;
	return TransitionState::Finished;
}