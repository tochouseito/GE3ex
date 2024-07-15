#pragma once
#include"ViewProjection.h"
#include"WorldTransform.h"
class MainCamera
{
public:
	MainCamera() = default;
	~MainCamera();
	
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, ViewProjection* viewProjection);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// プロジェクション行列計算用のメンバ設定関数群
	/// </summary>
	void SetFovAngleY(float value) { viewProjection_->fovAngleY = value; }
	void SetAspectRatio(float value) { viewProjection_->aspectRatio = value; }
	void SetNearZ(float value) { viewProjection_->nearZ = value; }
	void SetFarZ(float value) { viewProjection_->farZ = value; }
private:
	ViewProjection* viewProjection_;
};

