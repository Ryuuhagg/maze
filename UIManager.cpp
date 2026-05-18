//UIManager.cpp
#include"UIManager.h"
#include"Input.h"
#include"Dxlib.h"
void UIManager::Add(const std::shared_ptr<UI>& ui) {
    uiList.push_back(ui);

    if (uiList.size() == 1 || !uiList[focusIndex]->IsSelectable()) {
        for (int i = 0; i < uiList.size(); i++) {
            if (uiList[i]->IsSelectable()) {
                focusIndex = i;
                break;
            }
        }
    }
}

void UIManager::Update() {
    // マウスホイール
    bool anyUsing = false;
    for (auto& ui : uiList) {
        if (ui->IsUsing()) {
            anyUsing = true;
            break;
        }
    }

    if (enableScroll&&!anyUsing) {
        scrollY += GetMouseWheelRotVol() * -20;
    }

    // クランプ（制限）
    if (scrollY < 0) scrollY = 0;
    if (scrollY > 500) scrollY = 500; // ← 適当に最大値

    // ↓キー
    if (Input::IsActionTrigger(Action::Down)) {
        int start = focusIndex;

        do {
            focusIndex++;
            if (focusIndex >= uiList.size()) focusIndex = 0;

            if (focusIndex == start) break;
        } while (!uiList[focusIndex]->IsSelectable());
    }

    // ↑キー
    if (Input::IsActionTrigger(Action::Up)) {
        int start = focusIndex;

        do {
            focusIndex--;
            if (focusIndex < 0) focusIndex = uiList.size() - 1;

            if (focusIndex == start) break;
        } while (!uiList[focusIndex]->IsSelectable());
    }

    // フォーカス設定
    for (int i = 0; i < uiList.size(); i++) {
        uiList[i]->SetFocus(i == focusIndex);
    }

    if (enableScroll&&!uiList.empty()) {
        int y = uiList[focusIndex]->GetPos().y;

        const int viewHeight = 400;
        const int margin = 80;

        int targetScroll = scrollY;

        if (y < scrollY + margin) {
            targetScroll = y - margin;
        }
        else if (y > scrollY + viewHeight - margin) {
            targetScroll = y - viewHeight + margin;
        }

        if (targetScroll < 0) {
            targetScroll = 0;
        }

        // 補間（なめらか）
        scrollY += (targetScroll - scrollY) * 0.2f;
    }

    // 決定キー
    if (Input::IsActionTrigger(Action::Confirm)) {
        if (uiList[focusIndex]->IsSelectable()) {
            uiList[focusIndex]->OnClick();
        }
    }

    for (auto it = uiList.rbegin(); it != uiList.rend(); ++it) {
        (*it)->Update(scrollY);

        if ((*it)->IsUsing()) {
            break;
        }
    }

    if (!anyUsing && Input::IsMouseMoved()) {
        for (int i = 0; i < uiList.size(); i++) {
            if (uiList[i]->IsSelectable() && uiList[i]->IsMouseOverSelf(scrollY)) {

                if (abs(i - focusIndex) <= 5) {
                    focusIndex = i;
                }

                break;
            }
        }
    }
}

void UIManager::Draw() {
    for (auto& ui : uiList) {
        ui->Draw(scrollY);
    }
}

void UIManager::Clear() {
    uiList.clear();
}