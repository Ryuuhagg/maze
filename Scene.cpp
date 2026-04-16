#include"Scene.h"
#include"Constant.h"
#include"SceneManager.h"
#include"DxLib.h"
void Title::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "タイトル画面",GetColor(255,255,255));
}
void Title::Update(SceneManager& manager) {
	if (CheckHitKey(KEY_INPUT_SPACE)) {
		manager.ChangeScene(std::make_unique<Game>());
	}
}
void Game::Draw() {
	DrawString(WIDTH / 2, HEIGHT / 2, "ゲーム画面", GetColor(255, 255, 255));
}
void Game::Update(SceneManager& manager) {
	if (CheckHitKey(KEY_INPUT_SPACE)) {
		manager.ChangeScene(std::make_unique<Result>());
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