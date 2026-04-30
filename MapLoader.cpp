#include "MapLoader.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>



using namespace std;

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

int gameModelHandles[MODEL_MAX];

int gameCurrentMapIndex = 1;

int gameStartX = 0;
int gameStartY = 0;
int gameStartZ = 0;

int gameGoalX = 5;
int gameGoalY = 0;
int gameGoalZ = 5;

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

                GameFloorRot[y][z][x] = 0;
                GameWallRotA[y][z][x] = 0;
                GameWallRotB[y][z][x] = 0;
                GameCornerRot[y][z][x] = 0;
                GameDecoRot[y][z][x] = 0;
            }
        }
    }
}
//スタート場
VECTOR GetStartPosition()
{
    return VGet(
        gameStartX * BLOCK_SIZE,
        gameStartY * BLOCK_SIZE,
        gameStartZ * BLOCK_SIZE
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

    return GetBlockCenterPosition(x, y, z);
}

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
    gameModelHandles[6] = MV1LoadModel("model/tree.mv1");
    gameModelHandles[7] = MV1LoadModel("model/barrel.mv1");

    for (int i = 0; i < 8; i++)
    {
        if (gameModelHandles[i] == -1)
        {
            printfDx("game model load failed: %d\n", i);
        }
    }

    LoadGameMap();
}

void UpdateGameMap()
{
    if (CheckHitKey(KEY_INPUT_RIGHT))
    {
        gameCurrentMapIndex++;
        LoadGameMap();
    }

    if (CheckHitKey(KEY_INPUT_LEFT))
    {
        if (gameCurrentMapIndex > 1)
            gameCurrentMapIndex--;

        LoadGameMap();
    }
}

void DrawGameMap()
{
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    float centerX = BLOCK_NUM_X * BLOCK_SIZE * 0.5f;
    float centerZ = BLOCK_NUM_Z * BLOCK_SIZE * 0.5f;

  //  SetCameraPositionAndTarget_UpVecY(
  //    VGet(centerX, 6000.0f, centerZ - 6000.0f),
  //    VGet(centerX, 0.0f, centerZ)
   

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
    if (!IsGameMapPosValid(y, toZ, toX))
        return false;

    if (GameFloorMap[y][toZ][toX] < 0)
        return false;

    int dx = toX - fromX;
    int dz = toZ - fromZ;

    if (dx == 0 && dz == -1)
    {
        if (HasWallEdge(y, fromZ, fromX, 0)) return false;
        if (HasWallEdge(y, toZ, toX, 2)) return false;
    }
    else if (dx == 1 && dz == 0)
    {
        if (HasWallEdge(y, fromZ, fromX, 1)) return false;
        if (HasWallEdge(y, toZ, toX, 3)) return false;
    }
    else if (dx == 0 && dz == 1)
    {
        if (HasWallEdge(y, fromZ, fromX, 2)) return false;
        if (HasWallEdge(y, toZ, toX, 0)) return false;
    }
    else if (dx == -1 && dz == 0)
    {
        if (HasWallEdge(y, fromZ, fromX, 3)) return false;
        if (HasWallEdge(y, toZ, toX, 1)) return false;
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
