#include "Input.h"
#include<assert.h>
#include<WinApp.h>
/// <summary>
/// シングルトンインスタンスの取得
/// </summary>
Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}
/// <summary>
/// 初期化
/// </summary>
void Input::Initialize() {
	HRESULT result;

	// DirectInoutの初期化
	//IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(
		WinApp::GetInstance()->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	//IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	// 入力データの形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);// 標準形式
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}
/// <summary>
/// 更新
/// </summary>
void Input::Update() {

	// 前回のキー入力を保存
	memcpy(preKey, key, sizeof(key));

	// キーボード情報の取得開始
	keyboard->Acquire();

	// 全キーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(key), key);

}
/// <summary>
/// キーの押下をチェック
/// </summary>
/// <param name="keyNumber">キー番号( DIK_0 等)</param>
/// <returns>押されているか</returns>
//bool PushKey(BYTE keyNumber) const;
bool Input::PushKey(uint8_t keyNumber)const {
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

/// <summary>
/// キーのトリガーをチェック
/// </summary>
/// <param name="keyNumber">キー番号( DIK_0 等)</param>
/// <returns>トリガーか</returns>
//bool TriggerKey(BYTE keyNumber) const;
bool Input::TriggerKey(uint8_t keyNumber) const {
	if (key[keyNumber] && !preKey[keyNumber]) {
		return true;
	}
	return false;
}