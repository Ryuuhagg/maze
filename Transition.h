#pragma once
enum class TransitionState {
	None,
	Enter,      // 演出開始
	Switching,  // 切り替えタイミング
	Exit,       // 演出終了中
	Finished
};

class Transition {
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual bool IsFinished() = 0;
	virtual TransitionState GetState() = 0;
};

class Fade : Transition{
private:
	int alpha = 0;
	bool fadeOut = true;
public:
	void Update() override;
	void Draw() override;
	bool IsFinished() override { return !fadeOut && alpha <= 0; }
	bool IsMidPoint() { return alpha >= 255; }
	TransitionState GetState() override;
};