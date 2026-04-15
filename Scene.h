#pragma once
class Scene {
public:
	virtual void Update() = 0;
	virtual void Draw() = 0;
};

class Title :Scene {
public:
	void Update() override;
	void Draw() override;
};
class Game :Scene {
public:
	void Update() override;
	void Draw() override;
};
class Result :Scene {
public:
	void Update() override;
	void Draw() override;
};