#include "MapEditor.h"
#include "Input.h"
#include <algorithm>
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

#pragma endregion


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

    return GetBlockCenterPosition(x, y, z);
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
            EventMap[layer][z][x] = -1;

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

    camDist -= GetMouseWheelRotVol() * 300.0f;
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

    if (!selectMode && !shift && !isUI && (singlePlace || brushPlace))
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

    if (!selectMode && !shift && !isUI && rTrigger && canAccess)
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
        }
        else if (currentTab == DECO)
        {
            DecoMap[layer][z][x] = -1;
            DecoRot[layer][z][x] = 0;
        }
        else if (currentTab == EVENT)
        {
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
                        MV1DrawModel(modelHandles[id]);
                    }
                }

                if (WallMapA[y][z][x] >= 0)
                {
                    int id = WallMapA[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(WALL, x, y, z, WallRotA[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(WallRotA[y][z][x]), 0.0f));
                        MV1DrawModel(modelHandles[id]);
                    }
                }

                if (WallMapB[y][z][x] >= 0)
                {
                    int id = WallMapB[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(WALL, x, y, z, WallRotB[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(WallRotB[y][z][x]), 0.0f));
                        MV1DrawModel(modelHandles[id]);
                    }
                }

                if (CornerMap[y][z][x] >= 0)
                {
                    int id = CornerMap[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(CORNER, x, y, z, CornerRot[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(CornerRot[y][z][x]), 0.0f));
                        MV1DrawModel(modelHandles[id]);
                    }
                }

                if (DecoMap[y][z][x] >= 0)
                {
                    int id = DecoMap[y][z][x];

                    if (id >= 0 && id < MODEL_MAX && modelHandles[id] != -1)
                    {
                        MV1SetPosition(modelHandles[id], GetModelDrawPosition(DECO, x, y, z, DecoRot[y][z][x]));
                        MV1SetRotationXYZ(modelHandles[id], VGet(0.0f, RotToRad(DecoRot[y][z][x]), 0.0f));
                        MV1DrawModel(modelHandles[id]);
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
            MV1DrawModel(modelHandles[previewId]);
        }
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
