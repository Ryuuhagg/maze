#pragma once
#include"Constant.h"
enum class TransitionState {
	None,
	Enter,      // 演出開始
	Switching,  // 切り替えタイミング
	Exit,       // 演出終了中
	Finished
};

class Transition {
protected:
	bool Enter = true;
	bool Switched = false;
	bool Exit = false;
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual bool IsFinished() = 0;
	virtual TransitionState GetState() = 0;
};

class Fade : public Transition{
private:
	int alpha = 0;
	
public:
	void Update() override;
	void Draw() override;
	bool IsFinished() override { return !Enter && alpha <= 0; }
	bool IsMidPoint() { return alpha >= 255; }
	TransitionState GetState() override;
};

class Slide : public Transition {
private:
	int current = -WIDTH;
public:
	void Update() override;
	void Draw() override;
	bool IsFinished() override { return current <= WIDTH; }
	TransitionState GetState() override;
};