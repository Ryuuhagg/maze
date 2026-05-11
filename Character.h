//Character.h
#pragma once
#include"DxLib.h"
#include"Model.h"
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

enum class PlayerState {
	Jump,
	Idle,
	Walk,
	Dash
};


class Player : public Character {
	int m_model = 0;
	int m_animeIndex = 0;
	int m_Size = 0;
	int m_currentAnimNo = -1;
	
	int m_animIdle = 1;
	int m_animWalk = 2;
	int m_animJump = 0;

	float y = 0;
	float vy = 0;
	float gravity = -0.01f;
	bool isGround = true;

	PlayerState m_state = PlayerState::Idle;
	Angle angle = { 0.0f, 0.3f };
	Angle C_angle = { 0.0f, 0.3f };
public:
	Player();
	~Player();

	void Init()override;
	void Update()override;
	void Draw() override;

	void Move();

	void MoveAngle();

	void Jump();

	void UpdateState();
	void UpdateAnimation();

	void ChangeModel(const ModelData& data);

	Angle getAngle() { return angle; }

	void GetAngle(Angle& a);
};

class Camela : public Character {
	Player& p;
	Angle camelaAngle = {0.0f, 0.3f };
	float distance;
public:
	Camela(Player& p);
	void Init()override;
	void Update()override;
	void Draw() override;

	void MoveAngle();
};