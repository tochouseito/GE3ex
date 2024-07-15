#include "MainCamera.h"
#include"imgui.h"
#include<numbers>
#include"Mymath.h"
#include"DirectXCommon.h"
MainCamera::~MainCamera() {

}
/// <summary>
/// 初期化
/// </summary>
void MainCamera::Initialize(const Vector3& position, ViewProjection* viewProjection) {
	viewProjection_ = viewProjection;
	viewProjection_->translation_ = position;
	viewProjection_->rotation_.y = 3.14f;
	//viewProjection_->rotation_.y = 0.0f;
	//viewProjection_->TransferMatrix();
}
/// <summary>
/// 毎フレーム処理
/// </summary>
void MainCamera::Update() {

#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui::Begin("3Dobjects");
	if (ImGui::CollapsingHeader("MainCamera")) {
		ImGui::DragFloat3("CameraTranslate", &viewProjection_->translation_.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &viewProjection_->rotation_.x, 0.01f);
	}
	ImGui::End();
#endif
}