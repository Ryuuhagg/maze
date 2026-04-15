#include "DxLib.h"
#include <vector>
#include <fstream>

const int SCREEN_W = 800;
const int SCREEN_H = 600;
const int TILE_SIZE = 32;
const int MAP_W = 20;
const int MAP_H = 15;

int mapData[MAP_H][MAP_W] = { 0 };
int selectedTile = 1;

void SaveMap(const char* filename) {
    std::ofstream ofs(filename);
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            ofs << mapData[y][x] << ",";
        }
        ofs << "\n";
    }
}

void LoadMap(const char* filename) {
    std::ifstream ifs(filename);
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            char comma;
            ifs >> mapData[y][x] >> comma;
        }
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    ChangeWindowMode(TRUE);
    SetGraphMode(SCREEN_W, SCREEN_H, 32);
    DxLib_Init();

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        ClearDrawScreen();

        int mx, my;
        GetMousePoint(&mx, &my);

        int gridX = mx / TILE_SIZE;
        int gridY = my / TILE_SIZE;


        //左クリックで配置
        if (GetMouseInput() & MOUSE_INPUT_LEFT) {
            if (gridX >= 0 && gridX < MAP_W && gridY >= 0 && gridY < MAP_H) {
                mapData[gridY][gridX] = selectedTile;
            }
        }

        // 右クリックで削除
        if (GetMouseInput() & MOUSE_INPUT_RIGHT) {
            if (gridX >= 0 && gridX < MAP_W && gridY >= 0 && gridY < MAP_H) {
                mapData[gridY][gridX] = 0;
            }
        }

        // キーでタイル切り替え
        if (CheckHitKey(KEY_INPUT_1)) selectedTile = 1;
        if (CheckHitKey(KEY_INPUT_2)) selectedTile = 2;

        // 保存
        if (CheckHitKey(KEY_INPUT_S)) SaveMap("map.csv");

        // 読み込み
        if (CheckHitKey(KEY_INPUT_L)) LoadMap("map.csv");

        // 描画
        for (int y = 0; y < MAP_H; y++) {
            for (int x = 0; x < MAP_W; x++) {
                int color = GetColor(50, 50, 50);
                if (mapData[y][x] == 1) color = GetColor(0, 200, 0);
                if (mapData[y][x] == 2) color = GetColor(200, 0, 0);

                DrawBox(x * TILE_SIZE, y * TILE_SIZE,
                    x * TILE_SIZE + TILE_SIZE,
                    y * TILE_SIZE + TILE_SIZE,
                    color, TRUE);

                DrawBox(x * TILE_SIZE, y * TILE_SIZE,
                    x * TILE_SIZE + TILE_SIZE,
                    y * TILE_SIZE + TILE_SIZE,
                    GetColor(100, 100, 100), FALSE);
            }
        }

        DrawFormatString(10, 10, GetColor(255, 255, 255),
            "Tile: %d (1:緑 2:赤) S:セーブ、L：ロード", selectedTile);

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
