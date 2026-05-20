#include "MapEditor.h"
#include "DxLib.h"
#include <windows.h>

#pragma region ===== UI定数 =====

static const int TAB_W = 76;
static const int TAB_H = 28;
static const int PAD = 12;

static const int PALETTE_ICON = 48;
static const int PALETTE_STEP = 62;

#pragma endregion


#pragma region ===== フォント =====

static const char* EDITOR_FONT_PATH = "font/LightNovelPOPv2.otf";
static const char* TITLE_FONT_NAME = "ラノベPOP V2";
static const char* BODY_FONT_NAME = "メイリオ";

static int uiFont = -1;
static int uiSmallFont = -1;
static int uiTitleFont = -1;

static void InitEditorUIFont()
{
    if (uiFont != -1)
        return;

    AddFontResourceExA(EDITOR_FONT_PATH, FR_PRIVATE, NULL);

    uiFont = CreateFontToHandle(
        BODY_FONT_NAME,
        15,
        2,
        DX_FONTTYPE_ANTIALIASING
    );

    uiSmallFont = CreateFontToHandle(
        BODY_FONT_NAME,
        13,
        1,
        DX_FONTTYPE_ANTIALIASING
    );

    uiTitleFont = CreateFontToHandle(
        TITLE_FONT_NAME,
        17,
        2,
        DX_FONTTYPE_ANTIALIASING
    );
}

static void DrawText(int x, int y, const char* text, int color)
{
    InitEditorUIFont();

    if (uiFont != -1)
        DrawStringToHandle(x, y, text, color, uiFont);
    else
        DrawString(x, y, text, color);
}

static void DrawSmallText(int x, int y, const char* text, int color)
{
    InitEditorUIFont();

    if (uiSmallFont != -1)
        DrawStringToHandle(x, y, text, color, uiSmallFont);
    else
        DrawString(x, y, text, color);
}

static void DrawTitleText(int x, int y, const char* text, int color)
{
    InitEditorUIFont();

    if (uiTitleFont != -1)
        DrawStringToHandle(x, y, text, color, uiTitleFont);
    else
        DrawString(x, y, text, color);
}

#pragma endregion


#pragma region ===== UI補助 =====

static int GetUIX()
{
    return SCREEN_W - UI_WIDTH;
}

static void DrawPanelTitle(int x, int y, const char* text)
{
    DrawTitleText(x, y, text, GetColor(190, 190, 190));
    DrawLine(x, y + 22, x + UI_WIDTH - 24, y + 22, GetColor(70, 70, 70));
}

static void DrawButton(int x1, int y1, int x2, int y2, const char* text, bool active)
{
    int bg = active ? GetColor(220, 220, 80) : GetColor(70, 70, 70);
    int fg = active ? GetColor(0, 0, 0) : GetColor(235, 235, 235);

    DrawBox(x1, y1, x2, y2, bg, TRUE);
    DrawBox(x1, y1, x2, y2, GetColor(25, 25, 25), FALSE);
    DrawSmallText(x1 + 7, y1 + 6, text, fg);
}

#pragma endregion


#pragma region ===== UIクリック更新 =====

void UpdateEditorUI(int mx, int my, int lClick)
{
    if (!lClick)
        return;

    int uiX = GetUIX();

#pragma region タブ切替

    for (int i = 0; i < TAB_MAX; i++)
    {
        int col = i % 3;
        int row = i / 3;

        int x1 = uiX + PAD + col * (TAB_W + 6);
        int y1 = 32 + row * (TAB_H + 6);
        int x2 = x1 + TAB_W;
        int y2 = y1 + TAB_H;

        if (mx >= x1 && mx <= x2 &&
            my >= y1 && my <= y2)
        {
            currentTab = i;
            selectedModel = 0;
            return;
        }
    }

#pragma endregion


#pragma region 素材選択

    int px = uiX + PAD;
    int py = 112;

    for (int i = 0; i < tabModelCount[currentTab]; i++)
    {
        int y = py + i * PALETTE_STEP;

        if (mx >= px && mx <= px + PALETTE_ICON &&
            my >= y && my <= y + PALETTE_ICON)
        {
            selectedModel = i;
            return;
        }
    }

#pragma endregion


#pragma region ボタン

    if (mx >= uiX + PAD && mx <= uiX + 116 &&
        my >= 536 && my <= 562)
    {
        showGrid = !showGrid;
        return;
    }

    if (mx >= uiX + 128 && mx <= uiX + 248 &&
        my >= 536 && my <= 562)
    {
        brushMode = !brushMode;
        return;
    }

    // 2026-05-11: ON/OFFを減らし、セル編集/辺編集ボタンから直接編集に入れるよう変更。
    if (mx >= uiX + PAD && mx <= uiX + 116 &&
        my >= 568 && my <= 594)
    {
        collisionEditMode = true;
        collisionEdgeEditMode = false;
        // 2026-05-13: セル編集へ戻った時に厚みモードの選択が残らないよう追加。
        collisionDepthEditMode = false;
        showCollisionDebug = true;
        return;
    }

    if (mx >= uiX + 128 && mx <= uiX + 248 &&
        my >= 568 && my <= 594)
    {
        collisionEditMode = true;
        collisionEdgeEditMode = true;
        // 2026-05-13: 辺編集を開始した時はまず長さモードから触れるよう追加。
        collisionDepthEditMode = false;
        showCollisionDebug = true;
        return;
    }

    if (mx >= uiX + PAD && mx <= uiX + 116 &&
        my >= 600 && my <= 626)
    {
        collisionEditMode = false;
        return;
    }

    if (mx >= uiX + 128 && mx <= uiX + 248 &&
        my >= 600 && my <= 626)
    {
        // 2026-05-13: Shift操作ではなくボタンで辺の長さ編集/厚み編集を切り替えるため変更。
        collisionDepthEditMode = !collisionDepthEditMode;
        return;
    }
#pragma endregion
}

#pragma endregion


#pragma region ===== UI描画 =====

void DrawEditorUI()
{
    InitEditorUIFont();

    int uiX = GetUIX();

#pragma region 背景

    DrawBox(
        uiX, 0,
        SCREEN_W, SCREEN_H,
        GetColor(18, 18, 20),
        TRUE
    );

    DrawLine(uiX, 0, uiX, SCREEN_H, GetColor(80, 80, 80));

#pragma endregion


#pragma region タブ

    const char* tabName[TAB_MAX] =
    {
        "床",
        "壁",
        "角",
        "装飾",
        "イベント"
    };

    DrawPanelTitle(uiX + PAD, 6, "道具");

    for (int i = 0; i < TAB_MAX; i++)
    {
        int col = i % 3;
        int row = i / 3;

        int x1 = uiX + PAD + col * (TAB_W + 6);
        int y1 = 32 + row * (TAB_H + 6);
        int x2 = x1 + TAB_W;
        int y2 = y1 + TAB_H;

        DrawButton(x1, y1, x2, y2, tabName[i], i == currentTab);
    }

#pragma endregion


#pragma region 素材一覧

    DrawPanelTitle(uiX + PAD, 84, "素材一覧");

    int px = uiX + PAD;
    int py = 112;

    for (int i = 0; i < tabModelCount[currentTab]; i++)
    {
        int id = tabModelList[currentTab][i];
        int y = py + i * PALETTE_STEP;

        DrawBox(
            px - 3, y - 3,
            px + PALETTE_ICON + 3, y + PALETTE_ICON + 3,
            (selectedModel == i) ? GetColor(255, 230, 40) : GetColor(55, 55, 60),
            TRUE
        );

        DrawBox(
            px, y,
            px + PALETTE_ICON, y + PALETTE_ICON,
            GetColor(35, 35, 38),
            TRUE
        );

        if (id >= 0 && id < MODEL_MAX && paletteTex[id] > 0)
        {
            DrawExtendGraph(
                px, y,
                px + PALETTE_ICON, y + PALETTE_ICON,
                paletteTex[id],
                TRUE
            );
        }
        else
        {
            DrawBox(
                px, y,
                px + PALETTE_ICON, y + PALETTE_ICON,
                GetColor(110, 110, 110),
                TRUE
            );
        }

        if (currentTab == EVENT)
        {
            const char* name = "イベント";

            if (id == 0) name = "開始";
            else if (id == 1) name = "ゴール";

            DrawStringToHandle(
                px + 64,
                y + 16,
                name,
                GetColor(220, 220, 220),
                uiSmallFont
            );
        }
        else
        {
            DrawFormatStringToHandle(
                px + 64, y + 16,
                GetColor(220, 220, 220),
                uiSmallFont,
                "素材 %d",
                id
            );
        }

    }

#pragma endregion


#pragma region 状態

    DrawPanelTitle(uiX + PAD, 300, "状態");

    DrawFormatStringToHandle(
        uiX + PAD, 326,
        GetColor(255, 230, 40),
        uiFont,
        "選択素材 : %d",
        GetSelectedModel()
    );

    DrawFormatStringToHandle(
        uiX + PAD, 352,
        GetColor(230, 230, 230),
        uiSmallFont,
        "マップ : %d\n"
        "階層   : %d\n"
        "回転   : %d\n"
        "ブラシ : %s",
        currentMapIndex,
        currentLayer,
        currentRot,
        brushMode ? "ON" : "OFF"
    );

#pragma endregion


#pragma region 操作

    DrawPanelTitle(uiX + PAD, 430, "操作");

    DrawSmallText(uiX + PAD, 456, "Q/E    階層", GetColor(210, 210, 210));
    DrawSmallText(uiX + PAD, 476, "R      回転", GetColor(210, 210, 210));
    DrawSmallText(uiX + PAD, 496, "F5/F9  保存/読込", GetColor(210, 210, 210));

    DrawSmallText(uiX + 138, 456, "V   範囲選択", GetColor(210, 210, 210));
    DrawSmallText(uiX + 138, 476, "Ctrl+C コピー", GetColor(210, 210, 210));
    DrawSmallText(uiX + 138, 496, "Ctrl+V 貼付", GetColor(210, 210, 210));
    // 2026-05-13: Shift操作をやめ、ボタンで長さ/厚みモードを切り替える表示へ修正。
    DrawSmallText(uiX + PAD, 516, "辺:Wheel 調整", GetColor(210, 210, 210));
    DrawSmallText(uiX + 138, 516, collisionDepthEditMode ? "厚みモード" : "長さモード", GetColor(210, 210, 210));

#pragma endregion


#pragma region ボタン

    DrawButton(
        uiX + PAD, 536,
        uiX + 116, 562,
        showGrid ? "グリッドON" : "グリッドOFF",
        showGrid
    );

    DrawButton(
        uiX + 128, 536,
        uiX + 248, 562,
        brushMode ? "ブラシON" : "ブラシOFF",
        brushMode
    );

    // 2026-05-11: ON/OFFを減らし、セル編集/辺編集を直接選べるボタンへ変更。
    DrawButton(
        uiX + PAD, 568,
        uiX + 116, 594,
        "セル編集",
        collisionEditMode && !collisionEdgeEditMode
    );

    DrawButton(
        uiX + 128, 568,
        uiX + 248, 594,
        "辺編集",
        collisionEditMode && collisionEdgeEditMode
    );

    DrawButton(
        uiX + PAD, 600,
        uiX + 116, 626,
        "編集終了",
        false
    );

    // 2026-05-13: 辺編集時に長さと壁の厚みをボタンで切り替えられるよう変更。
    DrawButton(
        uiX + 128, 600,
        uiX + 248, 626,
        collisionDepthEditMode ? "厚みモード" : "長さモード",
        collisionDepthEditMode
    );
#pragma endregion


#pragma region 開始 / ゴール

    DrawPanelTitle(uiX + PAD, 640, "地点");

    DrawFormatStringToHandle(
        uiX + PAD, 664,
        GetColor(80, 255, 120),
        uiSmallFont,
        "開始  %d,%d,%d",
        startX, startY, startZ
    );

    DrawFormatStringToHandle(
        uiX + PAD, 688,
        GetColor(255, 100, 100),
        uiSmallFont,
        "ゴール %d,%d,%d",
        goalX, goalY, goalZ
    );

#pragma endregion
}

#pragma endregion
