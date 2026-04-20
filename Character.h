#pragma once
#include"DxLib.h"
class Character {
protected:
	float speed;
	VECTOR pos = VGet(0, 0, 0);
public:
	Character(float speed):speed(speed){}
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	VECTOR getVECTOR(){ return pos; };
};

struct Angle {
	float x;
	float y;
};

class Player : public Character {
	int m_Size = 0;
	Angle angle = { 0.0f, 0.3f };
	float sensitivity = 0.005f;
public:
	Player();
	void Init()override;
	void Update()override;
	void Draw() override;

	void MouseMove();

	Angle getAngle() { return angle; }
};

class Camela : public Character {
	Player& p;
	float distance;
public:
	Camela(Player& p);
	void Init()override;
	void Update()override;
	void Draw() override;
};