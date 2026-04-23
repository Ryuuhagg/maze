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
	int m_model = 0;
	int m_Size = 0;
	Angle angle = { 0.0f, 0.3f };
	float y = 0;
	float vy = 0;
	float gravity = -0.01f;
	bool isGround = true;
public:
	Player();
	~Player();

	void Init()override;
	void Update()override;
	void Draw() override;

	void Move();

	void MoveAngle();

	void Jump();

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