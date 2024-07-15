#include "GameScene.h"
#include "TextureManager.h"
#include <DirectXTex.h>
#include <cassert>
#include<fstream>
#ifdef _DEBUG
#include"imgui.h"
#endif


#include"Mymath.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete mainCamera_;
	delete sprite_;
	delete particles_;
	delete plane_;
	delete planeModel_;
	delete axis_;
	delete axisModel_;
	delete sphereModel_;
	delete sphere_;
	delete primitive_;

	delete serial;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	
	// ファイル名でテクスチャを読み込む
	textureHandle_[0] = TextureManager::Load("./Resources/uvChecker.png");
	textureHandle_[1] = TextureManager::Load("./Resources/circle.png");
	textureHandle_[2] = TextureManager::Load("./Resources/monsterball.png");
	
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	/*3D平面モデル*/
	planeModel_ = Model::CreatePlane();
	plane_ = new PlaneModel();
	plane_->Initialize(planeModel_, textureHandle_[0], &viewProjection_);

	/*3D軸モデル*/
	axisModel_ = Model::LordModel("axis.obj");
	axis_ = new axis();
	axis_->Initialize(axisModel_, textureHandle_[0], &viewProjection_);

	/*3D球モデル*/
	sphereModel_ = Model::CreateSphere(32);
	sphere_ = new SphereModel();
	sphere_->Initialize(sphereModel_, textureHandle_[2], &viewProjection_);

	/*パーティクル*/
	particles_ = new Particle();
	particles_->Initialize(&viewProjection_, textureHandle_[1]);
	
	// メインカメラの生成
	mainCamera_ = new MainCamera();
	mainCamera_->Initialize(Vector3(0.0f, 0.0f, 30.0f), &viewProjection_);
	// スプライトの生成
	sprite_ = new Sprite();
	sprite_->Initialize(Vector3(0.0f, 0.0f, 0.0f), &viewProjection_,textureHandle_[0]);

	primitive_ = new Primitive();
	primitive_->Initialize(textureHandle_[0],&viewProjection_);

	serial = new SerialToArduino();
	
	if (!serial->InitializeSerialPort("COM3")) {  // L"\\\\.\\COM3" はワイド文字列リテラルです
		return;
	}

}

void GameScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		OutputDebugStringA("OK");
	}
	
	ImGui::Begin("serialValue");
	if (ImGui::Button("Get")) {
	 std::string data = serial->ReadSerialPort();
		if (!data.empty()) {
			std::cout << "Received: " << data << std::endl;

			// データの解析
			size_t posHeartRate = data.find("Heart Rate: ");
			size_t posGSR = data.find("GSR: ");
			if (posHeartRate != std::string::npos && posGSR != std::string::npos) {
				heartRate = std::stoi(data.substr(posHeartRate + 12, posGSR - (posHeartRate + 12) - 2));
				gsrValue = std::stoi(data.substr(posGSR + 5));
			}
		}
	}
	ImGui::Text("gsrValue:%d", gsrValue);
	ImGui::Text("hrValue:%d", heartRate);
	ImGui::End();
	plane_->Update();
	axis_->Update();
	sphere_->Update();
	//primitive_->Update();
	sprite_->Update();
	//particles_->Update();
	// メインカメラの処理
	mainCamera_->Update();
	viewProjection_.TransferMatrix();
}

void GameScene::Draw() {
	plane_->Draw();
	axis_->Draw();
	sphere_->Draw();
	//primitive_->Draw();
	//particles_->Draw();
	sprite_->Draw();
}
