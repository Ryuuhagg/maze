//Scene.cpp
#include"Scene.h"
#include"Constant.h"
#include"SceneManager.h"
#include"MapLoader.h"
#include"Character.h"
#include"Input.h"
#include"UI.h"
#include"UIManager.h"
#include"DxLib.h"
#include"MapEditor.h"
using namespace std;
Player p;
Camela c(p);

Title::Title() {
	Init();
}

void Title::Init() {

}

void Title::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "タイトル画面",GetColor(255,255,255));
}
void Title::Update(SceneManager& manager) {
	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(
			make_unique<Game>(), 
			make_unique<Slide>()
		);
	}
}

Game::Game() {
	Init();
}

void Game::Init() {
	InitGameMap();
	p.Init();
}

void Game::Draw() {
	p.Draw();
	DrawString(WIDTH / 2, HEIGHT-550, "ゲーム画面", GetColor(255, 255, 255));
	DrawGameMap();

}

void Game::Update(SceneManager& manager) {
	p.Update();
	c.Update();   
	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(make_unique<Result>(), make_unique<Fade>());
	}
	UpdateGameMap();
}

Result::Result() {
	Init();
}

void Result::Init() {

}

void Result::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "リザルト画面", GetColor(255, 255, 255));
}
void Result::Update(SceneManager& manager) {
	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(std::make_unique<Title>());
	}
}

Create::Create() {
	Init();
}

void Create::Init() {
	InitEditor();
}

void Create::Update(SceneManager& manager) {
	UpdateEditor();
}

void Create::Draw() {
	DrawEditor();
}

OptionMenu::OptionMenu() {
	Init();
}

void OptionMenu::Init() {
	auto button = make_shared<Button>(
		Pos{ 100,100 },
		200, 50
	);

	button->SetOnClick([this]() {
		printfDx("押された\n");
		});

	auto BGMslider = make_shared<Slider>(
		Pos{ 100,200 },
		200, 30
	);

	auto SEslider = make_shared<Slider>(
		Pos{ 100,300 },
		200, 30
	);

	auto toggle = make_shared<Toggle>(
		Pos{ 100,400 },
		200, 50
	);

	uiManager.Add(button);
	uiManager.Add(BGMslider);
	uiManager.Add(SEslider);
	uiManager.Add(toggle);
}

void OptionMenu::Update(SceneManager& manager) {
	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(
			make_unique<Title>(),
			make_unique<Slide>()
		);
	}
	uiManager.Update();
}

void OptionMenu::Draw() {
	uiManager.Draw();
}
