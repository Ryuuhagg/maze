#include"Character.h"
#include<math.h>
Player::Player():Character(5)
{
    Init();
}

void Player::Init() {
    pos = VGet(0, 0, 0);
    m_Size = 10;
}

void Player::Update() {
    if (CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_UP)) {
        pos.x += sinf(angle.x) * speed;
        pos.z += cosf(angle.x) * speed;
    }
    if (CheckHitKey(KEY_INPUT_S) || CheckHitKey(KEY_INPUT_DOWN)) { 
        pos.x -= sinf(angle.x) * speed;
        pos.z -= cosf(angle.x) * speed;
    }
    if (CheckHitKey(KEY_INPUT_A) || CheckHitKey(KEY_INPUT_LEFT)) angle.x -= speed * 0.01f;
    if (CheckHitKey(KEY_INPUT_D) || CheckHitKey(KEY_INPUT_RIGHT)) angle.x += speed * 0.01f;

    

}

void Player::Draw() {
    DrawCube3D(
        VAdd(pos, VGet(-m_Size / 2, 0, -m_Size / 2)), // ç∂â∫éËëO
        VAdd(pos, VGet(m_Size / 2, m_Size, m_Size / 2)), // âEè„âú
        GetColor(255, 0, 0),
        GetColor(255, 0, 0),
        TRUE
    );
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