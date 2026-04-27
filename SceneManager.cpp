//SceneManager.cpp
#include"SceneManager.h"

void SceneManager::Update() {
	if (!transition && next) {
		current = std::move(next);
	}
	if (transition) {
		transition->Update();
		if (transition->GetState() == TransitionState::Switching && next) {
			current = std::move(next);
		}

		if (transition->GetState() == TransitionState::Finished) {
			transition.reset();
		}
		return;
	}
	if (current) current->Update(*this);
}

void SceneManager::Draw() {
	if (current) current->Draw();
	if (transition)transition->Draw();
}
void SceneManager::ChangeScene(unique_ptr<Scene> newScene,
							   unique_ptr<Transition> trans) {
	next = move(newScene);
	transition = move(trans);
}