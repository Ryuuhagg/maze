//Input.h
#pragma once
#include"Option.h"

enum class Action {
	Confirm,
	Cancel,
	Jump,
	Dash,
	Up,
	Down,
	Left,
	Right,
	MoveUp,
	MoveDown,
	MoveLeft,
	MoveRight
};

class Input {
	static Config config;

	static char now[256];
	static char prev[256];

	static int padNow;
	static int padPrev;

	static float prevLX ;
	static float prevLY ;

	static int mouseNow;
	static int mousePrev;

	static int mouseX, mouseY;
	static int prevMouseX, prevMouseY;

public:
	static void Update();
	//押してる間発動
	static bool IsKeyPressed(int key) { return now[key]; }
	//押した瞬間だけ発動
	static bool IsKeyTrigger(int key) { return now[key] && !prev[key]; }
	//押してる間発動
	static bool IsPadPressed(int button){ return padNow & button; }
	//押した瞬間だけ発動
	static bool IsPadTrigger(int button){ return (padNow & button) && !(padPrev & button); }
	//アクションをPADとキーボードで共通させる
	static bool IsActionTrigger(Action action);
	static bool IsActionPressed(Action action);

	static bool IsValidBindKey(int key);
	// 押している間
	static bool IsMousePressed(int button) { return mouseNow & button; }
	// 押した瞬間
	static bool IsMouseTrigger(int button) { return (mouseNow & button) && !(mousePrev & button); }

	static bool IsMouseMoved() {
		return mouseX != prevMouseX || mouseY != prevMouseY;
	}

	static int GetMouseX() { return mouseX; }
	static int GetMouseY() { return mouseY; }

	static int GetMouseDeltaX() { return mouseX - prevMouseX; }
	static int GetMouseDeltaY() { return mouseY - prevMouseY; }

	//左スティック
	static float GetPadLX();
	static float GetPadLY();
	//右スティック
	static float GetPadRX();
	static float GetPadRY();

	static float GetAxisLX();
	static float GetAxisLY();

	static float GetAxisRX();
	static float GetAxisRY();

	static float ApplyDeadZone(float v);

	static void SetConfig(const Config& cfg);

	static int GetAnyKeyTrigger();
	static int GetAnyPadTrigger();
};