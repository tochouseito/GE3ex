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
	Matrix4x4 scaleMatrix = MakeScaleMatrix(Vector3(1.0f, 1.0f, 1.0f));
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(viewProjection_->rotation_.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(viewProjection_->rotation_.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(viewProjection_->rotation_.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	Matrix4x4 translateMatrix = MakeTranslateMatrix(viewProjection_->translation_);
	Matrix4x4 cameraMatrix = Multiply(rotateXYZMatrix, Multiply(scaleMatrix, translateMatrix));
	viewProjection_->cameraMatrix = cameraMatrix;
	viewProjection_->viewMatrix_ = Inverse(cameraMatrix);
	viewProjection_->projectionMatrix_ = MakePerspectiveFovMatrix(0.45f,
		float(DirectXCommon::GetInstance()->GetBackBufferWidth()) / float(DirectXCommon::GetInstance()->GetBackBufferHeight()),
		0.1f, 100.0f);
}