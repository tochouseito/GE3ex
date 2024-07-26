#include "MyGame.h"

void MyGame::Initialize()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	leakCheck = D3DResourceLeakChecker::GetInstance();
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();
	// DirectXの初期化
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);
#pragma region 汎用機能初期化
	// Srvマネージャの初期化
	srvManager = SrvManager::GetInstance();
	srvManager->Initialize(dxCommon);
	// ImGuiの初期化
	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon, srvManager);
	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();
	/*音声の初期化*/
	audio = Audio::GetInstance();
	audio->Initialize();
	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice(), dxCommon, srvManager);
	// ここにモデルの初期化予定
	///
	/// 今はゲームシーンで初期化している
	///
#pragma endregion
	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
}

void MyGame::Operation()
{
	// ImGui受付開始
	imguiManager->Begin();
	// 入力関連の毎フレーム処理
	input->Update();
	// ゲームシーンの毎フレーム処理
	gameScene->Update();
	// ImGui受付終了
	imguiManager->End();
	// 描画開始
	dxCommon->PreDraw();

	srvManager->PreDraw();
	// ゲームシーンの描画
	gameScene->Draw();
	// ImGui描画
	imguiManager->Draw();
	// 描画終了
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	// 各種解放
	delete gameScene;
	dxCommon->FenceClose();
	// ImGui解放
	imguiManager->Finalize();
	CloseWindow(win->GetHwnd());
	// COMの終了処理
	CoUninitialize();
}
