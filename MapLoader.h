#pragma once
#include "DxLib.h"
#include "MapData.h"

/// <summary>
/// MapLoader を起動するには　MapData.h MapLoader.h/cpp mapmain.cppが必要
/// </summary>

void InitGameMap();
void LoadGameMap();
void UpdateGameMap();
void DrawGameMap();

bool HasWallEdge(int y, int z, int x, int edge);
bool CanMoveCellToCell(int y, int fromX, int fromZ, int toX, int toZ);
bool IsGoalCell(int y, int z, int x);

extern int gameCurrentMapIndex;

extern int gameStartX;
extern int gameStartY;
extern int gameStartZ;

extern int gameGoalX;
extern int gameGoalY;
extern int gameGoalZ;

VECTOR GetStartPosition();