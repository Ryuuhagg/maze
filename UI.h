//UI.h
#pragma once
#include"Constant.h"
#include<functional>
#include<string>
using namespace std;
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

	bool IsMouseOver(int x, int y, int w, int h, int offsetY);
	bool isFocused = false;
public:
	UI(Pos pos, int c, int sizeX, int sizeY) : pos(pos), color(c), sizeX(sizeX),sizeY(sizeY){}
	virtual void Update(int offsetY) = 0;
	virtual void Draw(int offsetY) = 0;

	virtual bool IsUsing() { return false; }
	virtual void OnClick(){}

	Pos GetPos() const { return pos; }
	void SetPos(const Pos& p) { pos = p; }

	void SetFocus(bool f) { isFocused = f; }
	bool IsFocused() const { return isFocused; }
	virtual bool IsSelectable() { return true; }
	bool IsMouseOverSelf(int offsetY) {
		return IsMouseOver(pos.x, pos.y, sizeX, sizeY, offsetY);
	}
};

class Button : public UI {
	bool clicked;

	function<void()> onClick;
public:
	Button(Pos pos, int sizeX, int sizeY);
	void Update(int offsetY) override;
	void Draw(int offsetY) override;
	bool IsClicked(int offsetY);

	bool IsUsing()override;

	void SetOnClick(std::function<void()> func);

	void OnClick()override;
};

class Toggle : public UI {
	bool isOn = false;
	bool usingNow = false;

	string label;
public:
	Toggle(Pos pos, int sizeX, int sizeY);
	void Update(int offsetY) override;
	void Draw(int offsetY) override;

	void OnClick()override;

	bool IsClicked(int offsetY);
	bool IsUsing()override;

	void Set(bool v) { isOn = v; }
	bool Get() const { return isOn; }
};

class Slider : public UI {
	float value;
	bool dragging;
public:
	Slider(Pos pos, int w, int h);

	void Update(int offsetY) override;
	void Draw(int offsetY) override;

	float GetValue() const { return value; }

	bool IsUsing()override;

	void SetValue(float v) {
		value = v;
	}
};

class KeyBindButton : public UI {
	bool waitingInput = false;
	int* targetKey; // ConfigÇÃÉLÅ[Çíºê⁄éwÇ∑

public:
	KeyBindButton(Pos pos, int w, int h, int* keyPtr);

	void Update(int offsetY) override;

	void Draw(int offsetY)  override;

	void OnClick()override;
};

class Label : public UI {
	string text;
public:
	Label(Pos pos, int sizeX, int sizeY, string text, int color);
	void Update(int offsetY)override;
	void Draw(int offsetY) override;

	bool IsSelectable() override { return false; }
};