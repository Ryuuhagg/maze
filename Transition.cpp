//Transition.cpp
#include"DxLib.h"
#include"Transition.h"
#pragma region Fade
void Fade::Update() {
	if (Enter) {
		alpha += 10;
		if (alpha >= 255) {
			alpha = 255;
			Enter = false;
			Switched = true;
		}
	}
	if (Exit) {
		alpha -= 10;
		if (alpha <= 0) {
			alpha = 0;
			Exit = false;
		}
	}
}

void Fade::Draw() {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, WIDTH, HEIGHT, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

TransitionState Fade::GetState() {
	if (Switched) {
		Switched = false;
		Exit = true;
		return TransitionState::Switching;
	}
	if (Enter) return TransitionState::Enter;
	if (Exit) return TransitionState::Exit;

	return TransitionState::Finished;
}
#pragma endregion
#pragma region Slide
void Slide::Update() {
	if (Enter) {
		current += 10;
		if (current == 0) {
			current = 0;
			Enter = false;
			Switched = true;
		}
	}
	if (Exit) {
		current += 10;
		if (current > WIDTH) {
			Exit = false;
			current = WIDTH;
		}
	}
}

void Slide::Draw() {
	DrawBox(current, 0, current+WIDTH, HEIGHT, GetColor(100, 100, 100), TRUE);
}

TransitionState Slide::GetState() {
	if (Switched) {
		Switched = false;
		Exit = true;
		return TransitionState::Switching;
	}
	if (Enter) return TransitionState::Enter;
	if (Exit)return TransitionState::Exit;
	return TransitionState::Finished;
}
#pragma endregion