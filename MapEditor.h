#pragma once
#include "MapData.h"

#pragma region ===== ‰Šú‰» =====

void InitEditor();

#pragma endregion


#pragma region ===== XV =====

void UpdateEditor();

#pragma endregion


#pragma region ===== •`‰æ =====

void DrawEditor();

#pragma endregion


#pragma region ===== CSV•Û‘¶ / “Ç =====

void SaveMap();
void LoadMap(int mapIndex);

#pragma endregion


#pragma region ===== Undo =====

void PushUndo();
void UndoMap();

#pragma endregion


#pragma region ===== •Çˆ— =====

void AutoConnectWalls();
void UpdateWallTile(int y, int z, int x);

#pragma endregion


#pragma region ===== UI =====

void DrawEditorUI();
void UpdateEditorUI(int mx, int my, int lClick);

#pragma endregion


#pragma region ===== ‹¤’Êˆ— =====

void ClearCurrentLayer();
void ResetAllMap();

#pragma endregion