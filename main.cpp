//メイン処理書く場所
//ヒロシです。。。
#include"DxLib.h"
#include"SceneManager.h"
#include"Constant.h"
#include"Input.h"
using namespace std;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	SetMainWindowText("迷路");
	if (DxLib_Init() == -1) return -1;
	//SetUseLighting(FALSE);
	//SetUseBackCulling(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetDrawScreen(DX_SCREEN_BACK);
	SceneManager manager;
	Input input;
	manager.ChangeScene(make_unique<Title>());
	SetLightDirection(VGet(0.0f, -1.0f, 0.5f));

	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f,255));
	SetLightSpcColor(GetColorF(1.0f, 1.0f, 1.0f,255));

	SetGlobalAmbientLight(GetColorF(0.4f, 0.4f, 0.4f,255));

	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		ClearDrawScreen();
		input.Update();
		manager.Update();
		manager.Draw();

		ScreenFlip();
		WaitTimer(16);
	}
	DxLib_End();
	return 0;
}