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
public:
	UI(Pos pos, int c) : pos(pos), color(c){}
	virtual void Update() = 0;
	virtual void Draw() = 0;


};

class Button : public UI {
	int sizeX;
	int sizeY;
public:
	Button(Pos pos, int c);
	void Update() override;
	void Draw() override;
	bool IsMouseOver(int x,int y, int width, int height);
	bool IsClicked();
};

class Toggle : public UI {
	int sizeX;
	int sizeY;
	bool isOn = false;
public:
	Toggle(Pos pos, int c);
	void Update() override;
	void Draw() override;

	bool IsMouseOver(int x, int y, int width, int height);
	bool IsClicked();
};