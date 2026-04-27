#include"Scene.h"
#include"Constant.h"
#include"SceneManager.h"
#include"Character.h"
#include"Input.h"
#include"UI.h"
#include"DxLib.h"
using namespace std;
Player p;
Camela c(p);
Button b({ 100,100 }, 0,100,50);
Toggle t({ 100,200 }, 0,100,50);
Slider s({ 100,300 }, 100, 25);
void Title::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "タイトル画面",GetColor(255,255,255));
}
void Title::Update(SceneManager& manager) {
	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(
			make_unique<Game>(), 
			make_unique<Slide>()
		);
		p.Init();
	}
}
void Game::Draw() {
	p.Draw();
	DrawString(WIDTH / 2, HEIGHT-550, "ゲーム画面", GetColor(255, 255, 255));

	DrawCube3D(
		VGet(-500, -1, -500),
		VGet(500, 0, 500),
		GetColor(0, 255, 0),
		GetColor(0, 255, 0),
		TRUE
	);
}
void Game::Update(SceneManager& manager) {
	p.Update();
	c.Update();   
	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(make_unique<Result>(), make_unique<Fade>());
	}
}
void Result::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "リザルト画面", GetColor(255, 255, 255));
}
void Result::Update(SceneManager& manager) {
	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(std::make_unique<Title>());
	}
}

void Create::Update(SceneManager& manager) {

}

void Create::Draw() {

}

void OptionMenu::Update(SceneManager& manager) {
	b.Update();
	t.Update();
	s.Update();
}

void OptionMenu::Draw() {
	b.Draw();
	t.Draw();
	s.Draw();
}
