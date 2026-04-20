//メイン処理書く場所
//ヒロシです。。。
#include"DxLib.h"
#include"SceneManager.h"
#include"Constant.h"
using namespace std;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	SetMainWindowText("迷路");
	if (DxLib_Init() == -1) return -1;
	SetUseLighting(FALSE);
	SetUseBackCulling(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetDrawScreen(DX_SCREEN_BACK);
	SceneManager manager;
	manager.ChangeScene(make_unique<Title>());
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		ClearDrawScreen();

		manager.Update();
		manager.Draw();

		DrawCube3D(
			VGet(-50, -1, -50),
			VGet(50, 0, 50),
			GetColor(0, 255, 0),
			GetColor(0, 255, 0),
			TRUE
		);
		
		ScreenFlip();
		WaitTimer(16);
	}
	DxLib_End();
	return 0;
}