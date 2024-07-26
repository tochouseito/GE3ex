#pragma warning(push)
#pragma warning(disable:28251)
#include "WinApp.h"
#include"MyGame.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma warning(pop)
	/*ゲーム、エンジン初期化*/
	MyGame* myGame = new MyGame();
	myGame->Initialize();
	// メインループ
	while (true){
		if (WinApp::GetInstance()->ProcessMessage()) {
			break;
		}
		/*ゲームループ*/
		myGame->Operation();
	}
	/*ゲーム終了処理*/
	myGame->Finalize();
	delete myGame;
	return 0;
}