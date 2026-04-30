//Character.cpp
#include"Character.h"
#include"Input.h"
#include"Constant.h"
#include<math.h>
#include <algorithm>
#include"MapLoader.h"
Player::Player():Character(10)
{
    Init();
}

Player::~Player() {
    MV1DeleteModel(m_model);
}

void Player::Init() {
    ChangeModel(cat);

    MV1SetScale(m_model, VGet(0.5f, 0.5f, 0.5f));

    pos = GetStartPosition();
    y = 0;
    vy = 0;
    isGround = true;
    gravity = -0.1f;

    m_Size = 10;
    angle = { 0.0f, 1.5f };
}

void Player::Update() {
    
    Move();
    //MoveAngle();
    Jump();

    UpdateState();
    UpdateAnimation();

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

    moveX += sinf(C_angle.x) * y;
    moveZ += cosf(C_angle.x) * y;

    moveX += cosf(C_angle.x) * x;
    moveZ -= sinf(C_angle.x) * x;

    float length = sqrtf(moveX * moveX + moveZ * moveZ);
    if (length > 1.0f) {
        moveX /= length;
        moveZ /= length;
    }

    if (length > 0.1f) {
        angle.x = atan2f(moveX, moveZ);
    }

    pos.x += moveX * speed;
    pos.z += moveZ * speed;
}

void Player::MoveAngle() {
    float rx = Input::GetAxisRX();
    float ry = Input::GetAxisRY();

    angle.x += rx * 0.01f;
    angle.y += ry * 0.01f;

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

void Player::UpdateState() {
    float x = Input::GetAxisLX();
    float y = Input::GetAxisLY();

    if (!isGround) {
        m_state = PlayerState::Jump;
    }
    else if (fabs(x) > 0.1f || fabs(y) > 0.1f) {
        m_state = PlayerState::Walk;
    }
    else {
        m_state = PlayerState::Idle;
    }
}

void Player::UpdateAnimation() {
    int nextAnim = 0;

    switch (m_state) {
    case PlayerState::Jump: nextAnim = 0; break;
    case PlayerState::Idle: nextAnim = 1; break;
    case PlayerState::Walk: nextAnim = 2; break;
    }

    if (nextAnim != m_currentAnimNo) {

        MV1DetachAnim(m_model, m_animeIndex);

        m_animeIndex = MV1AttachAnim(m_model, nextAnim);
        MV1SetAttachAnimBlendRate(m_model, m_animeIndex, 1.0f);

        m_currentAnimNo = nextAnim;
    }

    float now = MV1GetAttachAnimTime(m_model, m_animeIndex);
    float total = MV1GetAttachAnimTotalTime(m_model, m_animeIndex);

    now += 0.5f;
    now = fmod(now, total);

    MV1SetAttachAnimTime(m_model, m_animeIndex, now);
}

void Player::ChangeModel(const ModelData& data) {

    if (m_model != 0) {
        MV1DeleteModel(m_model);
    }

    m_model = MV1LoadModel(data.path);

    m_animIdle = data.idle;
    m_animWalk = data.walk;
    m_animJump = data.jump;

    m_animeIndex = MV1AttachAnim(m_model, m_animIdle);
    m_currentAnimNo = m_animIdle;
}

void Player::GetAngle(Angle& a) { C_angle = a; }

Camela::Camela(Player& p) :Character(0),p(p),distance(180)
{
    Init();
}

void Camela::Init() {
    camelaAngle = { 0.0f, 0.3f };
}

void Camela::Update() {
    p.GetAngle(camelaAngle);
    VECTOR camPos = VGet(
        p.getVECTOR().x - sinf(camelaAngle.x) * distance,
        p.getVECTOR().y + sinf(camelaAngle.y) * distance,
        p.getVECTOR().z - cosf(camelaAngle.x) * distance
    );
    SetLightPosition(camPos);

    VECTOR target = p.getVECTOR();
    VECTOR dir = VSub(target, camPos);

    // ê≥ãKâª
    float len = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    dir = VGet(dir.x / len, dir.y / len, dir.z / len);

    SetLightDirection(dir);

    SetCameraPositionAndTarget_UpVecY(
        VGet(
          p.getVECTOR().x - sinf(camelaAngle.x) * distance
        , p.getVECTOR().y + sinf(camelaAngle.y) * distance
        , p.getVECTOR().z - cosf(camelaAngle.x) * distance)
        , p.getVECTOR()
    );
    MoveAngle();
}

void Camela::Draw() {

}

void Camela::MoveAngle() {
    float rx = Input::GetAxisRX();
    float ry = Input::GetAxisRY();

    camelaAngle.x += rx * 0.03f;
    camelaAngle.y += ry * 0.03f;

    camelaAngle.y = Clamp(camelaAngle.y, -0.0f, 1.5f);
}