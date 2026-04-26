#include"UI.h"
#include"DxLib.h"
#include"Input.h"

Button::Button(Pos pos, int c): UI(pos,c), sizeX(100),sizeY(50){}

void Button::Update() {
	bool hover = IsMouseOver(pos.x, pos.y, sizeX, sizeY);
	color = hover ? GetColor(200, 200, 255) : GetColor(150, 150, 255);
	if (IsClicked()) {
		DrawString(100, 200, "CLICKED!", GetColor(255, 0, 0));
	}
}

void Button::Draw() {
	DrawBox(pos.x, pos.y, pos.x + sizeX, pos.y + sizeY, color, true);
}

bool Button::IsMouseOver(int x, int y, int width, int height) {
	int mx = Input::GetMouseX();
	int my = Input::GetMouseY();
	return(mx >= x && mx < x + width && my >= y && my <= y + height);
}

bool Button::IsClicked() {
	return IsMouseOver(pos.x, pos.y, sizeX, sizeY)
		&& Input::IsActionTrigger(Action::Confirm);
}

Toggle::Toggle(Pos pos, int c) : UI(pos, c), sizeX(100), sizeY(50), isOn(false) {}

void Toggle::Update() {

}

void Toggle::Draw() {

}

bool ToggleIsMouseOver(int x, int y, int width, int height) {
	int mx = Input::GetMouseX();
	int my = Input::GetMouseY();
	return(mx >= x && mx < x + width && my >= y && my <= y + height);
}

bool Toggle::IsClicked() {
	return IsMouseOver(pos.x, pos.y, sizeX, sizeY)
		&& Input::IsActionTrigger(Action::Confirm);
}