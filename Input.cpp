#include"Input.h"
#include"DxLib.h"
#include <cmath>

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

bool Input::IsActionTrigger(Action action) {
    switch (action) {
    case Action::Confirm:
        return IsKeyTrigger(KEY_INPUT_RETURN) || IsPadTrigger(PAD_INPUT_B);

    case Action::Cancel:
        return IsKeyTrigger(KEY_INPUT_ESCAPE) || IsPadTrigger(PAD_INPUT_A);

    case Action::Jump:
        return IsKeyTrigger(KEY_INPUT_SPACE) || IsPadTrigger(PAD_INPUT_X);
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