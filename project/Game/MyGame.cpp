#include "MyGame.h"

void MyGame::Initialize()
{
	/*基底クラスの初期化*/
	Framework::Initialize();
	/*タイトルシートの初期化*/
	//titleScene = new TitleScene();
	//titleScene->Initialize();
	// ゲームシーンの初期化
	//gameScene = new GameScene();
	//gameScene->Initialize();
	baseScene = new GameScene();
	baseScene->Initialize();
}



void MyGame::Finalize()
{
	// 各種解放
	//delete gameScene;
	//delete titleScene;
	delete baseScene;
	/*基底クラスの終了処理*/
	Framework::Finalize();
}

void MyGame::Update()
{
	/*基底クラスの更新処理*/
	Framework::Update();

	//titleScene->Update();
	//gameScene->Update();
	baseScene->Update();
}

void MyGame::PreDraw()
{
	/*基底クラスの描画前処理*/
	Framework::PreDraw();
	//titleScene->Draw();
	//gameScene->Draw();
	baseScene->Draw();
}

void MyGame::PostDraw()
{
	/*基底クラスの描画後処理*/
	Framework::PostDraw();
}


