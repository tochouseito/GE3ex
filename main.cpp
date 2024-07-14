#pragma warning(push)
#pragma warning(disable:28251)
#include "WinApp.h"
#include"DirectXCommon.h"
#include "ImGuiManager.h"
#include "GameScene.h"
#include "TextureManager.h"
#include"SrvManager.h"
#include"Input.h"
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	CoInitializeEx(0, COINIT_MULTITHREADED);
#pragma warning(pop)
	D3DResourceLeakChecker* leakCheck = nullptr;
	leakCheck = D3DResourceLeakChecker::GetInstance();
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	GameScene* gameScene = nullptr;
	SrvManager* srvManager = nullptr;

	// ゲームウィンドウの作成
	win = WinApp:: GetInstance();
	win->CreateGameWindow();
	// DirectXの初期化
	dxCommon=DirectXCommon::GetInstance();
	dxCommon->Initialize(win);
#pragma region 汎用機能初期化
	// Srvマネージャの初期化
	srvManager = SrvManager::GetInstance();
	srvManager->Initialize(dxCommon);
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon,srvManager);
	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();
	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice(),dxCommon,srvManager);
	// ここにモデルの初期化予定
	///
	/// 今はゲームシーンで初期化している
	///
#pragma endregion
	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
	// メインループ
	while (true){
		if (win->ProcessMessage()) {
			break;
		}
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
	// 各種解放
	delete gameScene;
	dxCommon->FenceClose();
	// ImGui解放
	imguiManager->Finalize();
	CloseWindow(win->GetHwnd());
	// COMの終了処理
	CoUninitialize();
	return 0;
}