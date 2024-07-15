#include "ViewProjection.h"
#include"DirectXCommon.h"
#include"Mymath.h"
/// <summary>
/// 初期化
/// </summary>
void ViewProjection::Initialize() {
	CreateConstBufferResource();
}
void ViewProjection::CreateConstBufferResource(uint32_t Instance) {
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(ConstBufferDataViewProjection) * Instance);
	// 書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	// 単位行列を書き込んでいく
	wvpData_->View = MakeIdentity4x4();
	wvpData_->Projection = MakeIdentity4x4();
	wvpData_->cameraPosition = Vector3(0.0f, 0.0f, 0.0f);
}
/// <summary>
/// マッピングする
/// </summary>
void ViewProjection::Map() {
	
}
/// <summary>
/// 行列を転送する
/// </summary>
void ViewProjection::TransferMatrix() {
	wvpData_->View = viewMatrix_;
	wvpData_->Projection = projectionMatrix_;
	wvpData_->cameraPosition = translation_;
}
void ViewProjection::UpdateMatrix() {

}
void ViewProjection::UpdateMatrixtest() {
	
}
