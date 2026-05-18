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

Title::Title():first(false) {
	Init();
}

void Title::Init() {
	changeFlg = 0;

	uiManager.SetScrollEnabled(false);

	auto title_text = make_shared<Label>(Pos{ 100, 100 }, 0, 0, "TITLE", WHITE, 32);

	auto gameBtn = make_shared<Button>(Pos{ 700, 400 }, 150, 50);
	gameBtn->SetOnClick([this]() {
		changeFlg = 1;
		});

	auto game_text = make_shared<Label>(Pos{ 750, 420 }, 0, 0, "遊ぶ", BLACK);

	auto createBtn = make_shared<Button>(Pos{ 700, 500 }, 150, 50);
	createBtn->SetOnClick([this]() {
		changeFlg = 2;
		});

	auto create_text = make_shared<Label>(Pos{ 750, 520 }, 0, 0, "作る", BLACK);

	auto optionBtn = make_shared<Button>(Pos{ 700, 600 }, 150, 50);
	optionBtn->SetOnClick([this]() {
		changeFlg = 3;
		});

	auto option_text = make_shared<Label>(Pos{ 750, 620 }, 0, 0, "設定", BLACK);

	uiManager.Add(title_text);

	uiManager.Add(gameBtn);
	uiManager.Add(game_text);

	uiManager.Add(createBtn);
	uiManager.Add(create_text);

	uiManager.Add(optionBtn);
	uiManager.Add(option_text);
}

void Title::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "タイトル画面",GetColor(255,255,255));
	uiManager.Draw();
}
void Title::Update(SceneManager& manager) {
	if (!first) {
		option.LoadOption(config);
		Input::SetConfig(config);
		first = true;
	}
	
	switch (changeFlg) {
	case 1:
		manager.ChangeScene(
			make_unique<Game>(),
			make_unique<Slide>()
		);
		break;
	case 2:
		manager.ChangeScene(
			make_unique<Create>(),
			make_unique<Slide>()
		);
		break;
	case 3:
		manager.ChangeScene(
			make_unique<OptionMenu>(),
			make_unique<Slide>()
		);
		break;
	}

	if (Input::IsActionTrigger(Action::Confirm)) {
		manager.ChangeScene(
			make_unique<OptionMenu>(), 
			make_unique<Slide>()
		);
	}

	uiManager.Update();
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
	uiManager.SetScrollEnabled(true);

	goBack = false;

	option.LoadOption(config);

	int x = 100;
	int currentY = 100;
	int line = 60;

	auto AddLine = [&]() {
		int result = currentY;
		currentY += line;
		return result;
		};

	int rowY;

	rowY = AddLine();

	auto Title_text = make_shared<Label>(Pos{ 100,rowY }, 0, 0, "OPTION", WHITE, 48);

	// --- Audio ---
	rowY = AddLine();
	rowY = AddLine();

	auto master_text = make_shared<Label>(Pos{ x , rowY }, 0, 0, "Master Volume", WHITE);
	auto master = make_shared<Slider>(Pos{ x + 200,rowY }, 300, 20);

	rowY = AddLine();

	auto bgm_text = make_shared<Label>(Pos{ x , rowY  }, 0, 0, "BGM Volume", WHITE);
	auto bgm = make_shared<Slider>(Pos{ x + 200, rowY }, 300, 20);

	rowY = AddLine();

	auto se_text = make_shared<Label>(Pos{ x , rowY }, 0, 0, "SE Volume", WHITE);
	auto se = make_shared<Slider>(Pos{ x + 200, rowY }, 300, 20);


	// 初期値反映
	master->SetValue(config.masterVolume / 100.0f);
	bgm->SetValue(config.BGMVolume / 100.0f);
	se->SetValue(config.SEVolume / 100.0f);

	// --- Camera ---

	rowY = AddLine();
	rowY = AddLine();

	auto invertY = make_shared<Toggle>(Pos{ x + 200, rowY }, 50, 30);
	auto invertY_text = make_shared<Label>(Pos{ x , rowY }, 0, 0, "Invert Y", WHITE);

	rowY = AddLine();

	auto invertX = make_shared<Toggle>(Pos{ x + 200, rowY }, 50, 30);
	auto invertX_text = make_shared<Label>(Pos{ x , rowY }, 0, 0, "Invert X", WHITE);



	invertY->Set(config.CamelaUpDownFlip);
	invertX->Set(config.CamelaRightLeftFlip);

	// --- キーコンフィグ ---
	rowY = AddLine();

	auto KeyCon_text = make_shared<Label>(Pos{ x , rowY }, 0, 0, "Key Config", WHITE,32);

	rowY = AddLine();

	auto jump_text = make_shared<Label>(Pos{ x , rowY }, 0, 0, "Jump", WHITE);
	auto jumpKey = make_shared<KeyBindButton>(Pos{ x + 200, rowY}, 150, 40, &config.KeyJump);

	rowY = AddLine();

	auto dash_text = make_shared<Label>(Pos{ x,rowY }, 0, 0, "Dash", WHITE);
	auto dashKey = make_shared<KeyBindButton>(Pos{ x + 200,rowY}, 150, 40, &config.KeyDash);

	

	// --- Saveボタン ---

	rowY = AddLine();

	auto saveBtn = make_shared<Button>(Pos{ x + 100, rowY }, 150, 50);
	saveBtn->SetOnClick([this, master, bgm, se, invertY, invertX]() {

		config.masterVolume = (int)(master->GetValue() * 100);
		config.BGMVolume = (int)(bgm->GetValue() * 100);
		config.SEVolume = (int)(se->GetValue() * 100);

		config.CamelaUpDownFlip = invertY->Get();
		config.CamelaRightLeftFlip = invertX->Get();

		option.SaveOption(config);

		printfDx("Saved!\n");
	});

	auto save_text = make_shared<Label>(Pos{ x + 150, rowY + 20 }, 0, 0, "Save", BLACK);

	// --- Backボタン ---

	auto backBtn = make_shared<Button>(Pos{ x + 300, rowY }, 150, 50);
	backBtn->SetOnClick([this]() {
		goBack = true;
	});

	auto back_text = make_shared<Label>(Pos{ x + 350, rowY + 20 }, 0, 0, "Back", BLACK);

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

	uiManager.Add(jumpKey);
	uiManager.Add(jump_text);

	uiManager.Add(dashKey);
	uiManager.Add(dash_text);

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
