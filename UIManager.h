//UIManager.h
#pragma once
#include<vector>
#include<memory>
#include"UI.h"
using namespace std;
class UIManager {
	vector<std::shared_ptr<UI>> uiList;
public:
	void Add(const std::shared_ptr<UI>& ui);
	void Update();
	void Draw();
	void Clear();
};