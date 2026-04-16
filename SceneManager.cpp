#include"SceneManager.h"
void SceneManager::Update() {
	if (transition) {
		transition->Update();
		if (transition->IsMidPoint() && next) {
			current = move(next);
		}
		if (transition->IsFinished()) {
			transition.reset();
		}
		return;
	}
	if (current) current->Update();
}

void SceneManager::Draw() {
	if (current) current->Draw();
}
void SceneManager::ChangeScene(unique_ptr<Scene> newScene) {
	next = move(newScene);
}