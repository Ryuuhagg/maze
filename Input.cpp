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
//アクションごとにキーを決定
bool Input::IsActionTrigger(Action action) {
    switch (action) {
    case Action::Confirm:
        return IsKeyTrigger(config.KeyConfirm) || IsPadTrigger(config.PadConfirm);

    case Action::Cancel:
        return IsKeyTrigger(config.KeyCancel) || IsPadTrigger(config.PadCancel);

    case Action::Jump:
        return IsKeyTrigger(config.KeyJump) || IsPadTrigger(config.PadJump);

    case Action::Up:
        return IsKeyTrigger(KEY_INPUT_UP);

    case Action::Down:
        return IsKeyTrigger(KEY_INPUT_DOWN);

    case Action::Right:
        return IsKeyTrigger(KEY_INPUT_RIGHT);

    case Action::Left:
        return IsKeyTrigger(KEY_INPUT_LEFT);
    }

    return false;
}
bool Input::IsActionPressed(Action action) {
    switch (action) {
    case Action::Dash:
        return IsKeyPressed(config.KeyDash) || IsPadPressed(config.PadDash);

    case Action::Up:
        return IsKeyPressed(KEY_INPUT_UP);

    case Action::Down:
        return IsKeyPressed(KEY_INPUT_DOWN);

    case Action::Right:
        return IsKeyPressed(KEY_INPUT_RIGHT);

    case Action::Left:
        return IsKeyPressed(KEY_INPUT_LEFT);

    case Action::MoveUp:
        return IsKeyPressed(config.KeyFront);

    case Action::MoveDown:
        return IsKeyPressed(config.KeyBack);

    case Action::MoveRight:
        return IsKeyPressed(config.KeyRight);

    case Action::MoveLeft:
        return IsKeyPressed(config.KeyLeft);
    }

    return false;
}
//許可されてるキー
bool Input::IsValidBindKey(int key) {
    return KeyToStringSafe(key) != "UNKNOWN";
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
    if (IsActionPressed(Action::MoveRight)) x += 1.0f;
    if (IsActionPressed(Action::MoveLeft)) x -= 1.0f;

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
    if (IsActionPressed(Action::MoveUp)) y += 1.0f;
    if (IsActionPressed(Action::MoveDown)) y -= 1.0f;

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
    if (IsActionPressed(Action::Right)) x += 1.0f;
    if (IsActionPressed(Action::Left)) x -= 1.0f;

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
    if (IsActionPressed(Action::Up)) y -= 1.0f;
    if (IsActionPressed(Action::Down)) y += 1.0f;

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

int Input::GetAnyKeyTrigger() {
    for (int i = 0; i < 256; i++) {
        if (now[i] && !prev[i]) {
            return i;
        }
    }
    return -1;
}

int Input::GetAnyPadTrigger() {
    int pad = GetJoypadInputState(DX_INPUT_PAD1);

    int diff = pad & (~padPrev);

    if (diff) return diff;

    return -1;
}