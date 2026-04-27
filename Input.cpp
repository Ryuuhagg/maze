//Input.cpp
#include"Input.h"
#include"DxLib.h"
#include <cmath>

char Input::now[256];
char Input::prev[256];

int Input::padNow = 0;
int Input::padPrev = 0;

float Input::prevLX = 0.0f;
float Input::prevLY = 0.0f;

int Input::mouseNow = 0;
int Input::mousePrev = 0;

int Input::mouseX = 0;
int Input::mouseY = 0;

int Input::prevMouseX = 0;
int Input::prevMouseY = 0;

Config Input::config;

void Input::Update() {
	memcpy(prev, now, 256);
	GetHitKeyStateAll(now);

	padPrev = padNow;
	padNow = GetJoypadInputState(DX_INPUT_PAD1);

    prevLX = GetPadLX();
    prevLY = GetPadLY();

    mousePrev = mouseNow;
    mouseNow = GetMouseInput();

    prevMouseX = mouseX;
    prevMouseY = mouseY;
    GetMousePoint(&mouseX, &mouseY);
}

bool Input::IsActionTrigger(Action action) {
    switch (action) {
    case Action::Confirm:
        return IsKeyTrigger(config.KeyConfirm) || IsPadTrigger(config.PadConfirm) || IsMouseTrigger(MOUSE_INPUT_LEFT);

    case Action::Cancel:
        return IsKeyTrigger(config.KeyCancel) || IsPadTrigger(config.PadCancel) || IsMouseTrigger(MOUSE_INPUT_RIGHT);

    case Action::Jump:
        return IsKeyTrigger(config.KeyJump) || IsPadTrigger(config.PadJump);
    }
    return false;
}

float Input::GetPadLX() {
    int x, y;
    GetJoypadAnalogInput(&x, &y, DX_INPUT_PAD1);
    return x / 1000.0f;
}

float Input::GetPadLY() {
    int x, y;
    GetJoypadAnalogInput(&x, &y, DX_INPUT_PAD1);
    return y / 1000.0f;
}

float Input::GetPadRX() {
    int x, y;
    GetJoypadAnalogInputRight(&x, &y, DX_INPUT_PAD1);
    return x / 1000.0f;
}

float Input::GetPadRY() {
    int x, y;
    GetJoypadAnalogInputRight(&x, &y, DX_INPUT_PAD1);
    return y / 1000.0f;
}

float Input::GetAxisLX() {
    float x = 0;

    // キーボード
    if (IsKeyPressed(KEY_INPUT_A)) x -= 1.0f;
    if (IsKeyPressed(KEY_INPUT_D)) x += 1.0f;

    // パッド
    float lx = GetPadLX();

    lx = ApplyDeadZone(lx);

    // 強い方を使う
    if (fabs(lx) > fabs(x)) x = lx;

    return x;
}

float Input::GetAxisLY() {
    float y = 0;

    // キーボード
    if (IsKeyPressed(KEY_INPUT_W)) y += 1.0f;
    if (IsKeyPressed(KEY_INPUT_S)) y -= 1.0f;

    // パッド
    float ly = GetPadLY();

    // 上がマイナスなので反転
    ly = -ly;

    ly = ApplyDeadZone(ly);

    if (fabs(ly) > fabs(y)) y = ly;

    return y;
}

float Input::GetAxisRX() {
    float x = 0;

    // キーボード
    if (IsKeyPressed(KEY_INPUT_LEFT)) x -= 1.0f;
    if (IsKeyPressed(KEY_INPUT_RIGHT)) x += 1.0f;

    // パッド
    float lx = GetPadRX();

    lx = ApplyDeadZone(lx);

    // 強い方を使う
    if (fabs(lx) > fabs(x)) x = lx;

    return x;
}

float Input::GetAxisRY() {
    float y = 0;

    // キーボード
    if (IsKeyPressed(KEY_INPUT_UP)) y += 1.0f;
    if (IsKeyPressed(KEY_INPUT_DOWN)) y -= 1.0f;

    // パッド
    float ly = GetPadRY();

    // 上がマイナスなので反転
    ly = -ly;

    ly = ApplyDeadZone(ly);

    if (fabs(ly) > fabs(y)) y = ly;

    return y;
}

float Input::ApplyDeadZone(float v) {
    float dead = 0.2f;
    return (fabs(v) < dead) ? 0.0f : v;
}

void Input::SetConfig(const Config& cfg) {
    config = cfg;
}