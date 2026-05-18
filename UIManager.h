//UIManager.h
#pragma once
#include<vector>
#include<memory>
#include"UI.h"
using namespace std;
class UIManager {
	vector<std::shared_ptr<UI>> uiList;
	int scrollY = 0;
	int focusIndex = 0;
	bool enableScroll = true;
public:
	void Add(const std::shared_ptr<UI>& ui);
	void Update();
	void Draw();
	void Clear();
	int GetScrollY() { return scrollY; }
	void SetScrollEnabled(bool flag) { enableScroll = flag; }
};