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
	goBack = false;

	option.LoadOption(config);

	int x = 100;
	int y = 100;
	int line = 60;

	auto Title_text = make_shared<Label>(Pos{ 100,100 }, 0, 0, "OPTION", WHITE);

	// --- Audio ---
	auto master_text = make_shared<Label>(Pos{ x , y + line * 1 }, 0, 0, "Master Volume", WHITE);
	auto master = make_shared<Slider>(Pos{ x + 200, y + line * 1 }, 300, 20);

	auto bgm_text = make_shared<Label>(Pos{ x , y + line * 2 }, 0, 0, "BGM Volume", WHITE);
	auto bgm = make_shared<Slider>(Pos{ x + 200, y + line * 2 }, 300, 20);

	auto se_text = make_shared<Label>(Pos{ x , y + line * 3 }, 0, 0, "SE Volume", WHITE);
	auto se = make_shared<Slider>(Pos{ x + 200, y + line * 3 }, 300, 20);


	// 初期値反映
	master->SetValue(config.masterVolume / 100.0f);
	bgm->SetValue(config.BGMVolume / 100.0f);
	se->SetValue(config.SEVolume / 100.0f);

	// --- Camera ---
	auto invertY = make_shared<Toggle>(Pos{ x + 200, y + line * 5 }, 50, 30);
	auto invertX = make_shared<Toggle>(Pos{ x + 200, y + line * 6 }, 50, 30);

	auto invertY_text = make_shared<Label>(Pos{ x , y + line * 5 }, 0, 0, "Invert Y", WHITE);
	auto invertX_text = make_shared<Label>(Pos{ x , y + line * 6 }, 0, 0, "Invert X", WHITE);

	invertY->Set(config.CamelaUpDownFlip);
	invertX->Set(config.CamelaRightLeftFlip);

	// --- Saveボタン ---
	auto saveBtn = make_shared<Button>(Pos{ x + 100, y + line * 8 }, 150, 50);
	saveBtn->SetOnClick([this, master, bgm, se, invertY, invertX]() {

		config.masterVolume = (int)(master->GetValue() * 100);
		config.BGMVolume = (int)(bgm->GetValue() * 100);
		config.SEVolume = (int)(se->GetValue() * 100);

		config.CamelaUpDownFlip = invertY->Get();
		config.CamelaRightLeftFlip = invertX->Get();

		option.SaveOption(config);

		printfDx("Saved!\n");
	});

	auto save_text = make_shared<Label>(Pos{ x + 150, y + line * 8 + 20 }, 0, 0, "Save", WHITE);

	// --- Backボタン ---
	auto backBtn = make_shared<Button>(Pos{ x + 300, y + line * 8 }, 150, 50);
	backBtn->SetOnClick([this]() {
		goBack = true;
	});

	auto back_text = make_shared<Label>(Pos{ x + 350, y + line * 8 + 20 }, 0, 0, "Back", WHITE);

	uiManager.Add(Title_text);

	uiManager.Add(master_text);
	uiManager.Add(master);

	uiManager.Add(bgm_text);
	uiManager.Add(bgm);

	uiManager.Add(se_text);
	uiManager.Add(se);

	uiManager.Add(invertY);
	uiManager.Add(invertX);

	uiManager.Add(invertY_text);
	uiManager.Add(invertX_text);

	uiManager.Add(saveBtn);
	uiManager.Add(save_text);

	uiManager.Add(backBtn);
	uiManager.Add(back_text);
}

void OptionMenu::Update(SceneManager& manager) {
	if (goBack) {
		manager.ChangeScene(
			make_unique<Title>(),
			make_unique<Slide>()
		);
	}

	uiManager.Update();
}

void OptionMenu::Draw() {
	int x = 100;
	int y = 100;
	int line = 60;

	int offset = uiManager.GetScrollY();

	uiManager.Draw();
}
