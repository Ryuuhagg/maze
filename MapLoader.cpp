#include "MapLoader.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;


#pragma region ===== マップ配列 =====

int GameFloorMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameFloorRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int GameWallMapA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameWallMapB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameWallRotA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameWallRotB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int GameCornerMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameCornerRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int GameDecoMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameDecoRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int GameEventMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-13: エディターで保存した手動当たり判定をゲーム側で使うため追加。
int GameCollisionMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-13: エディターで保存した辺当たり判定をゲーム側で使うため追加。
int GameCollisionEdgeMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-13: 辺当たり判定の長さ/厚みをゲーム側で再現するため追加。
int GameCollisionEdgeScaleMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X][4];
int GameCollisionEdgeThicknessMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X][4];
// 2026-05-13: コーナー当たり判定の長さ/厚み/奥行をゲーム側で再現するため追加。
int GameCollisionCornerScaleMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameCollisionCornerThicknessMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int GameCollisionCornerOffsetMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-11: エディターで保存した手動当たり判定をゲーム側で使うため追加。
//int GameCollisionMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-11: エディターで保存した辺単位の当たり判定をゲーム側で使うため追加。
//int GameCollisionEdgeMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-11: エディターで伸縮した辺当たり判定の長さ倍率をゲーム側で使うため追加。
//int GameCollisionEdgeScaleMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X][4];
// 2026-05-11: エディターで編集した辺当たり判定の厚さ倍率をゲーム側で使うため追加。
//int GameCollisionEdgeThicknessMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X][4];
// 2026-05-13: エディターで編集したコーナー当たり判定の長さ・厚み・奥行をゲーム側で使うため追加。
//int GameCollisionCornerScaleMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
//int GameCollisionCornerThicknessMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
//int GameCollisionCornerOffsetMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
/*// 2026-05-11: エディターで編集した角用の辺当たり判定の厚さ倍率をゲーム側で使うため追加。
int CollisionCornerScaleMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-11: エディターで編集した角用の辺当たり判定の厚さ倍率をゲーム側で使うため追加。
int CollisionCornerThicknessMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];*/
#pragma endregion

#pragma region ===== モデル別当たり判定 =====

enum CollisionType
{
    COLL_NONE,
    COLL_CIRCLE,
    COLL_BOX,
    COLL_WALL,
    COLL_STAIRS
};

struct CollisionInfo
{
    CollisionType type;

    float radius;

    float width;
    float depth;


};

CollisionInfo collisionTable[MODEL_MAX];

#pragma endregion

#pragma region ===== 状態 / リソース =====

int gameModelHandles[MODEL_MAX];

int gameCurrentMapIndex = 1;

int gameStartX = 0;
int gameStartY = 0;
int gameStartZ = 0;

int gameGoalX = 5;
int gameGoalY = 0;
int gameGoalZ = 5;

#pragma endregion


#pragma region ===== 共通処理 =====

static float RotToRad(int rot)
{
    return rot * 3.14159265f * 0.5f;
}

static vector<string> SplitCSV(const string& line)
{
    vector<string> result;
    string item;
    stringstream ss(line);

    while (getline(ss, item, ','))
    {
        result.push_back(item);
    }

    return result;
}

static bool IsGameMapPosValid(int y, int z, int x)
{
    return
        y >= 0 && y < BLOCK_NUM_Y &&
        z >= 0 && z < BLOCK_NUM_Z &&
        x >= 0 && x < BLOCK_NUM_X;
}

static int WorldToCell(float value)
{
    return (int)floorf(value / BLOCK_SIZE);
}

static float Clamp01(float v)
{
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

int GetMapLayerFromWorldY(float worldY)
{
    int layer = (int)floorf((worldY + 1.0f) / BLOCK_SIZE);

    if (layer < 0) layer = 0;
    if (layer >= BLOCK_NUM_Y) layer = BLOCK_NUM_Y - 1;

    return layer;
}

static CollisionType ParseCollisionType(const string& text)
{
    // 2026-05-11: 新しいモデルを追加した時に、collision_config.csv から既定当たり判定を設定できるよう追加。
    if (text == "CIRCLE") return COLL_CIRCLE;
    if (text == "BOX") return COLL_BOX;
    if (text == "WALL") return COLL_WALL;
    if (text == "STAIRS") return COLL_STAIRS;
    return COLL_NONE;
}

static bool IsStairsId(int id)
{
    return id == 5;
}

static int GetCollisionEdgeBit(int edge)
{
    // 2026-05-11: エディターで保存した辺当たり判定のビットをLoader側で読むため追加。
    return 1 << (edge % 4);
}

static int GetManualCollisionEdgeScale(int y, int z, int x, int edge)
{
    // 2026-05-11: 伸縮編集した辺当たり判定の長さ倍率を取得するため追加。
    if (!IsGameMapPosValid(y, z, x))
        return 100;

    return GameCollisionEdgeScaleMap[y][z][x][edge % 4];
}

static int GetManualCollisionEdgeThickness(int y, int z, int x, int edge)
{
    // 2026-05-11: 厚さ編集した辺当たり判定の倍率を取得するため追加。
    if (!IsGameMapPosValid(y, z, x))
        return 100;

    return GameCollisionEdgeThicknessMap[y][z][x][edge % 4];
}

static bool HasManualCollisionEdge(int y, int z, int x, int edge)
{
    // 2026-05-11: 壁モデルとは別に、手動で置いた辺当たり判定を移動判定へ反映するため追加。
    if (!IsGameMapPosValid(y, z, x))
        return true;

    if (GameCollisionEdgeMap[y][z][x] < 0)
        return false;

    return (GameCollisionEdgeMap[y][z][x] & GetCollisionEdgeBit(edge)) != 0;
}

#pragma endregion


#pragma region ===== 座標変換 =====

VECTOR GetStartPosition()
{
    return VGet(
        gameStartX * BLOCK_SIZE + BLOCK_SIZE * 0.5f,
        gameStartY * BLOCK_SIZE,
        gameStartZ * BLOCK_SIZE + BLOCK_SIZE * 0.5f
    );
}

static VECTOR GetBlockCenterPosition(int x, int y, int z)
{
    return VGet(
        x * BLOCK_SIZE + BLOCK_SIZE * 0.5f,
        y * BLOCK_SIZE,
        z * BLOCK_SIZE + BLOCK_SIZE * 0.5f
    );
}

static VECTOR GetWallPosition(int x, int y, int z, int rot)
{
    float px = x * BLOCK_SIZE + BLOCK_SIZE * 0.5f;
    float py = y * BLOCK_SIZE;
    float pz = z * BLOCK_SIZE + BLOCK_SIZE * 0.5f;

    switch (rot % 4)
    {
    case 0:
        pz = z * BLOCK_SIZE;
        break;
    case 1:
        px = (x + 1) * BLOCK_SIZE;
        break;
    case 2:
        pz = (z + 1) * BLOCK_SIZE;
        break;
    case 3:
        px = x * BLOCK_SIZE;
        break;
    }

    return VGet(px, py, pz);
}

static VECTOR GetModelDrawPosition(int tab, int x, int y, int z, int rot)
{
    if (tab == WALL)
        return GetWallPosition(x, y, z, rot);

    VECTOR pos = GetBlockCenterPosition(x, y, z);

    // 階段モデルだけ位置補正
    if (tab == DECO && GameDecoMap[y][z][x] == 5)
    {
        float offset = BLOCK_SIZE * -0.25f;

        switch (rot % 4)
        {
        case 0: // 上向き
            pos.z += offset;
            break;

        case 1: // 右向き
            pos.x += offset;
            break;

        case 2: // 下向き
            pos.z -= offset;
            break;

        case 3: // 左向き
            pos.x -= offset;
            break;
        }
    }

    return pos;
}
#pragma endregion


#pragma region ===== マップ初期化 =====

static void ClearGameMap()
{
    for (int y = 0; y < BLOCK_NUM_Y; y++)
    {
        for (int z = 0; z < BLOCK_NUM_Z; z++)
        {
            for (int x = 0; x < BLOCK_NUM_X; x++)
            {
                GameFloorMap[y][z][x] = -1;
                GameWallMapA[y][z][x] = -1;
                GameWallMapB[y][z][x] = -1;
                GameCornerMap[y][z][x] = -1;
                GameDecoMap[y][z][x] = -1;
                GameEventMap[y][z][x] = -1;
                // 2026-05-11: マップ再読込時に前の手動当たり判定を残さないため追加。
                GameCollisionMap[y][z][x] = -1;
                // 2026-05-11: マップ再読込時に前の辺当たり判定を残さないため追加。
                GameCollisionEdgeMap[y][z][x] = -1;
                // 2026-05-13: マップ未読込時にコーナー当たり調整値を標準値へ戻すため追加。
                GameCollisionCornerScaleMap[y][z][x] = 100;
                GameCollisionCornerThicknessMap[y][z][x] = 100;
                GameCollisionCornerOffsetMap[y][z][x] = 0;
                for (int edge = 0; edge < 4; edge++)
                {
                    GameCollisionEdgeScaleMap[y][z][x][edge] = 100;
                    GameCollisionEdgeThicknessMap[y][z][x][edge] = 100;
                }

                GameFloorRot[y][z][x] = 0;
                GameWallRotA[y][z][x] = 0;
                GameWallRotB[y][z][x] = 0;
                GameCornerRot[y][z][x] = 0;
                GameDecoRot[y][z][x] = 0;
            }
        }
    }
}

#pragma endregion


#pragma region ===== CSV読込 =====

void LoadGameMap()
{
    ClearGameMap();

    char fileName[64];
    sprintf_s(fileName, "map%d.csv", gameCurrentMapIndex);

    ifstream ifs(fileName);

    if (!ifs)
    {
        printfDx("game map load failed: %s\n", fileName);
        return;
    }

    string section;
    string line;

    while (getline(ifs, line))
    {
        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        if (line[0] == '[')
        {
            section = line;
            continue;
        }

        vector<string> cols = SplitCSV(line);

        if (cols.empty())
            continue;

        if (cols[0] == "SIZE")
            continue;

        if (cols[0] == "START" && cols.size() >= 4)
        {
            gameStartX = stoi(cols[1]);
            gameStartY = stoi(cols[2]);
            gameStartZ = stoi(cols[3]);
            continue;
        }

        if (cols[0] == "GOAL" && cols.size() >= 4)
        {
            gameGoalX = stoi(cols[1]);
            gameGoalY = stoi(cols[2]);
            gameGoalZ = stoi(cols[3]);
            continue;
        }

        if (cols[0] == "y")
            continue;

        if (section == "[FLOOR]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsGameMapPosValid(y, z, x))
            {
                GameFloorMap[y][z][x] = stoi(cols[3]);
                GameFloorRot[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[WALL_A]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsGameMapPosValid(y, z, x))
            {
                GameWallMapA[y][z][x] = stoi(cols[3]);
                GameWallRotA[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[WALL_B]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsGameMapPosValid(y, z, x))
            {
                GameWallMapB[y][z][x] = stoi(cols[3]);
                GameWallRotB[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[CORNER]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsGameMapPosValid(y, z, x))
            {
                GameCornerMap[y][z][x] = stoi(cols[3]);
                GameCornerRot[y][z][x] = stoi(cols[4]);
                // 2026-05-13: コーナー当たり判定の調整値をLoaderでも読むため追加。
                GameCollisionCornerScaleMap[y][z][x] = cols.size() > 5 ? stoi(cols[5]) : 100;
                GameCollisionCornerThicknessMap[y][z][x] = cols.size() > 6 ? stoi(cols[6]) : 100;
                GameCollisionCornerOffsetMap[y][z][x] = cols.size() > 7 ? stoi(cols[7]) : 0;
            }
        }
        else if (section == "[DECO]" && cols.size() >= 5)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsGameMapPosValid(y, z, x))
            {
                GameDecoMap[y][z][x] = stoi(cols[3]);
                GameDecoRot[y][z][x] = stoi(cols[4]);
            }
        }
        else if (section == "[COLLISION]" && cols.size() >= 4)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            // 2026-05-11: エディターで置いた手動当たり判定をLoaderでも読むため追加。
            if (IsGameMapPosValid(y, z, x))
            {
                GameCollisionMap[y][z][x] = stoi(cols[3]);
            }
        }
        else if (section == "[COLLISION_EDGE]" && cols.size() >= 4)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            // 2026-05-11: 保存済みの辺単位当たり判定をLoaderでも読むため追加。
            if (IsGameMapPosValid(y, z, x))
            {
                GameCollisionEdgeMap[y][z][x] = stoi(cols[3]);
                // 2026-05-11: 辺当たり判定の伸縮値もLoaderで読むため追加。
                for (int edge = 0; edge < 4; edge++)
                {
                    GameCollisionEdgeScaleMap[y][z][x][edge] = cols.size() >= 8 ? stoi(cols[4 + edge]) : 100;
                    GameCollisionEdgeThicknessMap[y][z][x][edge] = cols.size() >= 12 ? stoi(cols[8 + edge]) : 100;
                }
            }
        }
        else if (section == "[EVENT]" && cols.size() >= 4)
        {
            int y = stoi(cols[0]);
            int z = stoi(cols[1]);
            int x = stoi(cols[2]);

            if (IsGameMapPosValid(y, z, x))
            {
                GameEventMap[y][z][x] = stoi(cols[3]);
            }
        }
    }
}

#pragma endregion


#pragma region ===== 初期化 / 更新 =====



void UpdateGameMap()
{
    // マップ切り替えはプレイ中に暴発するので無効化
    // if (CheckHitKey(KEY_INPUT_RIGHT))
    // {
    //     gameCurrentMapIndex++;
    //     LoadGameMap();
    // }

    // if (CheckHitKey(KEY_INPUT_LEFT))
    // {
    //     if (gameCurrentMapIndex > 1)
    //         gameCurrentMapIndex--;

    //     LoadGameMap();
    // }
}

#pragma endregion
static void InitCollisionTable()
{
    for (int i = 0; i < MODEL_MAX; i++)
    {
        collisionTable[i].type = COLL_NONE;

        collisionTable[i].radius = 0.0f;

        collisionTable[i].width = 0.0f;
        collisionTable[i].depth = 0.0f;
    }

    collisionTable[1].type = COLL_WALL;

    collisionTable[2].type = COLL_CIRCLE;
    collisionTable[2].radius = BLOCK_SIZE * 0.3f;

    collisionTable[5].type = COLL_STAIRS;

    collisionTable[6].type = COLL_BOX;
    collisionTable[6].width = BLOCK_SIZE * 0.3f;
    collisionTable[6].depth = BLOCK_SIZE * 0.3f;

    collisionTable[7].type = COLL_CIRCLE;
    collisionTable[7].radius = BLOCK_SIZE * 0.20f;

    ifstream ifs("collision_config.csv");
    if (!ifs)
        return;

    string line;
    while (getline(ifs, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        vector<string> cols = SplitCSV(line);
        if (cols.size() < 2 || cols[0] == "id")
            continue;

        int id = stoi(cols[0]);
        if (id < 0 || id >= MODEL_MAX)
            continue;

        // 2026-05-11: 追加モデルの既定当たり判定を、コード変更なしでLoaderへ反映するため追加。
        collisionTable[id].type = ParseCollisionType(cols[1]);
        collisionTable[id].radius = cols.size() >= 3 ? stof(cols[2]) : 0.0f;
        collisionTable[id].width = cols.size() >= 4 ? stof(cols[3]) : 0.0f;
        collisionTable[id].depth = cols.size() >= 5 ? stof(cols[4]) : 0.0f;
    }
}
void InitGameMap()
{
    for (int i = 0; i < MODEL_MAX; i++)
    {
        gameModelHandles[i] = -1;
    }

    gameModelHandles[0] = MV1LoadModel("model/floor.mv1");
    gameModelHandles[1] = MV1LoadModel("model/wall.mv1");
    gameModelHandles[2] = MV1LoadModel("model/corner.mv1");
    gameModelHandles[3] = MV1LoadModel("model/wall_top.mv1");
    gameModelHandles[4] = MV1LoadModel("model/wall_detail.mv1");
    gameModelHandles[5] = MV1LoadModel("model/stairs.mv1");

    MV1SetScale(
        gameModelHandles[5],
        VGet(0.8f, 1.0f, 0.8f)
    );
    gameModelHandles[6] = MV1LoadModel("model/tree.mv1");
    gameModelHandles[7] = MV1LoadModel("model/barrel.mv1");

    for (int i = 0; i < 8; i++)
    {
        if (gameModelHandles[i] == -1)
        {
            printfDx("game model load failed: %d\n", i);
        }
        else
        {
            MV1SetupCollInfo(gameModelHandles[i], -1);
        }
    }

    LoadGameMap();

    InitCollisionTable();
}
#pragma region ===== 描画 =====

static void DrawModelById(int id, VECTOR pos, int rot)
{
    if (id < 0 || id >= MODEL_MAX)
        return;

    if (gameModelHandles[id] == -1)
        return;

    MV1SetPosition(gameModelHandles[id], pos);
    MV1SetRotationXYZ(gameModelHandles[id], VGet(0.0f, RotToRad(rot), 0.0f));
    MV1DrawModel(gameModelHandles[id]);
}

void DrawGameMap()
{
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    for (int y = 0; y < BLOCK_NUM_Y; y++)
    {
        for (int z = 0; z < BLOCK_NUM_Z; z++)
        {
            for (int x = 0; x < BLOCK_NUM_X; x++)
            {
                if (GameFloorMap[y][z][x] >= 0)
                {
                    DrawModelById(
                        GameFloorMap[y][z][x],
                        GetModelDrawPosition(FLOOR, x, y, z, GameFloorRot[y][z][x]),
                        GameFloorRot[y][z][x]
                    );
                }

                if (GameWallMapA[y][z][x] >= 0)
                {
                    DrawModelById(
                        GameWallMapA[y][z][x],
                        GetModelDrawPosition(WALL, x, y, z, GameWallRotA[y][z][x]),
                        GameWallRotA[y][z][x]
                    );
                }

                if (GameWallMapB[y][z][x] >= 0)
                {
                    DrawModelById(
                        GameWallMapB[y][z][x],
                        GetModelDrawPosition(WALL, x, y, z, GameWallRotB[y][z][x]),
                        GameWallRotB[y][z][x]
                    );
                }

                if (GameCornerMap[y][z][x] >= 0)
                {
                    DrawModelById(
                        GameCornerMap[y][z][x],
                        GetModelDrawPosition(CORNER, x, y, z, GameCornerRot[y][z][x]),
                        GameCornerRot[y][z][x]
                    );
                }

                if (GameDecoMap[y][z][x] >= 0)
                {
                    DrawModelById(
                        GameDecoMap[y][z][x],
                        GetModelDrawPosition(DECO, x, y, z, GameDecoRot[y][z][x]),
                        GameDecoRot[y][z][x]
                    );
                }
            }
        }
    }

    DrawFormatString(
        10, 10,
        GetColor(255, 255, 0),
        "Map:%d  Start:%d,%d,%d  Goal:%d,%d,%d",
        gameCurrentMapIndex,
        gameStartX, gameStartY, gameStartZ,
        gameGoalX, gameGoalY, gameGoalZ
    );
}

#pragma endregion


#pragma region ===== セル単位判定 =====

static bool IsWalkableCell(int y, int z, int x)
{
    if (!IsGameMapPosValid(y, z, x))
        return false;

    if (GameFloorMap[y][z][x] >= 0)
        return true;

    if (IsStairsId(GameDecoMap[y][z][x]))
        return true;

    return false;
}

bool HasWallEdge(int y, int z, int x, int edge)
{
    if (!IsGameMapPosValid(y, z, x))
        return true;

    if (GameWallMapA[y][z][x] >= 0 && GameWallRotA[y][z][x] == edge)
        return true;

    if (GameWallMapB[y][z][x] >= 0 && GameWallRotB[y][z][x] == edge)
        return true;

    return false;
}

bool CanMoveCellToCell(int y, int fromX, int fromZ, int toX, int toZ)
{
    if (!IsWalkableCell(y, toZ, toX))
        return false;

    int dx = toX - fromX;
    int dz = toZ - fromZ;

    if (dx == 0 && dz == -1)
    {
        // if (HasWallEdge(y, fromZ, fromX, 0)) return false;
        //if (HasWallEdge(y, toZ, toX, 2)) return false;
    }
    else if (dx == 1 && dz == 0)
    {
        //if (HasWallEdge(y, fromZ, fromX, 1)) return false;
        //if (HasWallEdge(y, toZ, toX, 3)) return false;
    }
    else if (dx == 0 && dz == 1)
    {
        // if (HasWallEdge(y, fromZ, fromX, 2)) return false;
        // if (HasWallEdge(y, toZ, toX, 0)) return false;
    }
    else if (dx == -1 && dz == 0)
    {
        // if (HasWallEdge(y, fromZ, fromX, 3)) return false;
        // if (HasWallEdge(y, toZ, toX, 1)) return false;
    }
    else
    {
        return false;
    }

    return true;
}

bool IsGoalCell(int y, int z, int x)
{
    return
        x == gameGoalX &&
        y == gameGoalY &&
        z == gameGoalZ;
}

#pragma endregion


#pragma region ===== ワールド判定 共通 =====

static bool IsWalkableAtWorld(int y, float worldX, float worldZ)
{
    int x = WorldToCell(worldX);
    int z = WorldToCell(worldZ);

    return IsWalkableCell(y, z, x);
}
/*
static bool HitVerticalWallLine(float px, float pz, float radius, float lineX, float z1, float z2)
{
    if (pz < z1 - radius || pz > z2 + radius)
        return false;

    return fabsf(px - lineX) <= radius * 0.7f;
}
*/
/*
static bool HitHorizontalWallLine(float px, float pz, float radius, float lineZ, float x1, float x2)
{
    if (px < x1 - radius || px > x2 + radius)
        return false;

    return fabsf(pz - lineZ) <= radius * 0.7f;
}
*/
static bool CircleHitAABB(float cx, float cz, float radius, float left, float top, float right, float bottom)
{
    float nearestX = cx;
    float nearestZ = cz;

    if (nearestX < left) nearestX = left;
    if (nearestX > right) nearestX = right;
    if (nearestZ < top) nearestZ = top;
    if (nearestZ > bottom) nearestZ = bottom;

    float dx = cx - nearestX;
    float dz = cz - nearestZ;



    return (dx * dx + dz * dz) <= radius * radius;
}

static bool CircleHitCircle(
    float x1,
    float z1,
    float r1,
    float x2,
    float z2,
    float r2)
{
    float dx = x1 - x2;
    float dz = z1 - z2;

    float rr = r1 + r2;

    return dx * dx + dz * dz <= rr * rr;


}

static bool CircleHitBox(
    float cx,
    float cz,
    float radius,
    float bx,
    float bz,
    float halfW,
    float halfD)
{
    float left = bx - halfW;
    float right = bx + halfW;
    float top = bz - halfD;
    float bottom = bz + halfD;

    return CircleHitAABB(
        cx,
        cz,
        radius,
        left,
        top,
        right,
        bottom);
}
/*
static bool HitCornerRound(
    int y,
    float worldX,
    float worldZ,
    float radius)
{
    const float cornerRadius = BLOCK_SIZE * 0.18f;
    const float offset = BLOCK_SIZE * 0.12f;

    int cellX = WorldToCell(worldX);
    int cellZ = WorldToCell(worldZ);

    for (int dz = -1; dz <= 1; dz++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int x = cellX + dx;
            int z = cellZ + dz;

            if (!IsGameMapPosValid(y, z, x))
                continue;

            if (GameCornerMap[y][z][x] < 0)
                continue;

            float cx = 0.0f;
            float cz = 0.0f;

            switch (GameCornerRot[y][z][x] % 4)
            {
            case 0:
                cx = x * BLOCK_SIZE + offset;
                cz = z * BLOCK_SIZE + offset;
                break;

            case 1:
                cx = (x + 1) * BLOCK_SIZE - offset;
                cz = z * BLOCK_SIZE + offset;
                break;

            case 2:
                cx = (x + 1) * BLOCK_SIZE - offset;
                cz = (z + 1) * BLOCK_SIZE - offset;
                break;

            case 3:
                cx = x * BLOCK_SIZE + offset;
                cz = (z + 1) * BLOCK_SIZE - offset;
                break;
            }

            if (CircleHitCircle(
                worldX,
                worldZ,
                radius,
                cx,
                cz,
                cornerRadius))
            {
                return true;
            }
        }
    }

    return false;
}
*/
static bool IsStairsAtWorldInLayer(int y, float worldX, float worldZ)
{
    int x = WorldToCell(worldX);
    int z = WorldToCell(worldZ);

    if (!IsGameMapPosValid(y, z, x))
        return false;

    return IsStairsId(GameDecoMap[y][z][x]);
}

static bool IsWalkableAtWorldForStairsTransition(int y, float worldX, float worldZ)
{
    // 2026-05-13: 階段でレイヤーが切り替わった直後も、半径の一部が下階段側に残って進めるよう追加。
    if (IsWalkableAtWorld(y, worldX, worldZ))
        return true;

    if (IsStairsAtWorldInLayer(y, worldX, worldZ))
        return true;

    if (IsStairsAtWorldInLayer(y - 1, worldX, worldZ))
        return true;

    return false;
}

static bool HasWalkableSurfaceForCircle(int y, float worldX, float worldZ, float radius)
{
    // 階段は1マス内で高さが変わるので、足元中心が階段なら通す。
    // 周囲5点まで見ると階段セルの端で弾かれて登れなくなる。
    if (IsStairsAtWorldInLayer(y, worldX, worldZ))
        return true;

    // 2026-05-13: レイヤー切り替え直後の上階判定でも、下の階段にかかっている足元を歩ける扱いにするため変更。
    if (!IsWalkableAtWorldForStairsTransition(y, worldX, worldZ)) return false;
    if (!IsWalkableAtWorldForStairsTransition(y, worldX - radius, worldZ)) return false;
    if (!IsWalkableAtWorldForStairsTransition(y, worldX + radius, worldZ)) return false;
    if (!IsWalkableAtWorldForStairsTransition(y, worldX, worldZ - radius)) return false;
    if (!IsWalkableAtWorldForStairsTransition(y, worldX, worldZ + radius)) return false;

    return true;
}
#pragma endregion


#pragma region ===== 壁 / 物 判定 =====

static bool IsSolidDecoId(int id)
{
    if (id < 0)
        return false;

    if (IsStairsId(id))
        return false;

    return true;
}

static bool IsSolidCell(int y, int z, int x)
{
    if (!IsGameMapPosValid(y, z, x))
        return true;

    // 2026-05-11: エディターで手動設定したセル当たり判定を移動判定へ反映するため追加。
    if (GameCollisionMap[y][z][x] >= 0)
        return true;

    //if (GameCornerMap[y][z][x] >= 0)
    //    return true;

    if (IsSolidDecoId(GameDecoMap[y][z][x]))
        return true;

    return false;
}
bool IsCellBlocked(int y, int z, int x)
{
    // 2026-05-11: エディター/デバッグ側からセルの当たり判定状態を確認できるよう追加。
    return IsSolidCell(y, z, x);
}
static float PointLineDistance(
    float px,
    float pz,
    float x1,
    float z1,
    float x2,
    float z2)
{
    float vx = x2 - x1;
    float vz = z2 - z1;

    float wx = px - x1;
    float wz = pz - z1;

    float lenSq = vx * vx + vz * vz;

    float t = 0.0f;

    if (lenSq > 0.0f)
    {
        t = (wx * vx + wz * vz) / lenSq;
    }

    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    float nearestX = x1 + vx * t;
    float nearestZ = z1 + vz * t;

    float dx = px - nearestX;
    float dz = pz - nearestZ;

    return sqrtf(dx * dx + dz * dz);
}
static bool HitMapObjects(
    int y,
    float worldX,
    float worldZ,
    float radius)
{
    int cellX = WorldToCell(worldX);
    int cellZ = WorldToCell(worldZ);

    for (int dz = -1; dz <= 1; dz++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int x = cellX + dx;
            int z = cellZ + dz;

            if (!IsGameMapPosValid(y, z, x))
                continue;

            // 2026-05-11: 手動当たり判定セルは、プレイヤー半径とセル矩形の接触で止めるため追加。
            if (GameCollisionMap[y][z][x] >= 0)
            {
                float blockLeft = x * BLOCK_SIZE;
                float blockRight = (x + 1) * BLOCK_SIZE;
                float blockTop = z * BLOCK_SIZE;
                float blockBottom = (z + 1) * BLOCK_SIZE;

                if (CircleHitAABB(worldX, worldZ, radius, blockLeft, blockTop, blockRight, blockBottom))
                    return true;
            }

            float left = x * BLOCK_SIZE;
            float right = (x + 1) * BLOCK_SIZE;
            float top = z * BLOCK_SIZE;
            float bottom = (z + 1) * BLOCK_SIZE;


            auto CheckWall = [&](int rot, int scale, int thicknessScale)
                {
                    float x1, z1;
                    float x2, z2;

                    // 各方向ごとの線位置
                    switch (rot % 4)
                    {
                    case 0: // 上壁
                        x1 = left + BLOCK_SIZE * 0.0f;
                        z1 = top + BLOCK_SIZE * 0.12f;

                        x2 = right - BLOCK_SIZE * 0.0f;
                        z2 = top + BLOCK_SIZE * 0.12f;
                        break;

                    case 1: // 右
                        x1 = right - BLOCK_SIZE * -0.12f;
                        z1 = top + BLOCK_SIZE * 0.00f;

                        x2 = right - BLOCK_SIZE * -0.12f;
                        z2 = bottom - BLOCK_SIZE * 0.00f;
                        break;
                    case 2: // 下壁
                        x1 = left + BLOCK_SIZE * 0.0f;
                        z1 = bottom - BLOCK_SIZE * 0.12f;

                        x2 = right - BLOCK_SIZE * 0.0f;
                        z2 = bottom - BLOCK_SIZE * 0.12f;
                        break;
                    case 3: // 左壁
                        x1 = left + BLOCK_SIZE * -0.12f;
                        z1 = top + BLOCK_SIZE * 0.0f;

                        x2 = left + BLOCK_SIZE * -0.12f;
                        z2 = bottom - BLOCK_SIZE * 0.0f;
                        break;


                    }

                    float centerX = (x1 + x2) * 0.5f;
                    float centerZ = (z1 + z2) * 0.5f;
                    float rate = scale / 100.0f;

                    x1 = centerX + (x1 - centerX) * rate;
                    z1 = centerZ + (z1 - centerZ) * rate;
                    x2 = centerX + (x2 - centerX) * rate;
                    z2 = centerZ + (z2 - centerZ) * rate;

                    float dist = PointLineDistance(
                        worldX,
                        worldZ,
                        x1,
                        z1,
                        x2,
                        z2);

                    // 方向別厚み
                    float wallThickness = BLOCK_SIZE * 0.08f;

                    switch (rot % 4)
                    {
                    case 0: wallThickness = BLOCK_SIZE * 0.07f; break;
                    case 1: wallThickness = BLOCK_SIZE * 0.07f; break;
                    case 2: wallThickness = BLOCK_SIZE * 0.05f; break;
                    case 3: wallThickness = BLOCK_SIZE * 0.09f; break;
                    }

                    wallThickness *= thicknessScale / 100.0f;

                    return dist <= radius + wallThickness;
                };

            if (HasWallEdge(y, z, x, 0) && CheckWall(0, 100, 100))
            {
                return true;
            }

            if (HasWallEdge(y, z, x, 1) && CheckWall(1, 100, 100))
            {
                return true;
            }

            if (HasWallEdge(y, z, x, 2) && CheckWall(2, 100, 100))
            {
                return true;
            }

            if (HasWallEdge(y, z, x, 3) && CheckWall(3, 100, 100))
            {
                return true;
            }


            if (HasManualCollisionEdge(y, z, x, 0) && CheckWall(0, GetManualCollisionEdgeScale(y, z, x, 0), GetManualCollisionEdgeThickness(y, z, x, 0)))
            {
                return true;
            }

            if (HasManualCollisionEdge(y, z, x, 1) && CheckWall(1, GetManualCollisionEdgeScale(y, z, x, 1), GetManualCollisionEdgeThickness(y, z, x, 1)))
            {
                return true;
            }

            if (HasManualCollisionEdge(y, z, x, 2) && CheckWall(2, GetManualCollisionEdgeScale(y, z, x, 2), GetManualCollisionEdgeThickness(y, z, x, 2)))
            {
                return true;
            }

            if (HasManualCollisionEdge(y, z, x, 3) && CheckWall(3, GetManualCollisionEdgeScale(y, z, x, 3), GetManualCollisionEdgeThickness(y, z, x, 3)))
            {
                return true;
            }
            int cornerId = GameCornerMap[y][z][x];

            if (cornerId >= 0)
            {
                float x1, z1;
                float x2, z2;

                float inset = BLOCK_SIZE * 0.12f;

                switch (GameCornerRot[y][z][x] % 4)
                {
                case 0: // ＼
                    x1 = left + inset;
                    z1 = top + inset;

                    x2 = right - inset;
                    z2 = bottom - inset;
                    break;

                case 1: // ／
                    x1 = right - inset;
                    z1 = top + inset;

                    x2 = left + inset;
                    z2 = bottom - inset;
                    break;

                case 2: // ＼
                    x1 = right - inset;
                    z1 = bottom - inset;

                    x2 = left + inset;
                    z2 = top + inset;
                    break;

                case 3: // ／
                    x1 = left + inset;
                    z1 = bottom - inset;

                    x2 = right - inset;
                    z2 = top + inset;
                    break;
                }

                // 2026-05-13: エディターで調整したコーナー当たり判定の長さ・厚み・奥行を実判定へ反映するため追加。
                float centerX = (x1 + x2) * 0.5f;
                float centerZ = (z1 + z2) * 0.5f;
                float rate = GameCollisionCornerScaleMap[y][z][x] / 100.0f;

                x1 = centerX + (x1 - centerX) * rate;
                z1 = centerZ + (z1 - centerZ) * rate;
                x2 = centerX + (x2 - centerX) * rate;
                z2 = centerZ + (z2 - centerZ) * rate;

                float lineDX = x2 - x1;
                float lineDZ = z2 - z1;
                float lineLen = sqrtf(lineDX * lineDX + lineDZ * lineDZ);
                if (lineLen > 0.0001f)
                {
                    float nx = -lineDZ / lineLen;
                    float nz = lineDX / lineLen;
                    float offset = (float)GameCollisionCornerOffsetMap[y][z][x];

                    x1 += nx * offset;
                    z1 += nz * offset;
                    x2 += nx * offset;
                    z2 += nz * offset;
                }

                float dist = PointLineDistance(
                    worldX,
                    worldZ,
                    x1,
                    z1,
                    x2,
                    z2);

                float wallThickness = BLOCK_SIZE * 0.07f;
                wallThickness *= GameCollisionCornerThicknessMap[y][z][x] / 100.0f;
                if (dist <= radius + wallThickness)
                {
                    return true;
                }
            }




            int decoId = GameDecoMap[y][z][x];

            if (decoId >= 0)
            {
                CollisionInfo& info = collisionTable[decoId];

                if (info.type == COLL_CIRCLE)
                {
                    float cx =
                        x * BLOCK_SIZE + BLOCK_SIZE * 0.5f;

                    float cz =
                        z * BLOCK_SIZE + BLOCK_SIZE * 0.5f;

                    if (CircleHitCircle(
                        worldX,
                        worldZ,
                        radius,
                        cx,
                        cz,
                        info.radius))
                    {
                        return true;
                    }
                }
                else if (info.type == COLL_BOX)
                {
                    float bx =
                        x * BLOCK_SIZE + BLOCK_SIZE * 0.5f;

                    float bz =
                        z * BLOCK_SIZE + BLOCK_SIZE * 0.5f;

                    if (CircleHitBox(
                        worldX,
                        worldZ,
                        radius,
                        bx,
                        bz,
                        info.width,
                        info.depth))
                    {
                        return true;
                    }
                }
                else if (info.type == COLL_STAIRS)
                {
                    // 階段は中央だけ通す
                    float left = x * BLOCK_SIZE;
                    float right = (x + 1) * BLOCK_SIZE;
                    float top = z * BLOCK_SIZE;
                    float bottom = (z + 1) * BLOCK_SIZE;

                    float wall = BLOCK_SIZE * 0.18f;

                    bool hit = false;

                    switch (GameDecoRot[y][z][x] % 4)
                    {
                    case 0:
                    case 2:
                        // 左右壁
                        if (CircleHitAABB(
                            worldX, worldZ, radius,
                            left, top,
                            left + wall, bottom))
                        {
                            hit = true;
                        }

                        if (CircleHitAABB(
                            worldX, worldZ, radius,
                            right - wall, top,
                            right, bottom))
                        {
                            hit = true;
                        }
                        break;

                    case 1:
                    case 3:
                        // 上下壁
                        if (CircleHitAABB(
                            worldX, worldZ, radius,
                            left, top,
                            right, top + wall))
                        {
                            hit = true;
                        }

                        if (CircleHitAABB(
                            worldX, worldZ, radius,
                            left, bottom - wall,
                            right, bottom))
                        {
                            hit = true;
                        }
                        break;
                    }

                    if (hit)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}
/*
static bool HitWallEdges(int y, float worldX, float worldZ, float radius)
{
    int cellX = WorldToCell(worldX);
    int cellZ = WorldToCell(worldZ);

    for (int dz = -1; dz <= 1; dz++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int x = cellX + dx;
            int z = cellZ + dz;

            if (!IsGameMapPosValid(y, z, x))
                continue;

            float left = x * BLOCK_SIZE;
            float right = (x + 1) * BLOCK_SIZE;
            float top = z * BLOCK_SIZE;
            float bottom = (z + 1) * BLOCK_SIZE;

            if (HasWallEdge(y, z, x, 0) && HitHorizontalWallLine(worldX, worldZ, radius, top, left, right))
                return true;

            if (HasWallEdge(y, z, x, 1) && HitVerticalWallLine(worldX, worldZ, radius, right, top, bottom))
                return true;

            if (HasWallEdge(y, z, x, 2) && HitHorizontalWallLine(worldX, worldZ, radius, bottom, left, right))
                return true;

            if (HasWallEdge(y, z, x, 3) && HitVerticalWallLine(worldX, worldZ, radius, left, top, bottom))
                return true;
        }
    }

    return false;
}
*/

#pragma endregion




#pragma region ===== 形状判定 =====

static bool IsCollisionModelId(int id)
{
    if (id < 0)
        return false;

    if (id == 0)
        return false;

    if (IsStairsId(id))
        return false;

    return true;
}

static bool HitPlacedModelSphere(int id, int tab, int x, int y, int z, int rot, VECTOR center, float radius)
{
    if (!IsCollisionModelId(id))
        return false;

    if (id >= MODEL_MAX || gameModelHandles[id] == -1)
        return false;

    MV1SetPosition(gameModelHandles[id], GetModelDrawPosition(tab, x, y, z, rot));
    MV1SetRotationXYZ(gameModelHandles[id], VGet(0.0f, RotToRad(rot), 0.0f));

    MV1_COLL_RESULT_POLY_DIM hit = MV1CollCheck_Sphere(gameModelHandles[id], -1, center, radius);
    bool result = hit.HitNum > 0;
    MV1CollResultPolyDimTerminate(hit);

    return result;
}
/*
static bool HitCollisionModels(int y, float worldX, float worldZ, float radius)
{
    int cellX = WorldToCell(worldX);
    int cellZ = WorldToCell(worldZ);
    VECTOR center = VGet(worldX, y * BLOCK_SIZE + radius, worldZ);

    for (int dz = -1; dz <= 1; dz++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int x = cellX + dx;
            int z = cellZ + dz;

            if (!IsGameMapPosValid(y, z, x))
                continue;

            // 壁はエディターと同じ端ライン判定を使う。
            // モデル判定も重ねると、モデル原点や当たり情報の差で見た目よりズレることがある。
            //if (HitPlacedModelSphere(GameCornerMap[y][z][x], CORNER, x, y, z, GameCornerRot[y][z][x], center, radius))
            //    return true;

            if (HitPlacedModelSphere(GameDecoMap[y][z][x], DECO, x, y, z, GameDecoRot[y][z][x], center, radius))
                return true;
        }
    }

    return false;
}
*/
#pragma endregion


#pragma region ===== 高さ / 階段 =====

bool IsStairsAtWorld(float worldX, float worldZ)
{
    int x = WorldToCell(worldX);
    int z = WorldToCell(worldZ);

    for (int y = BLOCK_NUM_Y - 1; y >= 0; y--)
    {
        if (!IsGameMapPosValid(y, z, x))
            continue;

        if (IsStairsId(GameDecoMap[y][z][x]))
            return true;
    }

    return false;
}

float GetMapGroundY(float worldX, float worldZ)
{
    int x = WorldToCell(worldX);
    int z = WorldToCell(worldZ);

    if (x < 0 || x >= BLOCK_NUM_X || z < 0 || z >= BLOCK_NUM_Z)
        return 0.0f;

    for (int y = BLOCK_NUM_Y - 1; y >= 0; y--)
    {
        float baseY = y * BLOCK_SIZE;

        if (IsStairsId(GameDecoMap[y][z][x]))
        {
            float localX = Clamp01((worldX - x * BLOCK_SIZE) / BLOCK_SIZE);
            float localZ = Clamp01((worldZ - z * BLOCK_SIZE) / BLOCK_SIZE);

            float t = 1.3f;

            switch (GameDecoRot[y][z][x] % 4)
            {
            case 0: // 奥→手前へ上る
                t = 1.0f - localZ;
                break;

            case 1: // 左→右へ上る
                t = 1.0f - localX;
                break;

            case 2: // 手前→奥へ上る
                t =  localZ;
                break;

            case 3: // 右→左へ上る
                t =  localX;
                break;
            }

            // なめらか補正
            t = sqrtf(t);

            // 全方向同じ高さ補正
            return baseY + t * BLOCK_SIZE;
        }

        if (GameFloorMap[y][z][x] >= 0)
            return baseY;
    }

    return 0.0f;
}
#pragma endregion


#pragma region ===== 連続移動判定 =====

static bool CanStandPlayerPosition(
    int y,
    float worldX,
    float worldZ,
    float radius)
{
    if (!HasWalkableSurfaceForCircle(y, worldX, worldZ, radius))
        return false;

    if (HitMapObjects(y, worldX, worldZ, radius))
        return false;

    return true;
}

bool CanMoveWorldPosition(int y, float worldX, float worldZ, float radius)
{
    // 2026-05-11: デバッグや他クラスから、ワールド座標が移動可能か直接確認できるよう追加。
    return CanStandPlayerPosition(y, worldX, worldZ, radius);
}
static bool CanStandPlayerPositionByWorld(float worldX, float worldZ, float radius, int currentLayer)
{
    // 今いる階層でまず判定
    if (CanStandPlayerPosition(currentLayer, worldX, worldZ, radius))
        return true;

    // 上に移動（階段）
    if (currentLayer + 1 < BLOCK_NUM_Y &&
        CanStandPlayerPosition(currentLayer + 1, worldX, worldZ, radius))
        return true;

    // 下に移動（段差）
    if (currentLayer - 1 >= 0 &&
        CanStandPlayerPosition(currentLayer - 1, worldX, worldZ, radius))
        return true;

    return false;
}

VECTOR ResolvePlayerMapCollision(
    VECTOR currentPos,
    VECTOR nextPos,
    float radius,
    int currentLayer)
{
    VECTOR result = currentPos;

    /// 移動量
    float moveX = nextPos.x - currentPos.x;
    float moveZ = nextPos.z - currentPos.z;

    if (CanStandPlayerPositionByWorld(
        nextPos.x,
        nextPos.z,
        radius,
        currentLayer))
    {
        result.x = nextPos.x;
        result.z = nextPos.z;
    }
    else
    {
        if (CanStandPlayerPositionByWorld(
            currentPos.x + moveX,
            currentPos.z,
            radius,
            currentLayer))
        {
            result.x = currentPos.x + moveX;
        }

        if (CanStandPlayerPositionByWorld(
            result.x,
            currentPos.z + moveZ,
            radius,
            currentLayer))
        {
            result.z = currentPos.z + moveZ;
        }
    }

    /// 高さ
    result.y = nextPos.y;

    return result;
}

bool IsGoalWorldPosition(int y, float worldX, float worldZ)
{
    int x = WorldToCell(worldX);
    int z = WorldToCell(worldZ);

    return IsGoalCell(y, z, x);
}

#pragma endregion

