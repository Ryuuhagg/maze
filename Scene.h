//Scene.h
#pragma once
#include"UIManager.h"
class SceneManager;

class Scene {
public:
	virtual void Init() = 0;
	virtual void Update(SceneManager& manager) = 0;
	virtual void Draw() = 0;
};

class Title :public Scene {
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
	UIManager uiManager;
public:
	OptionMenu();
	void Init()override;
	void Update(SceneManager& maneger)override;
	void Draw()override;
};