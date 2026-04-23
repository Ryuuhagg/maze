#include"Character.h"
#include"Input.h"
#include"Constant.h"
#include<math.h>
#include <algorithm>
Player::Player():Character(5)
{
    Init();
}

Player::~Player() {
    MV1DeleteModel(m_model);
}

void Player::Init() {
    m_model = MV1LoadModel("mv1Model/animal-beaver.mv1");
    MV1SetScale(m_model, VGet(0.05f, 0.05f, 0.05f));
    pos = VGet(0, 0, 0);
    y = 0;             
    vy = 0;             
    isGround = true;    
    gravity = -0.1f;   

    m_Size = 10;
    angle = { 0.0f, 1.5f };

}

void Player::Update() {
    Move();
    MoveAngle();
    Jump();

    MV1SetPosition(m_model, pos);
    MV1SetRotationXYZ(m_model, VGet(0, angle.x + DX_PI_F, 0));

}

void Player::Draw() {
    MV1DrawModel(m_model);
}

void Player::Move() {
    float x = Input::GetAxisLX();
    float y = Input::GetAxisLY();

    float moveX = 0;
    float moveZ = 0;

    moveX += sinf(angle.x) * y;
    moveZ += cosf(angle.x) * y;

    moveX += cosf(angle.x) * x;
    moveZ -= sinf(angle.x) * x;

    float length = sqrtf(moveX * moveX + moveZ * moveZ);
    if (length > 1.0f) {
        moveX /= length;
        moveZ /= length;
    }

    pos.x += moveX * speed;
    pos.z += moveZ * speed;
}

void Player::MoveAngle() {
    float rx = Input::GetAxisRX();
    float ry = Input::GetAxisRY();

    angle.x += rx * 0.05f;
    angle.y += ry * 0.05f;

    angle.y = Clamp(angle.y, -0.0f, 1.5f);
}

void Player::Jump() {
    if (Input::IsActionTrigger(Action::Jump) && isGround) {
        vy = 3.0f;
        isGround = false;
    }

    vy += gravity;
    y += vy;

    if (y <= 0) {
        y = 0;
        vy = 0;
        isGround = true;
    }

    pos.y = y;
}

Camela::Camela(Player& p) :Character(0),p(p),distance(60)
{
    Init();
}

void Camela::Init() {

}

void Camela::Update() {
    SetCameraPositionAndTarget_UpVecY(
        VGet(
          p.getVECTOR().x - sinf(p.getAngle().x) * distance
        , p.getVECTOR().y + sinf(p.getAngle().y) * distance
        , p.getVECTOR().z - cosf(p.getAngle().x) * distance)
        , p.getVECTOR()
    );
}

void Camela::Draw() {

}