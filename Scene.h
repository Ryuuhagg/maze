//Scene.h
#pragma once
class SceneManager;
class Scene {
public:
	virtual void Update(SceneManager& manager) = 0;
	virtual void Draw() = 0;
};

class Title :public Scene {
public:
	void Update(SceneManager& manager) override;
	void Draw() override;
};
class Game :public Scene {
public:
	void Update(SceneManager& manager) override;
	void Draw() override;
};
class Result :public Scene {
public:
	void Update(SceneManager& manager) override;
	void Draw() override;
};

class Create : public Scene {
public:
	void Update(SceneManager& manager) override;
	void Draw() override;
};

class OptionMenu : public Scene {
public:
	void Update(SceneManager& maneger)override;
	void Draw()override;
};