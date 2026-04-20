#include"Scene.h"
#include"Constant.h"
#include"SceneManager.h"
#include"Character.h"
#include"DxLib.h"
using namespace std;
Player p;
Camela c(p);
void Title::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "タイトル画面",GetColor(255,255,255));
}
void Title::Update(SceneManager& manager) {
	if (CheckHitKey(KEY_INPUT_SPACE)) {
		manager.ChangeScene(
			make_unique<Game>(), 
			make_unique<Fade>()
		);
	}
}
void Game::Draw() {
	p.Draw();
	DrawString(WIDTH / 2, HEIGHT-550, "ゲーム画面", GetColor(255, 255, 255));

	DrawCube3D(
		VGet(-50, -1, -50),
		VGet(50, 0, 50),
		GetColor(0, 255, 0),
		GetColor(0, 255, 0),
		TRUE
	);
}
void Game::Update(SceneManager& manager) {
	p.Update();
	c.Update();
	if (CheckHitKey(KEY_INPUT_SPACE)) {
		//manager.ChangeScene(std::make_unique<Result>());
	}
}
void Result::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "リザルト画面", GetColor(255, 255, 255));
}
void Result::Update(SceneManager& manager) {
	if (CheckHitKey(KEY_INPUT_SPACE)) {
		manager.ChangeScene(std::make_unique<Title>());
	}
}