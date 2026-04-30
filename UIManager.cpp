//UIManager.cpp
#include"UIManager.h"

void UIManager::Add(const std::shared_ptr<UI>& ui) {
    uiList.push_back(ui);
}

void UIManager::Update() {
    for (auto it = uiList.rbegin(); it != uiList.rend(); ++it) {
        (*it)->Update();

        if ((*it)->IsUsing()) {
            break;
        }
    }
}

void UIManager::Draw() {
    for (auto& ui : uiList) {
        ui->Draw();
    }
}

void UIManager::Clear() {
    uiList.clear();
}