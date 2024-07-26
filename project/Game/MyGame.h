#pragma once
#include "WinApp.h"
#include"DirectXCommon.h"
#include "ImGuiManager.h"
#include "GameScene.h"
#include "TextureManager.h"
#include"SrvManager.h"
#include"Input.h"
#include"Audio.h"
class MyGame
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	///	ゲームプレイ
	/// </summary>
	void Operation();
	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();
private:
	D3DResourceLeakChecker* leakCheck = nullptr;
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;
	ImGuiManager* imguiManager = nullptr;
	SrvManager* srvManager = nullptr;
};

