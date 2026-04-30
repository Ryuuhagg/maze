#pragma once
#include "DxLib.h"

/// <summary>
/// このスクリプトはEditorとゲームに表示するための2つで必要となるためずっとオン
/// </summary>

#pragma region ===== 画面設定 =====

const int SCREEN_W = 1280;
const int SCREEN_H = 720;
const int UI_WIDTH = 260;

#pragma endregion


#pragma region ===== マップサイズ =====

const int BLOCK_NUM_X = 20;
const int BLOCK_NUM_Y = 8;
const int BLOCK_NUM_Z = 20;

const float BLOCK_SIZE = 400.0f;

#pragma endregion


#pragma region ===== タブ種類 =====

enum TabType
{
    FLOOR = 0,
    WALL,
    CORNER,
    DECO,
    EVENT,
    TAB_MAX
};

#pragma endregion


#pragma region ===== モデル数上限 =====

const int MODEL_MAX = 32;

#pragma endregion


#pragma region ===== マップデータ =====

extern int FloorMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
extern int FloorRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

extern int WallMapA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
extern int WallMapB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

extern int WallRotA[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
extern int WallRotB[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

extern int CornerMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
extern int CornerRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

extern int DecoMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];
extern int DecoRot[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

extern int EventMap[BLOCK_NUM_Y][BLOCK_NUM_Z][BLOCK_NUM_X];

#pragma endregion


#pragma region ===== エディタ状態 =====

extern int currentLayer;
extern int currentRot;
extern int currentMapIndex;

extern int currentTab;
extern int selectedModel;

#pragma endregion


#pragma region ===== マウスカーソル位置 =====

extern int hoverX;
extern int hoverZ;

#pragma endregion


#pragma region ===== カメラ =====

extern float camRotY;
extern float camRotX;
extern float camDist;

#pragma endregion


#pragma region ===== モデル / UI画像 =====

extern int modelHandles[MODEL_MAX];
extern int paletteTex[MODEL_MAX];

#pragma endregion


#pragma region ===== UI用モデル一覧 =====

extern int tabModelList[TAB_MAX][16];
extern int tabModelCount[TAB_MAX];

#pragma endregion


#pragma region ===== 表示設定 =====

extern bool showGrid;
extern bool brushMode;

#pragma endregion


#pragma region ===== 開始地点 =====

extern int startX;
extern int startY;
extern int startZ;

#pragma endregion


#pragma region ===== ゴール地点 =====

extern int goalX;
extern int goalY;
extern int goalZ;

#pragma endregion


#pragma region ===== 共通関数 =====

int GetSelectedModel();

#pragma endregion
