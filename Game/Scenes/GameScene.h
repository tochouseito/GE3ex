#pragma once
// base
#include "DirectXCommon.h"
#include"Input.h"

// 3D
#include"Model.h"
#include"MainCamera.h"
#include"ViewProjection.h"
#include"Particle.h"

// 2D
#include"Sprite.h"

/*3Dオブジェクト*/
#include"PlaneModel.h"
#include"axis.h"
#include"SphereModel.h"
#include"Primitive.h"

/*externals*/
#include"SerialToArduino.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	
	// テクスチャハンドル
	std::string textureHandle_[3]{};

	
	Particle* particles_ = nullptr;
	Sprite* sprite_ = nullptr;
	MainCamera* mainCamera_ = nullptr;
	ViewProjection  viewProjection_;

	/*3D平面モデル*/
	Model* planeModel_ = nullptr;
	PlaneModel* plane_ = nullptr;
	/*3D軸モデル*/
	Model* axisModel_ = nullptr;
	axis* axis_ = nullptr;
	/*3D球モデル*/
	Model* sphereModel_ = nullptr;
	SphereModel* sphere_ = nullptr;

	/*ジオメトリ*/
	Primitive* primitive_ = nullptr;

	/*シリアルライブラリ*/
	//serialib serial;
	SerialToArduino* serial=nullptr;
	int heartRate = 0;
	int gsrValue = 0;
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};

