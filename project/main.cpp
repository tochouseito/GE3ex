#pragma warning(push)
#pragma warning(disable:28251)
#include"MyGame.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#pragma warning(pop)
	/*ゲーム、エンジン初期化*/
	MyGame* myGame = new MyGame();
	myGame->Initialize();
	// メインループ
	while (true){
		if (myGame->IsEndRequest()) {
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