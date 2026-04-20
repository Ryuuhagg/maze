#include"Input.h"
#include"DxLib.h"

char Input::now[256];
char Input::prev[256];

int Input::padNow = 0;
int Input::padPrev = 0;

void Input::Update() {
	memcpy(prev, now, 256);
	GetHitKeyStateAll(now);

	padPrev = padNow;
	padNow = GetJoypadInputState(DX_INPUT_PAD1);
}