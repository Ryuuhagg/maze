#pragma once
class Input {
	static char now[256];
	static char prev[256];

	static int padNow;
	static int padPrev;
public:
	static void Update();

	static bool IsKeyPressed(int key) { return now[key]; }
	static bool IsKeyTrigger(int key) { return now[key] && !prev[key]; }

	static bool IsPadPressed(int button){ return padNow & button; }
	static bool IsPadTrigger(int button){ return (padNow & button) && !(padPrev & button); }
};