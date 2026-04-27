#pragma once
#include"Constant.h"

struct Pos {
	int x;
	int y;
};

class UI {
protected:
	Pos pos;
	int color;

	int sizeX;
	int sizeY;

	bool IsMouseOver(int x, int y, int w, int h);
public:
	UI(Pos pos, int c, int sizeX, int sizeY) : pos(pos), color(c), sizeX(sizeX),sizeY(sizeY){}
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

class Button : public UI {
public:
	Button(Pos pos, int c, int sizeX, int sizeY);
	void Update() override;
	void Draw() override;
	bool IsClicked();
};

class Toggle : public UI {
	bool isOn = false;
public:
	Toggle(Pos pos, int c, int sizeX, int sizeY);
	void Update() override;
	void Draw() override;

	bool IsClicked();
};

class Slider : public UI {
	float value;
	bool dragging;
public:
	Slider(Pos pos, int w, int h);

	void Update() override;
	void Draw() override;

	float GetValue() const { return value; }
};