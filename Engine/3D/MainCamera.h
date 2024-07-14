#pragma once
#include"ViewProjection.h"
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
private:
	ViewProjection* viewProjection_;
};

