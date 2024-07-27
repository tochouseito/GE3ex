#include "MyGame.h"

void MyGame::Initialize()
{
	/*基底クラスの初期化*/
	Framework::Initialize();
	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();
}



void MyGame::Finalize()
{
	// 各種解放
	delete gameScene;
	/*基底クラスの終了処理*/
	Framework::Finalize();
}

void MyGame::Update()
{
	/*基底クラスの更新処理*/
	Framework::Update();

	gameScene->Update();
}

void MyGame::PreDraw()
{
	/*基底クラスの描画前処理*/
	Framework::PreDraw();
	gameScene->Draw();
}

void MyGame::PostDraw()
{
	/*基底クラスの描画後処理*/
	Framework::PostDraw();
}


