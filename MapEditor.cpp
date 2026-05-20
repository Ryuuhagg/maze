#include "MapEditor.h"
#include "Input.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

#pragma region ===== マップ配列 =====

int FloorMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int FloorRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int WallMapA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int WallMapB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int WallRotA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int WallRotB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int CornerMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int CornerRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int DecoMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int DecoRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

int EventMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-11: エディターでセル単位の手動当たり判定を持たせ、CSV経由でLoaderへ渡すため追加。
int CollisionMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-11: Loaderの壁ライン判定をエディターで直接見て編集できるよう、辺単位の手動当たり判定を追加。
int CollisionEdgeMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
// 2026-05-11: 辺当たり判定を伸ばしたり縮めたりする編集値を持つため追加。100が通常の長さ。
int CollisionEdgeScaleMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X][4];
// 2026-05-11: 辺当たり判定の厚さを編集できるよう、100を通常の厚さとして追加。
int CollisionEdgeThicknessMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X][4];
// 2026-05-13: コーナー当たり判定の長さ/厚みをエディターで調整・保存するため追加。
int CollisionCornerScaleMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
int CollisionCornerThicknessMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
#pragma endregion


#pragma region ===== モデル別の既定当たり判定 =====

enum EditorCollisionType
{
    EDITOR_COLL_NONE,
    EDITOR_COLL_CIRCLE,
    EDITOR_COLL_BOX,
    EDITOR_COLL_WALL,
    EDITOR_COLL_STAIRS
};

struct EditorCollisionInfo
{
    EditorCollisionType type;
    float radius;
    float width;
    float depth;
};

// 2026-05-11: オブジェクトを置いた時点で既定の当たり判定を持たせ、エディターで見えるようにするため追加。
EditorCollisionInfo editorCollisionTable[MODEL_MAX];



#pragma region ===== エディタ状態 =====

int currentLayer = 0;
int currentRot = 0;
int currentMapIndex = 1;

int currentTab = FLOOR;
int selectedModel = 0;

int hoverX = -1;
int hoverZ = -1;

bool showGrid = true;
bool brushMode = false;
// 2026-05-11: 当たり判定編集と確認表示を、配置作業中に切り替えられるよう追加。
bool showCollisionDebug = false;
bool collisionEditMode = false;
// 2026-05-11: 当たり判定編集時にセル編集/辺編集を切り替えるため追加。
bool collisionEdgeEditMode = false;
// 2026-05-13: 辺/コーナー当たりのホイール編集を長さモード/厚みモードで切り替えるため追加。
bool collisionDepthEditMode = false;

int startX = 0, startY = 0, startZ = 0;
int goalX = 5, goalY = 0, goalZ = 5;

#pragma endregion


#pragma region ===== カメラ =====

float camRotY = 0.0f;
float camRotX = -0.4f;
float camDist = 5000.0f;

#pragma endregion


#pragma region ===== リソース =====

int modelHandles[MODEL_MAX];
int paletteTex[MODEL_MAX];

#pragma endregion


#pragma region ===== タブ設定 =====

int tabModelList[TAB_MAX][16] =
{
    {0},        // FLOOR
    {1,3,4},    // WALL
    {2},        // CORNER
    {5,6,7},    // DECO
    {0,1}       // EVENT
};

int tabModelCount[TAB_MAX] = { 1,3,1,3,2 };

#pragma endregion


#pragma region ===== 入力状態 =====

int oldMX = 0;
int oldMY = 0;
int oldClick = 0;
int oldMouse = 0;

#pragma endregion


#pragma region ===== ブラシ状態 =====

int lastBrushX = -1;
int lastBrushZ = -1;
int lastBrushLayer = -1;

#pragma endregion


#pragma region ===== 範囲選択状態 =====

bool selectMode = false;
bool selecting = false;

int selectStartX = -1;
int selectStartZ = -1;
int selectEndX = -1;
int selectEndZ = -1;
int selectLayer = 0;

#pragma endregion


#pragma region コピー

const int COPY_MAX_X = BLOCK_NUM_X;
const int COPY_MAX_Z = BLOCK_NUM_Z;

int copySizeX = 0;
int copySizeZ = 0;
bool hasCopyData = false;

int CopyFloorMap[COPY_MAX_Z][COPY_MAX_X];
int CopyFloorRot[COPY_MAX_Z][COPY_MAX_X];

int CopyWallMapA[COPY_MAX_Z][COPY_MAX_X];
int CopyWallMapB[COPY_MAX_Z][COPY_MAX_X];
int CopyWallRotA[COPY_MAX_Z][COPY_MAX_X];
int CopyWallRotB[COPY_MAX_Z][COPY_MAX_X];

int CopyCornerMap[COPY_MAX_Z][COPY_MAX_X];
int CopyCornerRot[COPY_MAX_Z][COPY_MAX_X];

int CopyDecoMap[COPY_MAX_Z][COPY_MAX_X];
int CopyDecoRot[COPY_MAX_Z][COPY_MAX_X];

int CopyEventMap[COPY_MAX_Z][COPY_MAX_X];
// 2026-05-11: 範囲コピー/貼り付けでも手動当たり判定を一緒に扱うため追加。
int CopyCollisionMap[COPY_MAX_Z][COPY_MAX_X];
// 2026-05-11: 範囲コピー/貼り付けでも辺単位の当たり判定を一緒に扱うため追加。
int CopyCollisionEdgeMap[COPY_MAX_Z][COPY_MAX_X];
// 2026-05-11: 範囲コピー/貼り付けでも辺の伸縮値を一緒に扱うため追加。
int CopyCollisionEdgeScaleMap[COPY_MAX_Z][COPY_MAX_X][4];
// 2026-05-11: 範囲コピー/貼り付けでも辺の厚さを一緒に扱うため追加。
int CopyCollisionEdgeThicknessMap[COPY_MAX_Z][COPY_MAX_X][4];
// 2026-05-13: 範囲コピー/貼り付けでもコーナー当たり調整値を一緒に扱うため追加。
int CopyCollisionCornerScaleMap[COPY_MAX_Z][COPY_MAX_X];
int CopyCollisionCornerThicknessMap[COPY_MAX_Z][COPY_MAX_X];
int CopyCollisionCornerOffsetMap[COPY_MAX_Z][COPY_MAX_X];

// 2026-05-13: コーナー当たり判定の奥行オフセットを編集・保存するため追加。
int CollisionCornerOffsetMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

#pragma endregion


#pragma region ===== 共通判定 =====

static bool IsMapPosValid(int layer, int z, int x)
{
    return
        layer >= 0 && layer < BLOCK_NUM_Y &&
        z >= 0 && z < BLOCK_NUM_Z &&
        x >= 0 && x < BLOCK_NUM_X;
}

static bool IsHoverValid()
{
    return IsMapPosValid(currentLayer, hoverZ, hoverX);
}

static bool HasValidSelection()
{
    return
        IsMapPosValid(selectLayer, selectStartZ, selectStartX) &&
        IsMapPosValid(selectLayer, selectEndZ, selectEndX);
}

#pragma endregion

#pragma region ===== イベント配置 =====

static void ClearEventId(int eventId)
{
    for (int y = 0; y < BLOCK_NUM_Y; y++)
    {
        for (int z = 0; z < BLOCK_NUM_Z; z++)
        {
            for (int x = 0; x < BLOCK_NUM_X; x++)
            {
                if (EventMap[y][z][x] == eventId)
                    EventMap[y][z][x] = -1;
            }
        }
    }
}

static void PlaceEvent(int layer, int z, int x, int eventId)
{
    if (!IsMapPosValid(layer, z, x))
        return;

    ClearEventId(eventId);

    EventMap[layer][z][x] = eventId;

    if (eventId == 0)
    {
        startX = x;
        startY = layer;
        startZ = z;
    }
    else if (eventId == 1)
    {
        goalX = x;
        goalY = layer;
        goalZ = z;
    }
}

#pragma endregion

#pragma region ===== 座標・回転 =====

static float RotToRad(int rot)
{
    return rot * 3.14159265f * 0.5f;
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
    case 0: pz = z * BLOCK_SIZE; break;
    case 1: px = (x + 1) * BLOCK_SIZE; break;
    case 2: pz = (z + 1) * BLOCK_SIZE; break;
    case 3: px = x * BLOCK_SIZE; break;
    }

    return VGet(px, py, pz);
}

static VECTOR GetModelDrawPosition(int tab, int x, int y, int z, int rot)
{
    if (tab == WALL)
        return GetWallPosition(x, y, z, rot);

    VECTOR pos = GetBlockCenterPosition(x, y, z);

    // 階段だけ位置補正
    if (tab == DECO && DecoMap[y][z][x] == 5)
    {
        float offset = BLOCK_SIZE * -0.25f;

        switch (rot % 4)
        {
        case 0:
            pos.z += offset;
            break;

        case 1:
            pos.x += offset;
            break;

        case 2:
            pos.z -= offset;
            break;

        case 3:
            pos.x -= offset;
            break;
        }
    }

    return pos;
}

#pragma endregion


#pragma region ===== 当たり判定設定 =====

static vector<string> SplitCollisionCSV(const string& line)
{
    // 2026-05-11: 新しいオブジェクトを追加した時に、CSVで既定当たり判定を増やせるよう追加。
    vector<string> result;
    string item;
    stringstream ss(line);

    while (getline(ss, item, ','))
    {
        result.push_back(item);
    }

    return result;
}

static EditorCollisionType ParseEditorCollisionType(const string& text)
{
    // 2026-05-11: collision_config.csv の文字列から当たり判定タイプを選べるよう追加。
    if (text == "CIRCLE") return EDITOR_COLL_CIRCLE;
    if (text == "BOX") return EDITOR_COLL_BOX;
    if (text == "WALL") return EDITOR_COLL_WALL;
    if (text == "STAIRS") return EDITOR_COLL_STAIRS;
    return EDITOR_COLL_NONE;
}

static void InitEditorCollisionTable()
{
    // 2026-05-11: Loader側の既定当たり判定をエディターでも見えるようにするため追加。
    for (int i = 0; i < MODEL_MAX; i++)
    {
        editorCollisionTable[i].type = EDITOR_COLL_NONE;
        editorCollisionTable[i].radius = 0.0f;
        editorCollisionTable[i].width = 0.0f;
        editorCollisionTable[i].depth = 0.0f;
    }

    editorCollisionTable[1].type = EDITOR_COLL_WALL;

    editorCollisionTable[2].type = EDITOR_COLL_CIRCLE;
    editorCollisionTable[2].radius = BLOCK_SIZE * 0.3f;

    editorCollisionTable[5].type = EDITOR_COLL_STAIRS;

    editorCollisionTable[6].type = EDITOR_COLL_BOX;
    editorCollisionTable[6].width = BLOCK_SIZE * 0.3f;
    editorCollisionTable[6].depth = BLOCK_SIZE * 0.3f;

    editorCollisionTable[7].type = EDITOR_COLL_CIRCLE;
    editorCollisionTable[7].radius = BLOCK_SIZE * 0.20f;

    ifstream ifs("collision_config.csv");
    if (!ifs)
        return;

    string line;
    while (getline(ifs, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        vector<string> cols = SplitCollisionCSV(line);
        if (cols.size() < 2 || cols[0] == "id")
            continue;

        int id = stoi(cols[0]);
        if (id < 0 || id >= MODEL_MAX)
            continue;

        editorCollisionTable[id].type = ParseEditorCollisionType(cols[1]);
        editorCollisionTable[id].radius = cols.size() >= 3 ? stof(cols[2]) : 0.0f;
        editorCollisionTable[id].width = cols.size() >= 4 ? stof(cols[3]) : 0.0f;
        editorCollisionTable[id].depth = cols.size() >= 5 ? stof(cols[4]) : 0.0f;
    }
}

#pragma endregion


#pragma region ===== 当たり判定表示補助 =====

static int GetCollisionEdgeBit(int edge)
{
    // 2026-05-11: 1セルに複数の辺当たり判定を持たせるため、方向をビットに変換する処理を追加。
    return 1 << (edge % 4);
}

static void GetCollisionEdgeLine(int x, int z, int edge, int scale, float& x1, float& z1, float& x2, float& z2)
{
    // 2026-05-11: MapLoader.cppの壁ライン判定と同じ位置を、伸縮値込みでエディター可視化するため追加。
    float left = x * BLOCK_SIZE;
    float right = (x + 1) * BLOCK_SIZE;
    float top = z * BLOCK_SIZE;
    float bottom = (z + 1) * BLOCK_SIZE;

    switch (edge % 4)
    {
    case 0:
        x1 = left + BLOCK_SIZE * 0.0f;
        z1 = top + BLOCK_SIZE * 0.12f;
        x2 = right - BLOCK_SIZE * 0.0f;
        z2 = top + BLOCK_SIZE * 0.12f;
        break;

    case 1:
        x1 = right - BLOCK_SIZE * -0.12f;
        z1 = top + BLOCK_SIZE * 0.00f;
        x2 = right - BLOCK_SIZE * -0.12f;
        z2 = bottom - BLOCK_SIZE * 0.00f;
        break;

    case 2:
        x1 = left + BLOCK_SIZE * 0.0f;
        z1 = bottom - BLOCK_SIZE * 0.12f;
        x2 = right - BLOCK_SIZE * 0.0f;
        z2 = bottom - BLOCK_SIZE * 0.12f;
        break;

    case 3:
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
}

static void DrawCollisionEdgeLine(int x, int y, int z, int edge, int scale, int thickness, int color)
{
    // 2026-05-11: 壁モデル由来/手動設定由来の辺当たり判定を線で確認するため追加。
    float x1 = 0.0f;
    float z1 = 0.0f;
    float x2 = 0.0f;
    float z2 = 0.0f;
    float yy = y * BLOCK_SIZE + 70.0f;

    GetCollisionEdgeLine(x, z, edge, scale, x1, z1, x2, z2);
    DrawLine3D(VGet(x1, yy, z1), VGet(x2, yy, z2), color);
    DrawLine3D(VGet(x1, yy + 8.0f, z1), VGet(x2, yy + 8.0f, z2), color);

    // 2026-05-11: 辺当たり判定の厚さを、線に直交する短い目印として見えるよう追加。
    float dx = x2 - x1;
    float dz = z2 - z1;
    float len = sqrtf(dx * dx + dz * dz);
    if (len > 0.0001f)
    {
        float nx = -dz / len;
        float nz = dx / len;
        float half = BLOCK_SIZE * 0.08f * (thickness / 100.0f);
        float cx = (x1 + x2) * 0.5f;
        float cz = (z1 + z2) * 0.5f;
        DrawLine3D(VGet(cx - nx * half, yy + 16.0f, cz - nz * half), VGet(cx + nx * half, yy + 16.0f, cz + nz * half), color);
    }
}

static void DrawCollisionCircle(int x, int y, int z, float radius, int color)
{
    // 2026-05-11: 円形の既定当たり判定を、オブジェクトを置いた時点で直接見えるよう追加。
    const int DIV = 32;
    float cx = x * BLOCK_SIZE + BLOCK_SIZE * 0.5f;
    float cz = z * BLOCK_SIZE + BLOCK_SIZE * 0.5f;
    float yy = y * BLOCK_SIZE + 72.0f;

    for (int i = 0; i < DIV; i++)
    {
        float a1 = DX_PI_F * 2.0f * i / DIV;
        float a2 = DX_PI_F * 2.0f * (i + 1) / DIV;
        DrawLine3D(
            VGet(cx + cosf(a1) * radius, yy, cz + sinf(a1) * radius),
            VGet(cx + cosf(a2) * radius, yy, cz + sinf(a2) * radius),
            color);
    }
}

static void DrawCollisionBox(int x, int y, int z, float halfW, float halfD, int color)
{
    // 2026-05-11: 箱形の既定当たり判定を、オブジェクトを置いた時点で直接見えるよう追加。
    float cx = x * BLOCK_SIZE + BLOCK_SIZE * 0.5f;
    float cz = z * BLOCK_SIZE + BLOCK_SIZE * 0.5f;
    float yy = y * BLOCK_SIZE + 72.0f;

    float left = cx - halfW;
    float right = cx + halfW;
    float top = cz - halfD;
    float bottom = cz + halfD;

    DrawLine3D(VGet(left, yy, top), VGet(right, yy, top), color);
    DrawLine3D(VGet(right, yy, top), VGet(right, yy, bottom), color);
    DrawLine3D(VGet(right, yy, bottom), VGet(left, yy, bottom), color);
    DrawLine3D(VGet(left, yy, bottom), VGet(left, yy, top), color);
}

static void DrawCollisionCornerLine(
    int x,
    int y,
    int z,
    int rot,
    int scale,
    int thickness,
    int offset,
    int color)
{
    // 2026-05-11: CornerMapの斜め当たり判定も、Loaderの判定と同じ形で見えるよう追加。
    float left = x * BLOCK_SIZE;
    float right = (x + 1) * BLOCK_SIZE;
    float top = z * BLOCK_SIZE;
    float bottom = (z + 1) * BLOCK_SIZE;
    float inset = BLOCK_SIZE * 0.12f;
    float yy = y * BLOCK_SIZE + 74.0f;
    float x1 = 0.0f;
    float z1 = 0.0f;
    float x2 = 0.0f;
    float z2 = 0.0f;

    switch (rot % 4)
    {
    case 0:
        x1 = left + inset;
        z1 = top + inset;
        x2 = right - inset;
        z2 = bottom - inset;
        break;
    case 1:
        x1 = right - inset;
        z1 = top + inset;
        x2 = left + inset;
        z2 = bottom - inset;
        break;
    case 2:
        x1 = right - inset;
        z1 = bottom - inset;
        x2 = left + inset;
        z2 = top + inset;
        break;
    case 3:
        x1 = left + inset;
        z1 = bottom - inset;
        x2 = right - inset;
        z2 = top + inset;
        break;
    }

    float centerX = (x1 + x2) * 0.5f;
    float centerZ = (z1 + z2) * 0.5f;

    float rate = scale / 100.0f;

    x1 = centerX + (x1 - centerX) * rate;
    z1 = centerZ + (z1 - centerZ) * rate;
    x2 = centerX + (x2 - centerX) * rate;
    z2 = centerZ + (z2 - centerZ) * rate;

    // 線方向
    float dx = x2 - x1;
    float dz = z2 - z1;
    float len = sqrtf(dx * dx + dz * dz);

    // 奥/手前オフセット
    if (len > 0.0001f)
    {
        float nx = -dz / len;
        float nz = dx / len;

        x1 += nx * offset;
        z1 += nz * offset;

        x2 += nx * offset;
        z2 += nz * offset;
    }

    DrawLine3D(VGet(x1, yy, z1), VGet(x2, yy, z2), color);
    DrawLine3D(VGet(x1, yy + 8.0f, z1), VGet(x2, yy + 8.0f, z2), color);

    // 厚さ表示
    if (len > 0.0001f)
    {
        float nx = -dz / len;
        float nz = dx / len;

        float half = BLOCK_SIZE * 0.08f * (thickness / 100.0f);

        float midX = (x1 + x2) * 0.5f;
        float midZ = (z1 + z2) * 0.5f;

        DrawLine3D(
            VGet(midX - nx * half, yy + 16.0f, midZ - nz * half),
            VGet(midX + nx * half, yy + 16.0f, midZ + nz * half),
            color);
    }
}

static void DrawDefaultObjectCollision(int tab, int id, int x, int y, int z, int rot, int color)
{
    // 2026-05-11: オブジェクトが最初から持っている既定当たり判定を、配置後すぐ見えるよう追加。
    if (id < 0 || id >= MODEL_MAX)
        return;

    if (tab == WALL || editorCollisionTable[id].type == EDITOR_COLL_WALL)
    {
        DrawCollisionEdgeLine(x, y, z, rot, 100, 100, color);
        return;
    }

    if (tab == CORNER)
    {
        DrawCollisionCornerLine(
            x,
            y,
            z,
            rot,
            CollisionCornerScaleMap[y][z][x],
            CollisionCornerThicknessMap[y][z][x],
            CollisionCornerOffsetMap[y][z][x],
            color);
        return;
    }

    if (editorCollisionTable[id].type == EDITOR_COLL_CIRCLE)
    {
        DrawCollisionCircle(x, y, z, editorCollisionTable[id].radius, color);
    }
    else if (editorCollisionTable[id].type == EDITOR_COLL_BOX)
    {
        DrawCollisionBox(x, y, z, editorCollisionTable[id].width, editorCollisionTable[id].depth, color);
    }
}

#pragma endregion


#pragma region ===== コピー処理 =====

static void ClearCopyBuffer()
{
    copySizeX = 0;
    copySizeZ = 0;
    hasCopyData = false;

    for (int z = 0; z < COPY_MAX_Z; z++)
    {
        for (int x = 0; x < COPY_MAX_X; x++)
        {
            CopyFloorMap[z][x] = -1;
            CopyWallMapA[z][x] = -1;
            CopyWallMapB[z][x] = -1;
            CopyCornerMap[z][x] = -1;
            CopyDecoMap[z][x] = -1;
            CopyEventMap[z][x] = -1;
            // 2026-05-11: コピー用バッファ初期化時に当たり判定の古い値を残さないため追加。
            CopyCollisionMap[z][x] = -1;
            // 2026-05-11: コピー用バッファ初期化時に辺当たり判定の古い値を残さないため追加。
            CopyCollisionEdgeMap[z][x] = -1;
            // 2026-05-13: コピー用バッファ初期化時にコーナー当たり調整値の古い値を残さないため追加。
            CopyCollisionCornerScaleMap[z][x] = 100;
            CopyCollisionCornerThicknessMap[z][x] = 100;
            CopyCollisionCornerOffsetMap[z][x] = 0;
            for (int edge = 0; edge < 4; edge++)
            {
                CopyCollisionEdgeScaleMap[z][x][edge] = 100;
                CopyCollisionEdgeThicknessMap[z][x][edge] = 100;
            }

            CopyFloorRot[z][x] = 0;
            CopyWallRotA[z][x] = 0;
            CopyWallRotB[z][x] = 0;
            CopyCornerRot[z][x] = 0;
            CopyDecoRot[z][x] = 0;
        }
    }
}

static void CopySelection()
{
    if (!HasValidSelection())
        return;

    int minX = min(selectStartX, selectEndX);
    int maxX = max(selectStartX, selectEndX);
    int minZ = min(selectStartZ, selectEndZ);
    int maxZ = max(selectStartZ, selectEndZ);

    ClearCopyBuffer();

    copySizeX = maxX - minX + 1;
    copySizeZ = maxZ - minZ + 1;

    for (int z = 0; z < copySizeZ; z++)
    {
        for (int x = 0; x < copySizeX; x++)
        {
            int srcX = minX + x;
            int srcZ = minZ + z;

            CopyFloorMap[z][x] = FloorMap[selectLayer][srcZ][srcX];
            CopyFloorRot[z][x] = FloorRot[selectLayer][srcZ][srcX];

            CopyWallMapA[z][x] = WallMapA[selectLayer][srcZ][srcX];
            CopyWallMapB[z][x] = WallMapB[selectLayer][srcZ][srcX];
            CopyWallRotA[z][x] = WallRotA[selectLayer][srcZ][srcX];
            CopyWallRotB[z][x] = WallRotB[selectLayer][srcZ][srcX];

            CopyCornerMap[z][x] = CornerMap[selectLayer][srcZ][srcX];
            CopyCornerRot[z][x] = CornerRot[selectLayer][srcZ][srcX];

            CopyDecoMap[z][x] = DecoMap[selectLayer][srcZ][srcX];
            CopyDecoRot[z][x] = DecoRot[selectLayer][srcZ][srcX];

            CopyEventMap[z][x] = EventMap[selectLayer][srcZ][srcX];
            // 2026-05-11: 範囲コピー時に手動当たり判定も一緒に保持するため追加。
            CopyCollisionMap[z][x] = CollisionMap[selectLayer][srcZ][srcX];
            // 2026-05-11: 範囲コピー時に辺当たり判定も一緒に保持するため追加。
            CopyCollisionEdgeMap[z][x] = CollisionEdgeMap[selectLayer][srcZ][srcX];
            // 2026-05-13: 範囲コピー時にコーナー当たり調整値も一緒に保持するため追加。
            CopyCollisionCornerScaleMap[z][x] = CollisionCornerScaleMap[selectLayer][srcZ][srcX];
            CopyCollisionCornerThicknessMap[z][x] = CollisionCornerThicknessMap[selectLayer][srcZ][srcX];
            CopyCollisionCornerOffsetMap[z][x] = CollisionCornerOffsetMap[selectLayer][srcZ][srcX];
            for (int edge = 0; edge < 4; edge++)
            {
                CopyCollisionEdgeScaleMap[z][x][edge] = CollisionEdgeScaleMap[selectLayer][srcZ][srcX][edge];
                CopyCollisionEdgeThicknessMap[z][x][edge] = CollisionEdgeThicknessMap[selectLayer][srcZ][srcX][edge];
            }
        }
    }

    hasCopyData = true;
}

static void PasteSelection(int dstLayer, int dstX, int dstZ)
{
    if (!hasCopyData)
        return;

    for (int z = 0; z < copySizeZ; z++)
    {
        for (int x = 0; x < copySizeX; x++)
        {
            int targetX = dstX + x;
            int targetZ = dstZ + z;

            if (!IsMapPosValid(dstLayer, targetZ, targetX))
                continue;

            FloorMap[dstLayer][targetZ][targetX] = CopyFloorMap[z][x];
            FloorRot[dstLayer][targetZ][targetX] = CopyFloorRot[z][x];

            WallMapA[dstLayer][targetZ][targetX] = CopyWallMapA[z][x];
            WallMapB[dstLayer][targetZ][targetX] = CopyWallMapB[z][x];
            WallRotA[dstLayer][targetZ][targetX] = CopyWallRotA[z][x];
            WallRotB[dstLayer][targetZ][targetX] = CopyWallRotB[z][x];

            CornerMap[dstLayer][targetZ][targetX] = CopyCornerMap[z][x];
            CornerRot[dstLayer][targetZ][targetX] = CopyCornerRot[z][x];

            DecoMap[dstLayer][targetZ][targetX] = CopyDecoMap[z][x];
            DecoRot[dstLayer][targetZ][targetX] = CopyDecoRot[z][x];

            EventMap[dstLayer][targetZ][targetX] = CopyEventMap[z][x];
            // 2026-05-11: 貼り付け時に手動当たり判定も同じ位置へ復元するため追加。
            CollisionMap[dstLayer][targetZ][targetX] = CopyCollisionMap[z][x];
            // 2026-05-11: 貼り付け時に辺当たり判定も同じ位置へ復元するため追加。
            CollisionEdgeMap[dstLayer][targetZ][targetX] = CopyCollisionEdgeMap[z][x];
            // 2026-05-13: 貼り付け時にコーナー当たり調整値も同じ位置へ復元するため追加。
            CollisionCornerScaleMap[dstLayer][targetZ][targetX] = CopyCollisionCornerScaleMap[z][x];
            CollisionCornerThicknessMap[dstLayer][targetZ][targetX] = CopyCollisionCornerThicknessMap[z][x];
            CollisionCornerOffsetMap[dstLayer][targetZ][targetX] = CopyCollisionCornerOffsetMap[z][x];
            for (int edge = 0; edge < 4; edge++)
            {
                CollisionEdgeScaleMap[dstLayer][targetZ][targetX][edge] = CopyCollisionEdgeScaleMap[z][x][edge];
                CollisionEdgeThicknessMap[dstLayer][targetZ][targetX][edge] = CopyCollisionEdgeThicknessMap[z][x][edge];
            }
        }
    }
}

#pragma endregion


#pragma region ===== 選択モデル =====

int GetSelectedModel()
{
    if (currentTab < 0 || currentTab >= TAB_MAX)
        return 0;

    if (selectedModel < 0 || selectedModel >= tabModelCount[currentTab])
        return 0;

    return tabModelList[currentTab][selectedModel];
}

#pragma endregion


#pragma region ===== マップ初期化 =====

void ClearCurrentLayer()
{
    int layer = currentLayer;

    if (layer < 0 || layer >= BLOCK_NUM_Y)
        return;

    for (int z = 0; z < BLOCK_NUM_Z; z++)
    {
        for (int x = 0; x < BLOCK_NUM_X; x++)
        {
            FloorMap[layer][z][x] = -1;
            WallMapA[layer][z][x] = -1;
            WallMapB[layer][z][x] = -1;
            CornerMap[layer][z][x] = -1;
            DecoMap[layer][z][x] = -1;
            if (EventMap[layer][z][x] == 0)
            {
                startX = 0; startY = 0; startZ = 0;
            }
            else if (EventMap[layer][z][x] == 1)
            {
                goalX = 5; goalY = 0; goalZ = 5;
            }
            EventMap[layer][z][x] = -1;
            // 2026-05-11: レイヤークリア時に手動当たり判定も消すため追加。
            CollisionMap[layer][z][x] = -1;
            // 2026-05-11: レイヤークリア時に辺当たり判定も消すため追加。
            CollisionEdgeMap[layer][z][x] = -1;
            for (int edge = 0; edge < 4; edge++)
            {
                CollisionEdgeScaleMap[layer][z][x][edge] = 100;
                CollisionEdgeThicknessMap[layer][z][x][edge] = 100;

                CollisionCornerScaleMap[layer][z][x] = 100;
                CollisionCornerThicknessMap[layer][z][x] = 100;
                CollisionCornerOffsetMap[layer][z][x] = 0;
            }

            FloorRot[layer][z][x] = 0;
            WallRotA[layer][z][x] = 0;
            WallRotB[layer][z][x] = 0;
            CornerRot[layer][z][x] = 0;
            DecoRot[layer][z][x] = 0;
        }
    }
}

void ResetAllMap()
{
    // 2026-05-13: マップ読込/リセット時に前の開始・ゴール座標が残らないよう追加。
    startX = 0; startY = 0; startZ = 0;
    goalX = 5; goalY = 0; goalZ = 5;
    for (int y = 0; y < BLOCK_NUM_Y; y++)
    {
        for (int z = 0; z < BLOCK_NUM_Z; z++)
        {
            for (int x = 0; x < BLOCK_NUM_X; x++)
            {
                FloorMap[y][z][x] = -1;
                WallMapA[y][z][x] = -1;
                WallMapB[y][z][x] = -1;
                CornerMap[y][z][x] = -1;
                DecoMap[y][z][x] = -1;
                EventMap[y][z][x] = -1;
                // 2026-05-11: 全体リセット時に手動当たり判定も初期化するため追加。
                CollisionMap[y][z][x] = -1;
                // 2026-05-11: 全体リセット時に辺当たり判定も初期化するため追加。
                CollisionEdgeMap[y][z][x] = -1;
                for (int edge = 0; edge < 4; edge++)
                {
                    CollisionEdgeScaleMap[y][z][x][edge] = 100;
                    CollisionEdgeThicknessMap[y][z][x][edge] = 100;

                    // 2026-05-13: 全消去/読込初期化時にコーナー当たり調整値の古い値を残さないため追加。
                    CollisionCornerScaleMap[y][z][x] = 100;
                    CollisionCornerThicknessMap[y][z][x] = 100;
                    CollisionCornerOffsetMap[y][z][x] = 0;

                }

                FloorRot[y][z][x] = 0;
                WallRotA[y][z][x] = 0;
                WallRotB[y][z][x] = 0;
                CornerRot[y][z][x] = 0;
                DecoRot[y][z][x] = 0;
            }
        }
    }
}

#pragma endregion


#pragma region ===== 初期化 =====

void InitEditor()
{
    ResetAllMap();
    ClearCopyBuffer();

    for (int i = 0; i < MODEL_MAX; i++)
    {
        modelHandles[i] = -1;
        paletteTex[i] = -1;
    }

    modelHandles[0] = MV1LoadModel("model/floor.mv1");
    modelHandles[1] = MV1LoadModel("model/wall.mv1");
    modelHandles[2] = MV1LoadModel("model/corner.mv1");
    modelHandles[3] = MV1LoadModel("model/wall_top.mv1");
    modelHandles[4] = MV1LoadModel("model/wall_detail.mv1");
    modelHandles[5] = MV1LoadModel("model/stairs.mv1");
    modelHandles[6] = MV1LoadModel("model/floor_layer.mv1");
    modelHandles[7] = MV1LoadModel("model/floor_layer-raised.mv1");

    paletteTex[0] = LoadGraph("ui/floor.png");
    paletteTex[1] = LoadGraph("ui/wall.png");
    paletteTex[2] = LoadGraph("ui/corner.png");
    paletteTex[3] = LoadGraph("ui/wall_top.png");
    paletteTex[4] = LoadGraph("ui/wall2.png");
    paletteTex[5] = LoadGraph("ui/stairs.png");
    paletteTex[6] = LoadGraph("ui/tree.png");
    paletteTex[7] = LoadGraph("ui/barrel.png");

    for (int i = 0; i < 8; i++)
    {
        if (modelHandles[i] == -1)
            printfDx("model load failed: %d\n", i);
    }

    InitEditorCollisionTable();
}

#pragma endregion


#pragma region ===== 更新 =====

void UpdateEditor()
{

    int mx = 0;
    int my = 0;
    GetMousePoint(&mx, &my);

    int mouse = GetMouseInput();
    int lClick = mouse & MOUSE_INPUT_LEFT;
    int rClick = mouse & MOUSE_INPUT_RIGHT;

    bool lTrigger = lClick && !(oldMouse & MOUSE_INPUT_LEFT);
    bool rTrigger = rClick && !(oldMouse & MOUSE_INPUT_RIGHT);

    bool ctrl =
        CheckHitKey(KEY_INPUT_LCONTROL) ||
        CheckHitKey(KEY_INPUT_RCONTROL);

    bool shift =
        CheckHitKey(KEY_INPUT_LSHIFT) ||
        CheckHitKey(KEY_INPUT_RSHIFT);

    int uiX = SCREEN_W - UI_WIDTH;
    bool isUI = (mx >= uiX);

#pragma region カメラ操作

    if (shift && lClick)
    {
        camRotY += (mx - oldMX) * 0.01f;
        camRotX += (my - oldMY) * 0.01f;

        if (camRotX > 1.2f) camRotX = 1.2f;
        if (camRotX < -1.2f) camRotX = -1.2f;
    }

    oldMX = mx;
    oldMY = my;

    int wheel = GetMouseWheelRotVol();
    if (!(collisionEditMode && collisionEdgeEditMode))
        camDist -= wheel * 300.0f;
    camDist = max(1000.0f, min(10000.0f, camDist));

#pragma endregion


#pragma region キー操作

    if (ctrl && Input::IsKeyTrigger(KEY_INPUT_Z))
    {
        UndoMap();
        selecting = false;
        lastBrushX = -1;
        lastBrushZ = -1;
        lastBrushLayer = -1;
    }

    if (Input::IsKeyTrigger(KEY_INPUT_Q))
        currentLayer = max(0, currentLayer - 1);

    if (Input::IsKeyTrigger(KEY_INPUT_E))
        currentLayer = min(BLOCK_NUM_Y - 1, currentLayer + 1);

    if (Input::IsKeyTrigger(KEY_INPUT_R))
        currentRot = (currentRot + 1) % 4;

    if (Input::IsKeyTrigger(KEY_INPUT_B))
        brushMode = !brushMode;

    if (!ctrl && Input::IsKeyTrigger(KEY_INPUT_V))
    {
        selectMode = !selectMode;
        selecting = false;
    }

    if (!ctrl && Input::IsKeyTrigger(KEY_INPUT_C))
    {
        PushUndo();
        ClearCurrentLayer();
    }

    if (Input::IsKeyTrigger(KEY_INPUT_RIGHT))
    {
        currentMapIndex++;
        LoadMap(currentMapIndex);
    }

    if (Input::IsKeyTrigger(KEY_INPUT_LEFT))
    {
        if (currentMapIndex > 1)
            currentMapIndex--;

        LoadMap(currentMapIndex);
    }

    if (Input::IsKeyTrigger(KEY_INPUT_F5))
        SaveMap();

    if (Input::IsKeyTrigger(KEY_INPUT_F9))
        LoadMap(currentMapIndex);

#pragma endregion


#pragma region UIクリック

    if (lTrigger && isUI)
        UpdateEditorUI(mx, my, true);

#pragma endregion


#pragma region レイキャスト

    hoverX = -1;
    hoverZ = -1;

    if (!isUI)
    {
        VECTOR nearPos = ConvScreenPosToWorldPos(VGet((float)mx, (float)my, 0.0f));
        VECTOR farPos = ConvScreenPosToWorldPos(VGet((float)mx, (float)my, 1.0f));

        float targetY = currentLayer * BLOCK_SIZE;
        float dy = farPos.y - nearPos.y;

        if (fabsf(dy) > 0.0001f)
        {
            float t = (targetY - nearPos.y) / dy;

            if (t >= 0.0f && t <= 1.0f)
            {
                VECTOR hit = VGet(0.0f, targetY, 0.0f);

                hit.x = nearPos.x + (farPos.x - nearPos.x) * t;
                hit.z = nearPos.z + (farPos.z - nearPos.z) * t;

                hoverX = (int)floorf(hit.x / BLOCK_SIZE);
                hoverZ = (int)floorf(hit.z / BLOCK_SIZE);
            }
        }
    }

#pragma endregion

    int layer = currentLayer;
    int x = hoverX;
    int z = hoverZ;
    bool canAccess = IsMapPosValid(layer, z, x);


#pragma region コピー / 貼り付け

    if (ctrl && Input::IsKeyTrigger(KEY_INPUT_C))
        CopySelection();

    if (ctrl && Input::IsKeyTrigger(KEY_INPUT_V) && canAccess)
    {
        PushUndo();
        PasteSelection(layer, x, z);
    }

#pragma endregion


#pragma region 範囲選択

    if (selectMode && !isUI && canAccess)
    {
        if (lTrigger)
        {
            selecting = true;
            selectLayer = layer;
            selectStartX = x;
            selectStartZ = z;
            selectEndX = x;
            selectEndZ = z;
        }

        if (selecting && lClick)
        {
            selectEndX = x;
            selectEndZ = z;
        }

        if (selecting && !lClick)
            selecting = false;
    }

#pragma endregion


#pragma region 当たり判定伸縮編集

    // 2026-05-13: ホイール編集を長さモード/厚みモードで切り替え、範囲外セルを触らないよう修正。
    if (collisionEditMode && collisionEdgeEditMode && !selectMode && !isUI && canAccess && wheel != 0)
    {
        if (CornerMap[layer][z][x] >= 0)
        {
            if (collisionDepthEditMode)
            {
                int thick = CollisionCornerThicknessMap[layer][z][x] + wheel * 10;
                thick = max(20, min(500, thick));
                CollisionCornerThicknessMap[layer][z][x] = thick;
            }
            else
            {
                int scale = CollisionCornerScaleMap[layer][z][x] + wheel * 10;
                scale = max(20, min(200, scale));
                CollisionCornerScaleMap[layer][z][x] = scale;
            }
        }
        else
        {
            int mask = CollisionEdgeMap[layer][z][x] >= 0 ? CollisionEdgeMap[layer][z][x] : 0;
            CollisionEdgeMap[layer][z][x] = mask | GetCollisionEdgeBit(currentRot);

            if (collisionDepthEditMode)
            {
                int thick = CollisionEdgeThicknessMap[layer][z][x][currentRot] + wheel * 10;
                thick = max(20, min(300, thick));
                CollisionEdgeThicknessMap[layer][z][x][currentRot] = thick;
            }
            else
            {
                int scale = CollisionEdgeScaleMap[layer][z][x][currentRot] + wheel * 10;
                scale = max(20, min(200, scale));
                CollisionEdgeScaleMap[layer][z][x][currentRot] = scale;
            }
        }
    }
#pragma endregion

#pragma region 当たり判定編集

    // 2026-05-11: セル単位/辺単位の手動当たり判定をエディターで直接置けるよう追加。
    if (collisionEditMode && !selectMode && !shift && !isUI && canAccess)
    {
        if (lTrigger)
        {
            PushUndo();
             
            if (collisionEdgeEditMode)
            {
                int mask = CollisionEdgeMap[layer][z][x] >= 0 ? CollisionEdgeMap[layer][z][x] : 0;
                CollisionEdgeMap[layer][z][x] = mask | GetCollisionEdgeBit(currentRot);
                CollisionEdgeScaleMap[layer][z][x][currentRot] = 100;
                CollisionEdgeThicknessMap[layer][z][x][currentRot] = 100;
            }
            else
            {
                CollisionMap[layer][z][x] = 1;
            }
        }
        else if (rTrigger)
        {
            PushUndo();

            if (collisionEdgeEditMode)
            {
                int mask = CollisionEdgeMap[layer][z][x] >= 0 ? CollisionEdgeMap[layer][z][x] : 0;
                mask &= ~GetCollisionEdgeBit(currentRot);
                CollisionEdgeMap[layer][z][x] = mask > 0 ? mask : -1;
            }
            else
            {
                CollisionMap[layer][z][x] = -1;
            }
        }
    }

#pragma endregion
#pragma region 配置

    bool brushPlace =
        brushMode &&
        lClick &&
        canAccess &&
        (x != lastBrushX || z != lastBrushZ || layer != lastBrushLayer);

    bool singlePlace =
        !brushMode &&
        lTrigger &&
        canAccess;

    if (!collisionEditMode && !selectMode && !shift && !isUI && (singlePlace || brushPlace))
    {
        PushUndo();

        int model = GetSelectedModel();

        if (currentTab == FLOOR)
        {
            FloorMap[layer][z][x] = model;
            FloorRot[layer][z][x] = currentRot;
        }
        else if (currentTab == WALL)
        {
            if (WallMapA[layer][z][x] == -1)
            {
                WallMapA[layer][z][x] = model;
                WallRotA[layer][z][x] = currentRot;
            }
            else if (WallMapB[layer][z][x] == -1)
            {
                WallMapB[layer][z][x] = model;
                WallRotB[layer][z][x] = currentRot;
            }
            else
            {
                WallMapB[layer][z][x] = model;
                WallRotB[layer][z][x] = currentRot;
            }
        }
        else if (currentTab == CORNER)
        {
            CornerMap[layer][z][x] = model;
            CornerRot[layer][z][x] = currentRot;
            // 2026-05-13: コーナー配置時に当たり判定調整値を標準値へ戻すため追加。
            CollisionCornerScaleMap[layer][z][x] = 100;
            CollisionCornerThicknessMap[layer][z][x] = 100;
            CollisionCornerOffsetMap[layer][z][x] = 0;
        }
        else if (currentTab == DECO)
        {
            DecoMap[layer][z][x] = model;
            DecoRot[layer][z][x] = currentRot;
        }
        else if (currentTab == EVENT)
        {
            PlaceEvent(layer, z, x, model);
        }

        lastBrushX = x;
        lastBrushZ = z;
        lastBrushLayer = layer;
    }

    if (!lClick)
    {
        lastBrushX = -1;
        lastBrushZ = -1;
        lastBrushLayer = -1;
    }

#pragma endregion


#pragma region 削除

    if (!collisionEditMode && !selectMode && !shift && !isUI && rTrigger && canAccess)
    {
        PushUndo();

        if (currentTab == FLOOR)
        {
            FloorMap[layer][z][x] = -1;
            FloorRot[layer][z][x] = 0;
        }
        else if (currentTab == WALL)
        {
            WallMapA[layer][z][x] = -1;
            WallMapB[layer][z][x] = -1;
            WallRotA[layer][z][x] = 0;
            WallRotB[layer][z][x] = 0;
        }
        else if (currentTab == CORNER)
        {
            CornerMap[layer][z][x] = -1;
            CornerRot[layer][z][x] = 0;
            // 2026-05-13: コーナー削除時に当たり判定調整値も標準値へ戻すため追加。
            CollisionCornerScaleMap[layer][z][x] = 100;
            CollisionCornerThicknessMap[layer][z][x] = 100;
            CollisionCornerOffsetMap[layer][z][x] = 0;
        }
        else if (currentTab == DECO)
        {
            DecoMap[layer][z][x] = -1;
            DecoRot[layer][z][x] = 0;
        }
        else if (currentTab == EVENT)
        {
            if (EventMap[layer][z][x] == 0)
            {
                startX = 0; startY = 0; startZ = 0;
            }
            else if (EventMap[layer][z][x] == 1)
            {
                goalX = 5; goalY = 0; goalZ = 5;
            }
            EventMap[layer][z][x] = -1;
        }
    }

#pragma endregion

    oldMouse = mouse;
    oldClick = lClick;
}

#pragma endregion


#pragma region ===== 描画 =====

void DrawEditor()
{
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    // 2026-05-11: 当たり判定編集中に線を見やすくするため、モデルを少し透かす値を追加。
    float editorModelOpacity = (showCollisionDebug || collisionEditMode) ? 0.35f : 1.0f;

    float mapCenterX = BLOCK_NUM_X * BLOCK_SIZE * 0.5f;
    float mapCenterZ = BLOCK_NUM_Z * BLOCK_SIZE * 0.5f;

    float camX = sinf(camRotY) * cosf(camRotX) * camDist + mapCenterX;
    float camZ = cosf(camRotY) * cosf(camRotX) * camDist + mapCenterZ;
    float camY = 2000.0f + sinf(camRotX) * camDist;

    SetCameraPositionAndTarget_UpVecY(
        VGet(camX, camY, camZ),
        VGet(mapCenterX, 0.0f, mapCenterZ));

#pragma region グリッド描画

    if (showGrid)
    {
        for (int z = 0; z <= BLOCK_NUM_Z; z++)
        {
            DrawLine3D(
                VGet(0.0f, 0.0f, z * BLOCK_SIZE),
                VGet(BLOCK_NUM_X * BLOCK_SIZE, 0.0f, z * BLOCK_SIZE),
                GetColor(80, 80, 80));
        }

        for (int x = 0; x <= BLOCK_NUM_X; x++)
        {
            DrawLine3D(
                VGet(x * BLOCK_SIZE, 0.0f, 0.0f),
                VGet(x * BLOCK_SIZE, 0.0f, BLOCK_NUM_Z * BLOCK_SIZE),
                GetColor(80, 80, 80));
        }
    }

#pragma endregion


#pragma region モデル描画

    for (int y = 0; y < BLOCK_NUM_Y; y++)
    {
        for (int z = 0; z < BLOCK_NUM_Z; z++)
        {
            for (int x = 0; x < BLOCK_NUM_X; x++)
            {
                if (FloorMap[y][z][x] >= 0)
                {
                    int id = FloorMap[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(FLOOR, x, y, z, FloorRot[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(FloorRot[y][z][x]), 0.0f));
                        MV1SetOpacityRate(modelHandles[id], editorModelOpacity);
                        MV1DrawModel(modelHandles[id]);
                        MV1SetOpacityRate(modelHandles[id], 1.0f);
                    }
                }

                if (WallMapA[y][z][x] >= 0)
                {
                    int id = WallMapA[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(WALL, x, y, z, WallRotA[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(WallRotA[y][z][x]), 0.0f));
                        MV1SetOpacityRate(modelHandles[id], editorModelOpacity);
                        MV1DrawModel(modelHandles[id]);
                        MV1SetOpacityRate(modelHandles[id], 1.0f);
                    }
                }

                if (WallMapB[y][z][x] >= 0)
                {
                    int id = WallMapB[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(WALL, x, y, z, WallRotB[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(WallRotB[y][z][x]), 0.0f));
                        MV1SetOpacityRate(modelHandles[id], editorModelOpacity);
                        MV1DrawModel(modelHandles[id]);
                        MV1SetOpacityRate(modelHandles[id], 1.0f);
                    }
                }

                if (CornerMap[y][z][x] >= 0)
                {
                    int id = CornerMap[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(CORNER, x, y, z, CornerRot[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(CornerRot[y][z][x]), 0.0f));
                        MV1SetOpacityRate(modelHandles[id], editorModelOpacity);
                        MV1DrawModel(modelHandles[id]);
                        MV1SetOpacityRate(modelHandles[id], 1.0f);
                    }
                }

                if (DecoMap[y][z][x] >= 0)
                {
                    int id = DecoMap[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(DECO, x, y, z, DecoRot[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(DecoRot[y][z][x]), 0.0f));
                        MV1SetOpacityRate(modelHandles[id], editorModelOpacity);
                        MV1DrawModel(modelHandles[id]);
                        MV1SetOpacityRate(modelHandles[id], 1.0f);
                    }
                }
            }
        }
    }

#pragma endregion


#pragma region プレビュー描画

    if (IsHoverValid())
    {
        int previewId = GetSelectedModel();

        if (previewId >= 0 && previewId < MODEL_MAX && modelHandles[previewId] != -1)
        {
            VECTOR pos = GetModelDrawPosition(
                currentTab,
                hoverX,
                currentLayer,
                hoverZ,
                currentRot
            );

            pos.y += 8.0f;

            MV1SetPosition(modelHandles[previewId], pos);
            MV1SetRotationXYZ(modelHandles[previewId], VGet(0.0f, RotToRad(currentRot), 0.0f));
            // 2026-05-11: 当たり判定編集中に配置プレビューも線を隠さないよう透過するため追加。
            MV1SetOpacityRate(modelHandles[previewId], editorModelOpacity);
            MV1DrawModel(modelHandles[previewId]);
            MV1SetOpacityRate(modelHandles[previewId], 1.0f);
        }
    }

#pragma endregion


#pragma region 当たり判定デバッグ描画

    // 2026-05-11: 当たり判定をモデルや配置プレビューに隠されず直接見えるよう、最後にZバッファなしで描画するため追加。
    if (showCollisionDebug || collisionEditMode)
    {
        SetUseZBuffer3D(FALSE);
        SetWriteZBuffer3D(FALSE);

        for (int y = 0; y < BLOCK_NUM_Y; y++)
        {
            for (int z = 0; z < BLOCK_NUM_Z; z++)
            {
                for (int x = 0; x < BLOCK_NUM_X; x++)
                {
                    if (CollisionMap[y][z][x] >= 0)
                    {
                        float x1 = x * BLOCK_SIZE;
                        float z1 = z * BLOCK_SIZE;
                        float x2 = (x + 1) * BLOCK_SIZE;
                        float z2 = (z + 1) * BLOCK_SIZE;
                        float yy = y * BLOCK_SIZE + 55.0f;
                        int col = GetColor(255, 60, 60);

                        DrawLine3D(VGet(x1, yy, z1), VGet(x2, yy, z1), col);
                        DrawLine3D(VGet(x2, yy, z1), VGet(x2, yy, z2), col);
                        DrawLine3D(VGet(x2, yy, z2), VGet(x1, yy, z2), col);
                        DrawLine3D(VGet(x1, yy, z2), VGet(x1, yy, z1), col);
                        DrawLine3D(VGet(x1, yy, z1), VGet(x2, yy, z2), col);
                        DrawLine3D(VGet(x2, yy, z1), VGet(x1, yy, z2), col);
                    }

                    if (WallMapA[y][z][x] >= 0)
                        DrawDefaultObjectCollision(WALL, WallMapA[y][z][x], x, y, z, WallRotA[y][z][x], GetColor(255, 170, 40));

                    if (WallMapB[y][z][x] >= 0)
                        DrawDefaultObjectCollision(WALL, WallMapB[y][z][x], x, y, z, WallRotB[y][z][x], GetColor(255, 170, 40));

                    if (CornerMap[y][z][x] >= 0)
                        DrawDefaultObjectCollision(CORNER, CornerMap[y][z][x], x, y, z, CornerRot[y][z][x], GetColor(255, 170, 40));

                    if (DecoMap[y][z][x] >= 0)
                        DrawDefaultObjectCollision(DECO, DecoMap[y][z][x], x, y, z, DecoRot[y][z][x], GetColor(255, 170, 40));

                    int edgeMask = CollisionEdgeMap[y][z][x] >= 0 ? CollisionEdgeMap[y][z][x] : 0;
                    for (int edge = 0; edge < 4; edge++)
                    {
                        if (edgeMask & GetCollisionEdgeBit(edge))
                            DrawCollisionEdgeLine(x, y, z, edge, CollisionEdgeScaleMap[y][z][x][edge], CollisionEdgeThicknessMap[y][z][x][edge], GetColor(40, 220, 255));
                    }
                }
            }
        }

        // 2026-05-11: いま持っている配置予定オブジェクトの既定当たり判定も、置く前から直接見えるよう追加。
        if (IsHoverValid())
        {
            DrawDefaultObjectCollision(currentTab, GetSelectedModel(), hoverX, currentLayer, hoverZ, currentRot, GetColor(255, 255, 0));
        }
    }

    // 2026-05-11: 辺編集時に、いま置こうとしている辺を黄色でプレビューするため追加。
    if (collisionEditMode && collisionEdgeEditMode && IsHoverValid())
    {
        DrawCollisionEdgeLine(hoverX, currentLayer, hoverZ, currentRot, CollisionEdgeScaleMap[currentLayer][hoverZ][hoverX][currentRot], CollisionEdgeThicknessMap[currentLayer][hoverZ][hoverX][currentRot], GetColor(255, 255, 0));
    }

#pragma endregion
#pragma region ハイライト描画

    if (IsHoverValid())
    {
        float x = hoverX * BLOCK_SIZE;
        float z = hoverZ * BLOCK_SIZE;
        float y = currentLayer * BLOCK_SIZE + 5.0f;

        DrawLine3D(VGet(x, y, z), VGet(x + BLOCK_SIZE, y, z), GetColor(255, 255, 0));
        DrawLine3D(VGet(x + BLOCK_SIZE, y, z), VGet(x + BLOCK_SIZE, y, z + BLOCK_SIZE), GetColor(255, 255, 0));
        DrawLine3D(VGet(x + BLOCK_SIZE, y, z + BLOCK_SIZE), VGet(x, y, z + BLOCK_SIZE), GetColor(255, 255, 0));
        DrawLine3D(VGet(x, y, z + BLOCK_SIZE), VGet(x, y, z), GetColor(255, 255, 0));
    }

#pragma endregion


#pragma region 選択範囲描画

    if (HasValidSelection())
    {
        int minX = min(selectStartX, selectEndX);
        int maxX = max(selectStartX, selectEndX);
        int minZ = min(selectStartZ, selectEndZ);
        int maxZ = max(selectStartZ, selectEndZ);

        float x1 = minX * BLOCK_SIZE;
        float z1 = minZ * BLOCK_SIZE;
        float x2 = (maxX + 1) * BLOCK_SIZE;
        float z2 = (maxZ + 1) * BLOCK_SIZE;
        float y = selectLayer * BLOCK_SIZE + 15.0f;

        DrawLine3D(VGet(x1, y, z1), VGet(x2, y, z1), GetColor(0, 255, 255));
        DrawLine3D(VGet(x2, y, z1), VGet(x2, y, z2), GetColor(0, 255, 255));
        DrawLine3D(VGet(x2, y, z2), VGet(x1, y, z2), GetColor(0, 255, 255));
        DrawLine3D(VGet(x1, y, z2), VGet(x1, y, z1), GetColor(0, 255, 255));
    }

#pragma endregion
#pragma region 開始 / ゴール描画

    if (IsMapPosValid(startY, startZ, startX))
    {
        float x = startX * BLOCK_SIZE;
        float z = startZ * BLOCK_SIZE;
        float y = startY * BLOCK_SIZE + 25.0f;

        DrawLine3D(VGet(x, y, z), VGet(x + BLOCK_SIZE, y, z), GetColor(0, 255, 0));
        DrawLine3D(VGet(x + BLOCK_SIZE, y, z), VGet(x + BLOCK_SIZE, y, z + BLOCK_SIZE), GetColor(0, 255, 0));
        DrawLine3D(VGet(x + BLOCK_SIZE, y, z + BLOCK_SIZE), VGet(x, y, z + BLOCK_SIZE), GetColor(0, 255, 0));
        DrawLine3D(VGet(x, y, z + BLOCK_SIZE), VGet(x, y, z), GetColor(0, 255, 0));
    }

    if (IsMapPosValid(goalY, goalZ, goalX))
    {
        float x = goalX * BLOCK_SIZE;
        float z = goalZ * BLOCK_SIZE;
        float y = goalY * BLOCK_SIZE + 35.0f;

        DrawLine3D(VGet(x, y, z), VGet(x + BLOCK_SIZE, y, z), GetColor(255, 0, 0));
        DrawLine3D(VGet(x + BLOCK_SIZE, y, z), VGet(x + BLOCK_SIZE, y, z + BLOCK_SIZE), GetColor(255, 0, 0));
        DrawLine3D(VGet(x + BLOCK_SIZE, y, z + BLOCK_SIZE), VGet(x, y, z + BLOCK_SIZE), GetColor(255, 0, 0));
        DrawLine3D(VGet(x, y, z + BLOCK_SIZE), VGet(x, y, z), GetColor(255, 0, 0));
    }

#pragma endregion



#pragma region UI / デバッグ描画

    DrawEditorUI();

    /* DrawFormatString(0, 40, GetColor(255, 255, 255),
         "map=%d layer=%d rot=%d tab=%d model=%d brush=%s",
         currentMapIndex, currentLayer, currentRot, currentTab, GetSelectedModel(),
         brushMode ? "ON" : "OFF");

     DrawFormatString(0, 60, GetColor(255, 255, 0),
         "hoverX=%d hoverZ=%d  F5:Save F9:Load",
         hoverX, hoverZ);

     DrawFormatString(0, 80, GetColor(0, 255, 255),
         "select=%s copy=%s size=%d,%d  V:Select Ctrl+C/V/Z",
         selectMode ? "ON" : "OFF",
         hasCopyData ? "YES" : "NO",
         copySizeX, copySizeZ);
         */
#pragma endregion
}

#pragma endregion
