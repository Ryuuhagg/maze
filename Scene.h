//Scene.h
#pragma once
#include"UIManager.h"
#include"Option.h"
#include"Input.h"
class SceneManager;

class Scene {
protected:
	UIManager uiManager;
public:
	virtual void Init() = 0;
	virtual void Update(SceneManager& manager) = 0;
	virtual void Draw() = 0;
};

class Title :public Scene {
	Config config;
	Option option;

	int changeFlg;

	bool first;
public:
	Title();
	void Init() override;
	void Update(SceneManager& manager) override;
	void Draw() override;
};
class Game :public Scene {
public:
	Game();
	void Init() override;
	void Update(SceneManager& manager) override;
	void Draw() override;
};
class Result :public Scene {
public:
	Result();
	void Init() override;
	void Update(SceneManager& manager) override;
	void Draw() override;
};

class Create : public Scene {
public:
	Create();
	void Init()override;
	void Update(SceneManager& manager) override;
	void Draw() override;
};

class OptionMenu : public Scene {
	Config config;
	Option option;
	bool goBack = false;
public:
	OptionMenu();
	void Init()override;
	void Update(SceneManager& maneger)override;
	void Draw()override;
};