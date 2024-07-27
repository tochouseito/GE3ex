#pragma once
#include "GameScene.h"
#include"TitleScene.h"
#include"BaseScene.h"
#include"SceneManager.h"
#include"Framework.h"
class MyGame:public Framework
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;
	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize()override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update()override;

	/// <summary>
	/// 描画
	/// </summary>
	void PreDraw()override;

	/// <summary>
	/// 描画
	/// </summary>
	void PostDraw()override;

private:
	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;
	BaseScene* baseScene = nullptr;
	SceneManager* sceneManager = nullptr;
};

