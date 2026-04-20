#include"Character.h"
#include"Input.h"
#include<math.h>
Player::Player():Character(5)
{
    Init();
}

void Player::Init() {
    pos = VGet(0, 0, 0);
    m_Size = 10;
    angle = { 0.0f, 0.3f };
}

void Player::Update() {
    Move();
}

void Player::Draw() {
    DrawCube3D(
        VAdd(pos, VGet(-m_Size / 2, 0, -m_Size / 2)), // ¶‰ºŽè‘O
        VAdd(pos, VGet(m_Size / 2, m_Size, m_Size / 2)), // ‰Eã‰œ
        GetColor(255, 0, 0),
        GetColor(255, 0, 0),
        TRUE
    );
}

void Player::Move() {
    float x = Input::GetAxisX();
    float y = Input::GetAxisY();

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


void Player::MouseMove() {
    int mouseX, mouseY;
    GetMousePoint(&mouseX, &mouseY);

    int prevX = mouseX;
    int prevY = mouseY;

    int dx = mouseX - prevX;
    int dy = mouseY - prevY;

    prevX = mouseX;
    prevY = mouseY;

    angle.x += dx * sensitivity;
    angle.y += dy * sensitivity;

    
}

Camela::Camela(Player& p) :Character(0),p(p),distance(90.0f)
{
    Init();
}

void Camela::Init() {
    
}

void Camela::Update() {
    SetCameraPositionAndTarget_UpVecY(
        VGet(
          p.getVECTOR().x - sinf(p.getAngle().x) * distance
        , p.getVECTOR().y + distance
        , p.getVECTOR().z - cosf(p.getAngle().x) * distance)
        , p.getVECTOR()
    );
}

void Camela::Draw() {

}