//UI.cpp
#include"UI.h"
#include"DxLib.h"
#include"Input.h"

bool UI::IsMouseOver(int x, int y, int w, int h)
{
		int mx = Input::GetMouseX();
		int my = Input::GetMouseY();
		return (mx >= x && mx < x + w && my >= y && my <= y + h);
}
#pragma region Button
Button::Button(Pos pos, int c, int sizeX, int sizeY): UI(pos,c,sizeX,sizeY),clicked(false){}

void Button::Update() {
	clicked = IsClicked();
	bool hover = IsMouseOver(pos.x, pos.y, sizeX, sizeY);
	color = hover ? GetColor(200, 200, 255) : GetColor(150, 150, 255);
	
}

void Button::Draw() {
	DrawBox(pos.x, pos.y, pos.x + sizeX, pos.y + sizeY, color, true);
	if (clicked) {
		DrawString(200, 100, "CLICKED!", GetColor(255, 0, 0));
	}
}

bool Button::IsClicked() {
	return IsMouseOver(pos.x, pos.y, sizeX, sizeY)
		&& Input::IsActionTrigger(Action::Confirm);
}

bool Button::IsUsing() {
	return clicked;
}
#pragma endregion
#pragma region Toggle
Toggle::Toggle(Pos pos, int c, int sizeX, int sizeY) : UI(pos, c, sizeX, sizeY), isOn(false) {}

void Toggle::Update() {
	bool hover = IsMouseOver(pos.x, pos.y, sizeX, sizeY);
	color = hover ? GetColor(200, 200, 255) : GetColor(150, 150, 255);
	if (IsClicked()) {
		isOn = !isOn;
	}
}

void Toggle::Draw() {
	int drawColor = isOn ? GetColor(100, 255, 100) : GetColor(255, 100, 100);

	DrawBox(pos.x, pos.y, pos.x + sizeX, pos.y + sizeY, drawColor, TRUE);
}

bool Toggle::IsClicked() {
	return IsMouseOver(pos.x, pos.y, sizeX, sizeY)
		&& Input::IsActionTrigger(Action::Confirm);
}

bool Toggle::IsUsing() {
	return IsClicked();
}
#pragma endregion
#pragma region Slider
Slider::Slider(Pos pos, int w, int h):UI(pos, 0, w, h),dragging(false),value(1){}

void Slider::Update() {
	int mx = Input::GetMouseX();
	int my = Input::GetMouseY();

	int knobX = pos.x + (int)(value * sizeX);

	// ノブにマウスがあるか
	bool onKnob =
		(mx >= knobX - 8 && mx <= knobX + 8 &&
			my >= pos.y - 8 && my <= pos.y + sizeY + 8);
	bool onBar = IsMouseOver(pos.x, pos.y, sizeX, sizeY);
	// 押したらドラッグ開始
	if (Input::IsMousePressed(MOUSE_INPUT_LEFT) && (onKnob || onBar)) {
		dragging = true;
	}

	// 離したら終了
	if (!Input::IsMousePressed(MOUSE_INPUT_LEFT)) {
		dragging = false;
	}

	// ドラッグ中なら値更新
	if (dragging) {
		value = (float)(mx - pos.x) / sizeX;

		if (value < 0.0f) value = 0.0f;
		if (value > 1.0f) value = 1.0f;
	}
}

void Slider::Draw() {
	// ノブ位置
	int knobX = pos.x + (int)(value * sizeX);

	// バー
	DrawBox(pos.x, pos.y,
		knobX, pos.y + sizeY,
		GetColor(200, 200, 200), TRUE);

	DrawBox(knobX, pos.y,
		pos.x + sizeX, pos.y + sizeY,
		GetColor(100, 100, 100), TRUE);

	// ノブ
	DrawBox(knobX - 5, pos.y - 5,
		knobX + 5, pos.y + sizeY + 5,
		GetColor(255, 255, 255), TRUE);
}

bool Slider::IsUsing() {
	return dragging;
}
#pragma endregion