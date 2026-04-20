#pragma once

enum class Action {
	Confirm,
	Cancel,
	Jump
};

class Input {
	static char now[256];
	static char prev[256];

	static int padNow;
	static int padPrev;
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
};