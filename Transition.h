#pragma once
class Transition {
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual bool IsFinished() = 0;
	virtual bool IsMidPoint() = 0;
};

class Fade : Transition{
private:
	int alpha = 0;
	bool fadeOut = true;
public:
	void Update() override;
	void Draw() override;
	bool IsFinished() override { return !fadeOut && alpha <= 0; }
	bool IsMidPoint() override { return alpha >= 255; }
};