#include "DebugCamera.h"
#include"Input.h"
#include"Mymath.h"
#include"imgui.h"

void DebugCamera::Initialize(const Vector3& position, ViewProjection* viewProjection)
{
	viewProjection_ = viewProjection;
	viewProjection_->translation_ = position;
}

void DebugCamera::Update()
{
	CameraMove(rotation_,translation_,mouse);
	viewProjection_->translation_ = translation_;
	viewProjection_->rotation_ = rotation_;
}

void DebugCamera::CameraMove(Vector3& cameraRotate, Vector3& cameraTranslate, Vector2& clickPosition)
{
	// カーソルを動かすときの感度
	const float mouseSensitivity = 0.003f;
	// カメラの移動速度
	const float moveSpeed = 0.005f;

	// 各フラグ
	static bool isLeftClicked = false;
	static bool isWheelClicked = false;
	//static bool isDebugCamera = false;

	// 回転を考慮する
	Matrix4x4 rotationMatrix = MakeRotateXYZMatrix(cameraRotate);
	Vector3 X = { 1.0f, 0.0f, 0.0f };
	Vector3 Y = { 0.0f, 1.0f, 0.0f };
	Vector3 Z = { 0.0f, 0.0f, -1.0f };

	Vector3 rotatedX = Transform(X, rotationMatrix);
	Vector3 rotatedY = Transform(Y, rotationMatrix);
	Vector3 rotatedZ = Transform(Z, rotationMatrix);



	//if (isDebugCamera) {

		/// ========カメラ操作========
		// カメラの回転を更新する
		if (Input::GetInstance()->IsPressMouse(0) == 1) {
			if (!isLeftClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				clickPosition = Input::GetInstance()->GetMousePosition();
				isLeftClicked = true;
			} else {
				// マウスがクリックされている間はカメラの回転を更新する
				Vector2 currentMousePos;
				currentMousePos = Input::GetInstance()->GetMousePosition();

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraRotate.x += deltaY * mouseSensitivity;
				cameraRotate.y -= deltaX * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		} else {
			// マウスがクリックされていない場合はフラグをリセットする
			isLeftClicked = false;
		}

		// カメラの位置を更新する
		if (Input::GetInstance()->IsPressMouse(2) == 1) {
			if (!isWheelClicked) {
				// マウスがクリックされたときに現在のマウス位置を保存する
				clickPosition = Input::GetInstance()->GetMousePosition();
				isWheelClicked = true;
			} else {
				// マウスがクリックされている間はカメラの位置を更新する
				Vector2 currentMousePos;
				currentMousePos = Input::GetInstance()->GetMousePosition();

				float deltaX = static_cast<float>(currentMousePos.x - clickPosition.x);
				float deltaY = static_cast<float>(currentMousePos.y - clickPosition.y);

				cameraTranslate -= rotatedX * deltaX * mouseSensitivity;
				cameraTranslate += rotatedY * deltaY * mouseSensitivity;

				// 現在のマウス位置を保存する
				clickPosition = currentMousePos;
			}
		} else {
			// マウスがクリックされていない場合はフラグをリセットする
			isWheelClicked = false;
		}

		// マウスホイールの移動量を取得する
		int wheelDelta = -Input::GetInstance()->GetWheel();

		// マウスホイールの移動量に応じてカメラの移動を更新する
		cameraTranslate += rotatedZ * float(wheelDelta) * moveSpeed;
		/// =====================
	//}
	ImGui::Begin("camera explanation");
	ImGui::Text("PressingMouseLeftbutton : moveCameraRotate\nPressingMouseWheelbutton : moveCameraTranslate");
	ImGui::End();
}
